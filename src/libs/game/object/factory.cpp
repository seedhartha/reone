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

#include "reone/game/object/factory.h"

using namespace reone::scene;

namespace reone {

namespace game {

std::unique_ptr<Module> ObjectFactory::newModule() {
    return newObject<Module>(_game, _services);
}

std::unique_ptr<Item> ObjectFactory::newItem() {
    return newObject<Item>(_game, _services);
}

std::unique_ptr<Area> ObjectFactory::newArea(std::string sceneName) {
    return newObject<Area>(std::move(sceneName), _game, _services);
}

std::unique_ptr<Creature> ObjectFactory::newCreature(std::string sceneName) {
    return newObject<Creature>(std::move(sceneName), _game, _services);
}

std::unique_ptr<Placeable> ObjectFactory::newPlaceable(std::string sceneName) {
    return newObject<Placeable>(std::move(sceneName), _game, _services);
}

std::unique_ptr<Door> ObjectFactory::newDoor(std::string sceneName) {
    return newObject<Door>(std::move(sceneName), _game, _services);
}

std::unique_ptr<Waypoint> ObjectFactory::newWaypoint(std::string sceneName) {
    return newObject<Waypoint>(std::move(sceneName), _game, _services);
}

std::unique_ptr<Trigger> ObjectFactory::newTrigger(std::string sceneName) {
    return newObject<Trigger>(std::move(sceneName), _game, _services);
}

std::unique_ptr<Sound> ObjectFactory::newSound(std::string sceneName) {
    return newObject<Sound>(std::move(sceneName), _game, _services);
}

std::unique_ptr<PlaceableCamera> ObjectFactory::newCamera(std::string sceneName) {
    return newObject<PlaceableCamera>(std::move(sceneName), _game, _services);
}

std::unique_ptr<Encounter> ObjectFactory::newEncounter(std::string sceneName) {
    return newObject<Encounter>(std::move(sceneName), _game, _services);
}

} // namespace game

} // namespace reone
