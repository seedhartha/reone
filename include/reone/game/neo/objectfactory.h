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

#include "types.h"

namespace reone {

namespace game {

namespace neo {

class Area;
class Camera;
class Creature;
class Door;
class Encounter;
class Item;
class Module;
class Placeable;
class Sound;
class Store;
class Trigger;
class Waypoint;

class IObjectFactory {
public:
    virtual ~IObjectFactory() = default;

    virtual Area &newArea(ObjectTag tag) = 0;
    virtual Camera &newCamera(ObjectTag tag) = 0;
    virtual Creature &newCreature(ObjectTag tag) = 0;
    virtual Door &newDoor(ObjectTag tag) = 0;
    virtual Encounter &newEncounter(ObjectTag tag) = 0;
    virtual Item &newItem(ObjectTag tag) = 0;
    virtual Module &newModule(ObjectTag tag) = 0;
    virtual Placeable &newPlaceable(ObjectTag tag) = 0;
    virtual Sound &newSound(ObjectTag tag) = 0;
    virtual Store &newStore(ObjectTag tag) = 0;
    virtual Trigger &newTrigger(ObjectTag tag) = 0;
    virtual Waypoint &newWaypoint(ObjectTag tag) = 0;
};

} // namespace neo

} // namespace game

} // namespace reone
