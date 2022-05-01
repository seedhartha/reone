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

#include "factory.h"

#include "area.h"
#include "camera.h"
#include "creature.h"
#include "door.h"
#include "encounter.h"
#include "item.h"
#include "module.h"
#include "placeable.h"
#include "room.h"
#include "sound.h"
#include "store.h"
#include "trigger.h"
#include "waypoint.h"

using namespace std;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Object> ObjectFactory::newArea() {
    return make_unique<Area>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newCamera() {
    return make_unique<Camera>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newCreature() {
    return make_unique<Creature>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newDoor() {
    return make_unique<Door>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newEncounter() {
    return make_unique<Encounter>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newItem() {
    return make_unique<Item>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newModule() {
    return make_unique<Module>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newPlaceable() {
    return make_unique<Placeable>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newRoom() {
    return make_unique<Room>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newSound() {
    return make_unique<Sound>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newStore() {
    return make_unique<Store>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newTrigger() {
    return make_unique<Trigger>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Object> ObjectFactory::newWaypoint() {
    return make_unique<Waypoint>(_idSeq.nextObjectId(), *this, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

} // namespace neo

} // namespace game

} // namespace reone