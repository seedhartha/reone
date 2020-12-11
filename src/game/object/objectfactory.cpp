/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../game.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

ObjectFactory::ObjectFactory(Game *game, SceneGraph *sceneGraph) :
    _game(game), _sceneGraph(sceneGraph) {

    if (!game) {
        throw invalid_argument("game must not be null");
    }
    if (!sceneGraph) {
        throw invalid_argument("sceneGraph must not be null");
    }
}

unique_ptr<Module> ObjectFactory::newModule() {
    return make_unique<Module>(_counter++, _game);
}

unique_ptr<Area> ObjectFactory::newArea() {
    return make_unique<Area>(_counter++, _game);
}

unique_ptr<Creature> ObjectFactory::newCreature() {
    return make_unique<Creature>(_counter++, this, _sceneGraph);
}

unique_ptr<Placeable> ObjectFactory::newPlaceable() {
    return make_unique<Placeable>(_counter++, this, _sceneGraph);
}

unique_ptr<Door> ObjectFactory::newDoor() {
    return make_unique<Door>(_counter++, this, _sceneGraph);
}

unique_ptr<Waypoint> ObjectFactory::newWaypoint() {
    return make_unique<Waypoint>(_counter++, this, _sceneGraph);
}

unique_ptr<Trigger> ObjectFactory::newTrigger() {
    return make_unique<Trigger>(_counter++, this, _sceneGraph);
}

unique_ptr<Item> ObjectFactory::newItem() {
    return make_unique<Item>(_counter++);
}

unique_ptr<Sound> ObjectFactory::newSound() {
    return make_unique<Sound>(_counter++, this, _sceneGraph);
}

unique_ptr<CameraObject> ObjectFactory::newCamera() {
    return make_unique<CameraObject>(_counter++, this, _sceneGraph);
}

} // namespace game

} // namespace reone
