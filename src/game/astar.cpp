/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/game/astar.h"

using namespace std;

namespace reone {

namespace game {

vector<glm::vec2> AStar::plotPath(const glm::vec2 &from, const glm::vec2 &to) const {
    // Early exit
    int startPointIdx = _path.indexOfClosestPoint(from);
    int endPointIdx = _path.indexOfClosestPoint(to);
    if (startPointIdx == -1 || endPointIdx == -1 || startPointIdx == endPointIdx) {
        return vector<glm::vec2> {to};
    }

    auto openSet = set<int>();
    openSet.insert(startPointIdx);

    auto cameFrom = map<int, int>();

    auto gScore = map<int, float>();
    auto fScore = map<int, float>();
    for (int i = 0; i < static_cast<int>(_path.points.size()); ++i) {
        gScore[i] = numeric_limits<float>::max();
        fScore[i] = numeric_limits<float>::max();
    }
    gScore[startPointIdx] = 0.0f;
    fScore[startPointIdx] = _path.squareDistanceBetween(startPointIdx, endPointIdx);

    while (!openSet.empty()) {
        // Find point in openSet having the lowest fScore
        // TODO: optimize
        auto openToFScore = vector<pair<int, float>>();
        for (auto &pointIdx : openSet) {
            openToFScore.push_back(make_pair(pointIdx, fScore.at(pointIdx)));
        }
        sort(openToFScore.begin(), openToFScore.end(), [](const auto &l, const auto &r) { return l.second < r.second; });        
        int currentPointIdx = openToFScore.front().first;

        // Reconstruct path and return
        if (currentPointIdx == endPointIdx) {
            auto reconstructed = list<glm::vec2>();
            reconstructed.push_back(_path.points[currentPointIdx].coords);
            while (cameFrom.count(currentPointIdx) > 0) {
                currentPointIdx = cameFrom[currentPointIdx];
                reconstructed.push_front(_path.points[currentPointIdx].coords);
            }
            auto plotted = vector<glm::vec2>(reconstructed.begin(), reconstructed.end());
            plotted.push_back(to);
            return plotted;
        }

        openSet.erase(currentPointIdx);
        for (auto &adjPointIdx : _path.points[currentPointIdx].adjPoints) {
            auto tentativeGScore = gScore.at(currentPointIdx) + _path.squareDistanceBetween(currentPointIdx, adjPointIdx);
            if (tentativeGScore < gScore.at(adjPointIdx)) {
                cameFrom[adjPointIdx] = currentPointIdx;
                gScore[adjPointIdx] = tentativeGScore;
                fScore[adjPointIdx] = tentativeGScore + _path.squareDistanceBetween(adjPointIdx, endPointIdx);
                if (openSet.count(adjPointIdx) == 0) {
                    openSet.insert(adjPointIdx);
                }
            }
        }
    }

    return vector<glm::vec2> {to};
}

} // namespace game

} // namespace reone
