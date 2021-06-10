/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "pathfinder.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

Pathfinder::Edge::Edge(uint16_t toIndex, float length) : toIndex(toIndex), length(length) {
}

void Pathfinder::load(const vector<Path::Point> &points, const unordered_map<int, float> &pointZ) {
    for (uint16_t i = 0; i < points.size(); ++i) {
        const Path::Point &point = points[i];
        glm::vec3 pointVec(point.x, point.y, pointZ.find(i)->second);
        _vertices.push_back(pointVec);

        glm::vec3 adjPointVec;
        for (auto &adjPointIdx : point.adjPoints) {
            const Path::Point &adjPoint = points[adjPointIdx];
            adjPointVec = glm::vec3(adjPoint.x, adjPoint.y, pointZ.find(adjPointIdx)->second);
            float distance = glm::distance2(pointVec, adjPointVec);
            _edges[i].push_back({ static_cast<uint16_t>(adjPointIdx), distance });
        }
    }
}

const vector<glm::vec3> Pathfinder::findPath(const glm::vec3 &from, const glm::vec3 &to) const {
    if (_vertices.empty()) {
        return vector<glm::vec3> { from, to };
    }
    uint16_t fromIdx = getNearestVertex(from);
    uint16_t toIdx = getNearestVertex(to);

    if (fromIdx == toIdx) {
        return vector<glm::vec3> { from, to };
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
        return vector<glm::vec3> { from, to };
    }

    uint16_t idx = toIdx;
    vector<glm::vec3> path;

    while (true) {
        path.insert(path.begin(), _vertices[idx]);
        if (idx == fromIdx) break;

        auto &pair = ctx.fromToDistance[idx];
        idx = pair.first;
    }

    return move(path);
}

uint16_t Pathfinder::getNearestVertex(const glm::vec3 &point) const {
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

void Pathfinder::visit(uint16_t index, FindPathContext &ctx) const {
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
