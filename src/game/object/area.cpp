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

#include "area.h"

#include "reone/common/exception/validation.h"
#include "reone/graphics/options.h"
#include "reone/graphics/services.h"
#include "reone/graphics/textures.h"
#include "reone/resource/gff.h"
#include "reone/resource/gffs.h"
#include "reone/resource/services.h"
#include "../../scene/fogproperties.h"
#include "../../scene/graph.h"

#include "../camerastyles.h"
#include "../layouts.h"
#include "../paths.h"
#include "../services.h"
#include "../surfaces.h"
#include "../visibilities.h"

#include "creature.h"
#include "door.h"
#include "factory.h"
#include "placeable.h"
#include "room.h"
#include "trigger.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Area::load(const string &name) {
    info("Loading area " + name);

    // ARE, GIT, LYT, PTH, VIS

    auto are = _resourceSvc.gffs.get(name, ResourceType::Are);
    if (!are) {
        throw ValidationException("ARE not found: " + name);
    }

    auto git = _resourceSvc.gffs.get(name, ResourceType::Git);
    if (!git) {
        throw ValidationException("GIT not found: " + name);
    }

    auto layout = _gameSvc.layouts.get(name);
    if (!layout) {
        throw ValidationException("LYT not found: " + name);
    }

    auto path = _gameSvc.paths.get(name);
    if (!path) {
        warn("PTH not found: " + name);
    }
    _path = path.get();

    auto visibility = _gameSvc.visibilities.get(name);
    if (visibility) {
        _visibility = *visibility;
    } else {
        warn("VIS not found: " + name);
    }

    // Grass

    auto grassTexture = shared_ptr<Texture>();
    auto grassTexName = are->getString("Grass_TexName");
    if (!grassTexName.empty()) {
        grassTexture = _graphicsSvc.textures.get(grassTexName, TextureUsage::Diffuse);
    }

    auto grass = GrassProperties();
    grass.density = are->getFloat("Grass_Density");
    grass.quadSize = are->getFloat("Grass_QuadSize");
    grass.probabilities[0] = are->getFloat("Grass_Prob_UL");
    grass.probabilities[1] = are->getFloat("Grass_Prob_UR");
    grass.probabilities[2] = are->getFloat("Grass_Prob_LL");
    grass.probabilities[3] = are->getFloat("Grass_Prob_LR");
    grass.materials = _gameSvc.surfaces.getGrassSurfaces();
    grass.texture = grassTexture.get();

    // Fog

    _fog.enabled = are->getBool("SunFogOn");
    _fog.nearPlane = are->getFloat("SunFogNear");
    _fog.farPlane = are->getFloat("SunFogFar");
    _fog.color = are->getColor("SunFogColor");

    // Rooms

    auto areRooms = are->getList("Rooms");
    for (auto &areRoom : areRooms) {
        auto roomName = areRoom->getString("RoomName");
        auto lytRoom = layout->findByName(roomName);
        if (!lytRoom) {
            continue;
        }
        auto &room = static_cast<Room &>(*_objectFactory.newRoom());
        room.setSceneGraph(_sceneGraph);
        room.loadFromLyt(*lytRoom, grass);
        _rooms.push_back(&room);
    }

    // Main camera

    auto cameraStyleIdx = are->getInt("CameraStyle");
    auto cameraStyle = _gameSvc.cameraStyles.get(cameraStyleIdx);
    float aspect = _graphicsOpt.width / static_cast<float>(_graphicsOpt.height);
    auto &mainCamera = static_cast<Camera &>(*_objectFactory.newCamera());
    mainCamera.setSceneGraph(_sceneGraph);
    mainCamera.loadFromStyle(*cameraStyle);
    _mainCamera = &mainCamera;

    // Creatures

    auto gitCreatures = git->getList("Creature List");
    for (auto &gitCreature : gitCreatures) {
        auto &creature = static_cast<Creature &>(*_objectFactory.newCreature());
        creature.setSceneGraph(_sceneGraph);
        creature.loadFromGit(*gitCreature);
        _objects.push_back(&creature);
    }

    // Placeables

    auto gitPlaceables = git->getList("Placeable List");
    for (auto &gitPlaceable : gitPlaceables) {
        auto &placeable = static_cast<Placeable &>(*_objectFactory.newPlaceable());
        placeable.setSceneGraph(_sceneGraph);
        placeable.loadFromGit(*gitPlaceable);
        _objects.push_back(&placeable);
    }

    // Doors

    auto gitDoors = git->getList("Door List");
    for (auto &gitDoor : gitDoors) {
        auto &door = static_cast<Door &>(*_objectFactory.newDoor());
        door.setSceneGraph(_sceneGraph);
        door.loadFromGit(*gitDoor);
        _objects.push_back(&door);
    }

    // Triggers

    auto gitTriggers = git->getList("TriggerList");
    for (auto &gitTrigger : gitTriggers) {
        auto &trigger = static_cast<Trigger &>(*_objectFactory.newTrigger());
        trigger.setSceneGraph(_sceneGraph);
        trigger.loadFromGit(*gitTrigger);
        _objects.push_back(&trigger);
    }
}

} // namespace game

} // namespace reone
