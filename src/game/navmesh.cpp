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

namespace reone {

namespace game {

NavMesh::Edge::Edge(uint16_t toIndex, float length) : toIndex(toIndex), length(length) {
}

void NavMesh::load(const Paths &paths) {
    const vector<Paths::Point> &points = paths.points();
    for (uint16_t i = 0; i < points.size(); ++i) {
        const Paths::Point &point = points[i];
        _vertices.push_back(glm::vec2(point.x, point.y));

        for (auto &adjPointIdx : point.adjPoints) {
            const Paths::Point &adjPoint = points[adjPointIdx];
            float distance = glm::distance2(glm::vec2(point.x, point.y), glm::vec2(adjPoint.x, adjPoint.y));

            _edges[i].push_back({ static_cast<uint16_t>(adjPointIdx), distance });
        }
    }
}

const vector<glm::vec2> NavMesh::findPath(const glm::vec2 &from, const glm::vec2 &to) const {
    if (_vertices.empty()) {
        return vector<glm::vec2> { from, to };
    }
    uint16_t fromIdx = getNearestVertex(from);
    uint16_t toIdx = getNearestVertex(to);

    if (fromIdx == toIdx) {
        return vector<glm::vec2> { from, to };
    }
    FindPathContext ctx;
    ctx.fromToDistance = { { fromIdx, make_pair(fromIdx, 0.0f) } };
    ctx.queue.push(fromIdx);

    while (!ctx.queue.empty()) {
        uint16_t idx = ctx.queue.front();
        ctx.queue.pop();
        visit(idx, ctx);
    }
    if (ctx.fromToDistance.find(toIdx) == ctx.fromToDistance.end()) {
        return vector<glm::vec2> { from, to };
    }

    uint16_t idx = toIdx;
    vector<glm::vec2> path;

    while (true) {
        path.insert(path.begin(), _vertices[idx]);
        if (idx == fromIdx) break;

        auto &pair = ctx.fromToDistance[idx];
        idx = pair.first;
    }

    return move(path);
}

uint16_t NavMesh::getNearestVertex(const glm::vec2 &point) const {
    uint16_t index = 0xffff;
    float minDist = 0.0f;

    for (int i = 0; i < _vertices.size(); ++i) {
        float dist = glm::distance2(point, _vertices[i]);

        if (index == 0xffff || dist < minDist) {
            index = i;
            minDist = dist;
        }
    }

    return index;
}

void NavMesh::visit(uint16_t index, FindPathContext &ctx) const {
    if (ctx.visited.find(index) != ctx.visited.end()) return;

    float dist = ctx.fromToDistance[index].second;

    auto edges = _edges.find(index);
    if (edges != _edges.end()) {
        for (auto &edge : edges->second) {
            auto it = ctx.fromToDistance.find(edge.toIndex);
            if (it == ctx.fromToDistance.end() || it->second.second > dist + edge.length) {
                ctx.fromToDistance[edge.toIndex] = make_pair(index, dist + edge.length);
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
