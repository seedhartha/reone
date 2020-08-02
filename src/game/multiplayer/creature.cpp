#include "creature.h"

#include "callbacks.h"

namespace reone {

namespace game {

MultiplayerCreature::MultiplayerCreature(uint32_t id, IMultiplayerCallbacks *callbacks) : Creature(id), _callbacks(callbacks) {
}

void MultiplayerCreature::setClientTag(const std::string &clientTag) {
    _clientTag = clientTag;
}

bool MultiplayerCreature::isControlled() const {
    return !_clientTag.empty();
}

const std::string &MultiplayerCreature::clientTag() const {
    return _clientTag;
}

void MultiplayerCreature::animate(const std::string &anim, int flags) {
    Object::animate(anim, flags);
    if (_synchronize) {
        _callbacks->onObjectAnimationChanged(*this, anim, flags);
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

} // namespace game

} // namespace reone
