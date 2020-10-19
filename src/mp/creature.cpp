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

using namespace reone::game;
using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace mp {

MultiplayerCreature::MultiplayerCreature(uint32_t id, ObjectFactory *objectFactory, SceneGraph *sceneGraph, IMultiplayerCallbacks *callbacks) :
    Creature(id, objectFactory, sceneGraph), _callbacks(callbacks) {
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

void MultiplayerCreature::playAnimation(const string &anim, int flags, float speed) {
    SpatialObject::playAnimation(anim, flags);
    _callbacks->onObjectAnimationChanged(*this, anim, flags, speed);
}

void MultiplayerCreature::updateTransform() {
    SpatialObject::updateTransform();
    _callbacks->onObjectTransformChanged(*this, _position, _heading);
}

void MultiplayerCreature::setMovementType(MovementType type) {
    if (type == _movementType) return;

    Creature::setMovementType(type);
    _callbacks->onCreatureMovementTypeChanged(*this, type);
}

void MultiplayerCreature::setTalking(bool talking) {
    if (talking == _talking) return;

    Creature::setTalking(talking);
    _callbacks->onCreatureTalkingChanged(*this, talking);
}

} // namespace mp

} // namespace reone
