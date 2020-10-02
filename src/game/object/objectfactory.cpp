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

#include "objectfactory.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

ObjectFactory::ObjectFactory(GameVersion version, SceneGraph *sceneGraph, const Options &opts) :
    _version(version), _sceneGraph(sceneGraph), _options(opts) {
}

unique_ptr<Module> ObjectFactory::newModule() {
    return make_unique<Module>(_counter++, _version, this, _sceneGraph, _options.graphics);
}

unique_ptr<Area> ObjectFactory::newArea() {
    return make_unique<Area>(_counter++, _version, this, _sceneGraph, _options.graphics);
}

unique_ptr<Creature> ObjectFactory::newCreature() {
    return make_unique<Creature>(_counter++, this, _sceneGraph);
}

unique_ptr<Placeable> ObjectFactory::newPlaceable() {
    return make_unique<Placeable>(_counter++, this, _sceneGraph);
}

unique_ptr<Door> ObjectFactory::newDoor() {
    return make_unique<Door>(_counter++, _sceneGraph);
}

unique_ptr<Waypoint> ObjectFactory::newWaypoint() {
    return make_unique<Waypoint>(_counter++, _sceneGraph);
}

unique_ptr<Trigger> ObjectFactory::newTrigger() {
    return make_unique<Trigger>(_counter++, _sceneGraph);
}

unique_ptr<Item> ObjectFactory::newItem() {
    return make_unique<Item>(_counter++);
}

} // namespace game

} // namespace reone
