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

#include "reone/game/neo/objectfactory.h"

#include "reone/game/neo/actionexecutor.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/item.h"
#include "reone/game/neo/object/module.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/game/neo/objectloader.h"

namespace reone {

namespace game {

namespace neo {

Area &ObjectFactory::newArea(ObjectTag tag) {
    return newObject<Area>(std::move(tag));
}

Camera &ObjectFactory::newCamera(ObjectTag tag) {
    return newObject<Camera>(std::move(tag));
}

Creature &ObjectFactory::newCreature(ObjectTag tag) {
    return newObject<Creature>(std::move(tag));
}

Door &ObjectFactory::newDoor(ObjectTag tag) {
    return newObject<Door>(std::move(tag));
}

Encounter &ObjectFactory::newEncounter(ObjectTag tag) {
    return newObject<Encounter>(std::move(tag));
}

Item &ObjectFactory::newItem(ObjectTag tag) {
    return newObject<Item>(std::move(tag));
}

Module &ObjectFactory::newModule(ObjectTag tag) {
    return newObject<Module>(std::move(tag));
}

Placeable &ObjectFactory::newPlaceable(ObjectTag tag) {
    return newObject<Placeable>(std::move(tag));
}

Sound &ObjectFactory::newSound(ObjectTag tag) {
    return newObject<Sound>(std::move(tag));
}

Store &ObjectFactory::newStore(ObjectTag tag) {
    return newObject<Store>(std::move(tag));
}

Trigger &ObjectFactory::newTrigger(ObjectTag tag) {
    return newObject<Trigger>(std::move(tag));
}

Waypoint &ObjectFactory::newWaypoint(ObjectTag tag) {
    return newObject<Waypoint>(std::move(tag));
}

} // namespace neo

} // namespace game

} // namespace reone
