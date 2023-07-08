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

#pragma once

#include "reone/graphics/types.h"
#include "reone/resource/types.h"

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

struct ServicesView;

class ObjectFactory {
public:
    ObjectFactory(Game &game, ServicesView &services) :
        _game(game),
        _services(services) {
    }

    std::unique_ptr<Module> newModule();
    std::unique_ptr<Item> newItem();

    std::unique_ptr<Area> newArea(std::string sceneName = kSceneMain);
    std::unique_ptr<Creature> newCreature(std::string sceneName = kSceneMain);
    std::unique_ptr<Placeable> newPlaceable(std::string sceneName = kSceneMain);
    std::unique_ptr<Door> newDoor(std::string sceneName = kSceneMain);
    std::unique_ptr<Waypoint> newWaypoint(std::string sceneName = kSceneMain);
    std::unique_ptr<Trigger> newTrigger(std::string sceneName = kSceneMain);
    std::unique_ptr<Sound> newSound(std::string sceneName = kSceneMain);
    std::unique_ptr<PlaceableCamera> newCamera(std::string sceneName = kSceneMain);
    std::unique_ptr<Encounter> newEncounter(std::string sceneName = kSceneMain);

private:
    Game &_game;
    ServicesView &_services;

    uint32_t _counter {2}; // ids 0 and 1 are reserved

    template <class T, class... Args>
    std::unique_ptr<T> newObject(Args &&...args) {
        return std::make_unique<T>(_counter++, std::forward<Args>(args)...);
    }
};

} // namespace game

} // namespace reone
