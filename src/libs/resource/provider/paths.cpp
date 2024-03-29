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

#include "reone/resource/provider/paths.h"

#include "reone/resource/provider/gffs.h"

using namespace reone::resource;

namespace reone {

namespace resource {

std::shared_ptr<Path> Paths::doGet(std::string resRef) {
    auto pth = _gffs.get(resRef, ResType::Pth);
    if (!pth) {
        return nullptr;
    }
    auto pthParsed = resource::generated::parsePTH(*pth);
    return loadPath(pthParsed);
}

std::unique_ptr<Path> Paths::loadPath(const resource::generated::PTH &pth) const {
    auto path = std::make_unique<Path>();

    std::vector<int> connections;
    for (auto &connection : pth.Path_Conections) {
        int destination = connection.Destination;
        connections.push_back(destination);
    }
    for (auto &pointStruct : pth.Path_Points) {
        int connectionCount = pointStruct.Conections;
        int firstConnection = pointStruct.First_Conection;
        float x = pointStruct.X;
        float y = pointStruct.Y;

        Path::Point point;
        point.x = x;
        point.y = y;

        for (int i = 0; i < connectionCount; ++i) {
            int connectionIdx = firstConnection + i;
            int pointIdx = connections[connectionIdx];
            point.adjPoints.push_back(pointIdx);
        }
        path->points.push_back(std::move(point));
    }

    return path;
}

} // namespace resource

} // namespace reone
