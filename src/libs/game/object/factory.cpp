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

#include "reone/game/object/factory.h"

#include "reone/system/collectionutil.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

shared_ptr<Module> ObjectFactory::newModule() {
    return newObject<Module>(_game, _services);
}

shared_ptr<Item> ObjectFactory::newItem() {
    return newObject<Item>(_game, _services);
}

shared_ptr<Area> ObjectFactory::newArea(string sceneName) {
    return newObject<Area>(move(sceneName), _game, _services);
}

shared_ptr<Creature> ObjectFactory::newCreature(string sceneName) {
    return newObject<Creature>(move(sceneName), _game, _services);
}

shared_ptr<Placeable> ObjectFactory::newPlaceable(string sceneName) {
    return newObject<Placeable>(move(sceneName), _game, _services);
}

shared_ptr<Door> ObjectFactory::newDoor(string sceneName) {
    return newObject<Door>(move(sceneName), _game, _services);
}

shared_ptr<Waypoint> ObjectFactory::newWaypoint(string sceneName) {
    return newObject<Waypoint>(move(sceneName), _game, _services);
}

shared_ptr<Trigger> ObjectFactory::newTrigger(string sceneName) {
    return newObject<Trigger>(move(sceneName), _game, _services);
}

shared_ptr<Sound> ObjectFactory::newSound(string sceneName) {
    return newObject<Sound>(move(sceneName), _game, _services);
}

shared_ptr<PlaceableCamera> ObjectFactory::newCamera(string sceneName) {
    return newObject<PlaceableCamera>(move(sceneName), _game, _services);
}

shared_ptr<Encounter> ObjectFactory::newEncounter(string sceneName) {
    return newObject<Encounter>(move(sceneName), _game, _services);
}

shared_ptr<Object> ObjectFactory::getObjectById(uint32_t id) const {
    return getFromLookupOrNull(_objectById, id);
}

} // namespace game

} // namespace reone
