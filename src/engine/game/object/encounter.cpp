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

#include "encounter.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/resources.h"

#include "../game.h"

using namespace std;

using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

Encounter::Encounter(
    uint32_t id,
    Game *game,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph
) :
    SpatialObject(id, ObjectType::Encounter, game, objectFactory, sceneGraph) {
}

void Encounter::loadFromGIT(const GffStruct &gffs) {
    string blueprintResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(blueprintResRef);

    loadPositionFromGIT(gffs);
    loadGeometryFromGIT(gffs);
}

void Encounter::loadFromBlueprint(const string &blueprintResRef) {
    shared_ptr<GffStruct> ute(_game->resources().getGFF(blueprintResRef, ResourceType::Ute));
    if (ute) {
        loadUTE(*ute);
    }
}

void Encounter::loadPositionFromGIT(const GffStruct &gffs) {
    float x = gffs.getFloat("XPosition");
    float y = gffs.getFloat("YPosition");
    float z = gffs.getFloat("ZPosition");
    _position = glm::vec3(x, y, z);
    updateTransform();
}

void Encounter::loadGeometryFromGIT(const GffStruct &gffs) {
    for (auto &point : gffs.getList("Geometry")) {
        float x = point->getFloat("X");
        float y = point->getFloat("Y");
        float z = point->getFloat("Z");
        _geometry.push_back(glm::vec3(x, y, z));
    }
}

void Encounter::loadSpawnPointsFromGIT(const GffStruct &gffs) {
    for (auto &pointGffs : gffs.getList("SpawnPointList")) {
        float x = pointGffs->getFloat("X");
        float y = pointGffs->getFloat("Y");
        float z = pointGffs->getFloat("Z");
        float orientation = pointGffs->getFloat("Orientation");

        SpawnPoint point;
        point.position = glm::vec3(x, y, z);
        point.orientation = glm::angleAxis(orientation, glm::vec3(0.0f, 0.0f, 1.0f)); // TODO: validate
        _spawnPoints.push_back(move(point));
    }
}

} // namespace game

} // namespace reone
