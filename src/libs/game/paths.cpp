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

#include "reone/game/paths.h"

#include "reone/game/schema/pth.h"
#include "reone/resource/gffs.h"

using namespace reone::resource;

namespace reone {

namespace game {

std::shared_ptr<Path> Paths::doGet(std::string resRef) {
    auto pth = _gffs.get(resRef, ResourceType::Pth);
    if (!pth) {
        return nullptr;
    }
    return loadPath(*pth);
}

std::unique_ptr<Path> Paths::loadPath(const Gff &pth) const {
    auto path = std::make_unique<Path>();
    auto parsed = parsePTH(pth);

    std::vector<int> connections;
    for (auto &connection : pth.getList("Path_Conections")) {
        int destination = connection->getInt("Destination");
        connections.push_back(destination);
    }
    for (auto &pointGffs : pth.getList("Path_Points")) {
        int connectionCount = pointGffs->getInt("Conections");
        int firstConnection = pointGffs->getInt("First_Conection");
        float x = pointGffs->getFloat("X");
        float y = pointGffs->getFloat("Y");

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

    return std::move(path);
}

} // namespace game

} // namespace reone
