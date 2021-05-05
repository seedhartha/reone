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

#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "../../scene/scenegraph.h"
#include "../../graphics/types.h"
#include "../../resource/types.h"

#include "../types.h"

#include "area.h"
#include "creature.h"
#include "door.h"
#include "encounter.h"
#include "module.h"
#include "placeable.h"
#include "placeablecamera.h"
#include "sound.h"
#include "trigger.h"
#include "waypoint.h"

namespace reone {

namespace game {

class Game;

class ObjectFactory {
public:
    ObjectFactory(Game *game, scene::SceneGraph *sceneGraph);

    std::shared_ptr<Module> newModule();
    std::shared_ptr<Area> newArea();
    std::shared_ptr<Creature> newCreature();
    std::shared_ptr<Placeable> newPlaceable();
    std::shared_ptr<Door> newDoor();
    std::shared_ptr<Waypoint> newWaypoint();
    std::shared_ptr<Trigger> newTrigger();
    std::shared_ptr<Item> newItem();
    std::shared_ptr<Sound> newSound();
    std::shared_ptr<PlaceableCamera> newCamera();
    std::shared_ptr<Encounter> newEncounter();

    std::shared_ptr<Object> getObjectById(uint32_t id) const;

private:
    Game *_game;
    scene::SceneGraph *_sceneGraph;

    uint32_t _counter { 2 }; // ids 0 and 1 are reserved
    std::unordered_map<uint32_t, std::shared_ptr<Object>> _objectById;

    template <class T, class ...Args>
    std::shared_ptr<T> newObject(Args &&... args) {
        uint32_t id = _counter++;
        std::shared_ptr<T> object(std::make_shared<T>(id, std::forward<Args>(args)...));
        _objectById.insert(std::make_pair(id, object));
        return move(object);
    }
};

} // namespace game

} // namespace reone
