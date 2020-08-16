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

#include "creature.h"

#include "callbacks.h"

using namespace std;

namespace reone {

namespace game {

MultiplayerCreature::MultiplayerCreature(uint32_t id, IMultiplayerCallbacks *callbacks) : Creature(id), _callbacks(callbacks) {
}

void MultiplayerCreature::setClientTag(const string &clientTag) {
    _clientTag = clientTag;
}

bool MultiplayerCreature::isControlled() const {
    return !_clientTag.empty();
}

const string &MultiplayerCreature::clientTag() const {
    return _clientTag;
}

void MultiplayerCreature::animate(const string &anim, int flags, float speed) {
    Object::animate(anim, flags);
    if (_synchronize) {
        _callbacks->onObjectAnimationChanged(*this, anim, flags, speed);
    }
}

void MultiplayerCreature::updateTransform() {
    Object::updateTransform();
    if (_synchronize) {
        _callbacks->onObjectTransformChanged(*this, _position, _heading);
    }
}

void MultiplayerCreature::setMovementType(MovementType type) {
    if (type == _movementType) return;

    Creature::setMovementType(type);
    if (_synchronize) {
        _callbacks->onCreatureMovementTypeChanged(*this, type);
    }
}

void MultiplayerCreature::setTalking(bool talking) {
    if (talking == _talking) return;

    Creature::setTalking(talking);
    if (_synchronize) {
        _callbacks->onCreatureTalkingChanged(*this, talking);
    }
}

} // namespace game

} // namespace reone
