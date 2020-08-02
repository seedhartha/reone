#pragma once

#include "../object/door.h"

namespace reone {

namespace game {

class IMultiplayerCallbacks;

class MultiplayerDoor : public Door {
public:
    MultiplayerDoor(uint32_t id, IMultiplayerCallbacks *callbacks);

    void open(const std::shared_ptr<Object> &trigerrer) override;

private:
    IMultiplayerCallbacks *_callbacks { nullptr };;
};

} // namespace game

} // namespace reone
