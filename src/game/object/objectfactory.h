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

#pragma once

#include <cstdint>
#include <memory>

#include "../../system/scene/scenegraph.h"
#include "../../system/render/types.h"
#include "../../system/resource/types.h"

#include "../types.h"

#include "area.h"
#include "creature.h"
#include "door.h"
#include "module.h"
#include "placeable.h"
#include "trigger.h"
#include "waypoint.h"

namespace reone {

namespace game {

class Game;

class ObjectFactory {
public:
    ObjectFactory(resource::GameVersion version, Game *game, scene::SceneGraph *sceneGraph, const render::GraphicsOptions &opts);

    std::unique_ptr<Module> newModule();
    std::unique_ptr<Area> newArea();
    std::unique_ptr<Creature> newCreature();
    std::unique_ptr<Placeable> newPlaceable();
    std::unique_ptr<Door> newDoor();
    std::unique_ptr<Waypoint> newWaypoint();
    std::unique_ptr<Trigger> newTrigger();
    std::unique_ptr<Item> newItem();

private:
    resource::GameVersion _version { resource::GameVersion::KotOR };
    Game *_game { nullptr };
    scene::SceneGraph *_sceneGraph { nullptr };
    render::GraphicsOptions _options;
    uint32_t _counter { 2 }; // ids 0 and 1 are reserved

    ObjectFactory(const ObjectFactory &) = delete;
    ObjectFactory &operator=(const ObjectFactory &) = delete;
};

} // namespace game

} // namespace reone
