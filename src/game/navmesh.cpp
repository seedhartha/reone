/*
 * Copyright © 2020 Vsevolod Kremianskii
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "navmesh.h"

#include "glm/gtx/norm.hpp"

#include "../core/log.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace game {

static const float kCellSize = 2.0f;
static const float kTestDistance = 16.0f;

NavMesh::WalkmeshWrapper::WalkmeshWrapper(const shared_ptr<Walkmesh> &walkmesh, const glm::mat4 &transform) :
    walkmesh(walkmesh), transform(transform) {
}

NavMesh::Edge::Edge(uint16_t toIndex, float length) : toIndex(toIndex), length(length) {
}

void NavMesh::add(const shared_ptr<Walkmesh> &walkmesh, const glm::mat4 &transform) {
    _walkmeshes.push_back(WalkmeshWrapper(walkmesh, transform));
}

void NavMesh::compute(const atomic_bool &cancel) {
    vector<glm::vec3> candidates;
    glm::vec3 size;

    for (auto &walkmesh : _walkmeshes) {
        AABB aabb(walkmesh.walkmesh->aabb() * walkmesh.transform);
        const glm::vec3 &min = aabb.min();
        const glm::vec3 &max = aabb.max();
        const glm::vec3 &center = aabb.center();
        size = aabb.size();

        int xCellCount = glm::floor(size.x / kCellSize);
        if (xCellCount % 2 != 0) xCellCount++;
        int xccHalf = xCellCount / 2;

        int yCellCount = glm::floor(size.y / kCellSize);
        if (yCellCount % 2 != 0) yCellCount++;
        int yccHalf = yCellCount / 2;

        if (xCellCount > 1000 || yCellCount > 1000) {
            warn("Walkmesh ignored because of high cell count");
            continue;
        }

        for (int i = -xccHalf; i <= xccHalf; ++i) {
            float x = center.x + i * kCellSize;
            for (int j = -yccHalf; j <= yccHalf; ++j) {
                float y = center.y + j * kCellSize;
                candidates.push_back(glm::vec3(x, y, center.z));
            }
        }
    }

    glm::vec3 adjusted;
    glm::vec3 intersection;
    float z = 0.0f;

    for (auto &can : candidates) {
        if (cancel.load()) return;

        for (auto &walkmesh : _walkmeshes) {
            adjusted = glm::inverse(walkmesh.transform) * glm::vec4(can, 1.0f);
            if (walkmesh.walkmesh->findObstacle(adjusted, adjusted + glm::vec3(0.0f, 0.0f, -100.0f), intersection)) break;

            if (walkmesh.walkmesh->findElevationAt(adjusted, z)) {
                _vertices.push_back(glm::vec3(adjusted.x, adjusted.y, z));
                break;
            }
        }
    }

    int vertexCount = _vertices.size();

    for (int i = 0; i < vertexCount - 1; ++i) {
        for (int j = i + 1; j < vertexCount; ++j) {
            float length = glm::distance2(glm::vec2(_vertices[i]), glm::vec2(_vertices[j]));
            if (length > 2.0f * kCellSize * kCellSize) continue;

            _edges[i].push_back(Edge(j, length));
            _edges[j].push_back(Edge(i, length));
        }
    }

    _computed = true;
}

const vector<glm::vec3> NavMesh::findPath(const glm::vec3 &from, const glm::vec3 &to) const {
    if (!_computed) {
        return vector<glm::vec3> { from, to };
    }
    uint16_t fromIdx = getNearestVertex(from);
    uint16_t toIdx = getNearestVertex(to);
    if (fromIdx == 0xffff || toIdx == 0xffff) {
        return vector<glm::vec3> { from, to };
    }

    FindPathContext ctx;
    ctx.distToOrigin = { { fromIdx, make_pair(fromIdx, 0.0f) } };
    ctx.queue.push(fromIdx);

    while (!ctx.queue.empty()) {
        uint16_t idx = ctx.queue.front();
        ctx.queue.pop();
        visit(idx, ctx);
    }
    if (ctx.distToOrigin.find(toIdx) == ctx.distToOrigin.end()) {
        return vector<glm::vec3> { from, to };
    }

    uint16_t idx = toIdx;
    vector<glm::vec3> path;

    while (true) {
        path.insert(path.begin(), _vertices[idx]);
        if (idx == fromIdx) break;

        auto &pair = ctx.distToOrigin[idx];
        idx = pair.first;
    }

    return move(path);
}

uint16_t NavMesh::getNearestVertex(const glm::vec3 &point) const {
    uint16_t index = 0xffff;
    float minDist = 0.0f;

    for (int i = 0; i < _vertices.size(); ++i) {
        float dist = glm::distance2(glm::vec2(point), glm::vec2(_vertices[i]));

        if (index == 0xffff || dist < minDist) {
            index = i;
            minDist = dist;
        }
    }

    return index;
}

void NavMesh::visit(uint16_t index, FindPathContext &ctx) const {
    if (ctx.visited.find(index) != ctx.visited.end()) return;

    float dist = ctx.distToOrigin[index].second;
    if (dist > kTestDistance) return;

    auto edges = _edges.find(index);
    if (edges != _edges.end()) {
        for (auto &edge : edges->second) {
            auto it = ctx.distToOrigin.find(edge.toIndex);
            if (it == ctx.distToOrigin.end() || it->second.second > dist + edge.length) {
                ctx.distToOrigin[edge.toIndex] = make_pair(index, dist + edge.length);
            }
            if (ctx.visited.find(edge.toIndex) == ctx.visited.end()) {
                ctx.queue.push(edge.toIndex);
            }
        }
    }

    ctx.visited.insert(index);
}

} // namespace game

} // namespace reone
