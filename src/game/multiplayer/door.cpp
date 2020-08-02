#include "door.h"

#include "callbacks.h"

namespace reone {

namespace game {

MultiplayerDoor::MultiplayerDoor(uint32_t id, IMultiplayerCallbacks *callbacks) : Door(id), _callbacks(callbacks) {
}

void MultiplayerDoor::open(const std::shared_ptr<Object> &trigerrer) {
    Door::open(trigerrer);
    if (_synchronize) {
        _callbacks->onDoorOpen(*this, trigerrer);
    }
}

} // namespace game

} // namespace reone
