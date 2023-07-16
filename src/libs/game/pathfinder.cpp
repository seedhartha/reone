/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/pathfinder.h"

namespace reone {

namespace game {

const Pathfinder::ContextVertex &Pathfinder::Context::getVertexWithLeastTotalCostFromOpen() const {
    uint16_t bestIdx = 0xffff;
    float bestTotalCost = std::numeric_limits<float>().max();

    for (uint16_t idx : open) {
        const ContextVertex &vert = vertices.find(idx)->second;
        if (bestIdx == 0xffff || vert.totalCost < bestTotalCost) {
            bestIdx = idx;
            bestTotalCost = vert.totalCost;
        }
    }

    return vertices.find(bestIdx)->second;
}

void Pathfinder::load(const std::vector<Path::Point> &points, const std::unordered_map<int, float> &pointZ) {
    for (uint16_t i = 0; i < points.size(); ++i) {
        if (pointZ.count(i) == 0) {
            continue;
        }

        const Path::Point &point = points[i];
        glm::vec3 pointVec(point.x, point.y, pointZ.find(i)->second);
        _vertices.push_back(pointVec);

        glm::vec3 adjPointVec;
        for (auto &adjPointIdx : point.adjPoints) {
            const Path::Point &adjPoint = points[adjPointIdx];
            _adjacentVertices[i].push_back(static_cast<uint16_t>(adjPointIdx));
        }
    }
}

const std::vector<glm::vec3> Pathfinder::findPath(const glm::vec3 &from, const glm::vec3 &to) const {
    // When there are no vertices, return a path of start and end points
    if (_vertices.empty()) {
        return std::vector<glm::vec3> {from, to};
    }

    // Find vertices nearest to start and end points
    uint16_t fromIdx = getNearestVertex(from);
    uint16_t toIdx = getNearestVertex(to);

    // When start and end point have a common nearest vertex, return a path of start and end point
    if (fromIdx == toIdx) {
        return std::vector<glm::vec3> {from, to};
    }

    Context ctx;

    // Add vertex, nearest to start point, to open list
    ContextVertex fromVert;
    fromVert.index = fromIdx;
    ctx.vertices.insert(std::make_pair(fromIdx, fromVert));
    ctx.open.insert(fromIdx);

    while (!ctx.open.empty()) {
        // Extract vertex with least total cost from open list
        const ContextVertex &current = ctx.getVertexWithLeastTotalCostFromOpen();
        ctx.open.erase(current.index);

        // Add current vertex to closed list
        ctx.closed.insert(current.index);

        // Reconstruct path if current vertex is nearest to end point
        if (current.index == toIdx) {
            std::vector<glm::vec3> path;
            uint16_t idx = current.index;
            do {
                const ContextVertex &vert = ctx.vertices.find(idx)->second;
                path.push_back(_vertices[vert.index]);
                idx = vert.parentIndex;
            } while (idx != 0xffff);
            reverse(path.begin(), path.end());
            return path;
        }

        // Skip current vertex if it has no adjacent vertices
        auto maybeAdjVerts = _adjacentVertices.find(current.index);
        if (maybeAdjVerts == _adjacentVertices.end())
            continue;

        for (auto &adjVertIdx : maybeAdjVerts->second) {
            // Skip adjacent vertex if it is present in closed list
            if (ctx.closed.count(adjVertIdx) > 0)
                continue;

            ContextVertex child;
            child.index = adjVertIdx;
            child.parentIndex = current.index;
            child.distance = current.distance + glm::distance2(_vertices[current.index], _vertices[adjVertIdx]);
            child.heuristic = glm::distance2(_vertices[child.index], _vertices[toIdx]);
            child.totalCost = child.distance + child.heuristic;

            // Do nothing if adjacent vertex is present in open list and computed distance is greater
            auto maybeOpenAdjVert = ctx.open.find(adjVertIdx);
            if (maybeOpenAdjVert != ctx.open.end()) {
                const ContextVertex &openAdjVert = ctx.vertices.find(*maybeOpenAdjVert)->second;
                if (child.distance > openAdjVert.distance)
                    continue;
            }

            // Insert or update adjacent vertex in open list
            ctx.vertices.insert(std::make_pair(adjVertIdx, child));
            ctx.open.insert(adjVertIdx);
        }
    }

    // Return a path of start and end points by default
    return std::vector<glm::vec3> {from, to};
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

} // namespace game

} // namespace reone
