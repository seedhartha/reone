#pragma once

#include "creature.h"
#include "door.h"

namespace reone {

namespace game {

class IMultiplayerCallbacks {
public:
    virtual void onObjectTransformChanged(const Object &object, const glm::vec3 &position, float heading) = 0;
    virtual void onObjectAnimationChanged(const Object &object, const std::string &anim, int flags) = 0;
    virtual void onCreatureMovementTypeChanged(const MultiplayerCreature &creature, MovementType type) = 0;
    virtual void onDoorOpen(const MultiplayerDoor &door, const std::shared_ptr<Object> &trigerrer) = 0;
};

} // namespace game

} // namespace reone
