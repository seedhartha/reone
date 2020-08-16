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

#include "creature.h"
#include "door.h"

namespace reone {

namespace game {

class IMultiplayerCallbacks {
public:
    virtual void onObjectTransformChanged(const Object &object, const glm::vec3 &position, float heading) = 0;
    virtual void onObjectAnimationChanged(const Object &object, const std::string &anim, int flags, float speed) = 0;
    virtual void onCreatureMovementTypeChanged(const MultiplayerCreature &creature, MovementType type) = 0;
    virtual void onDoorOpen(const MultiplayerDoor &door, const std::shared_ptr<Object> &trigerrer) = 0;
    virtual void onCreatureTalkingChanged(const MultiplayerCreature &creature, bool talking) = 0;
};

} // namespace game

} // namespace reone
