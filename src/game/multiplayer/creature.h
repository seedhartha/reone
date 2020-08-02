#pragma once

#include "../object/creature.h"

namespace reone {

namespace game {

class IMultiplayerCallbacks;

class MultiplayerCreature : public Creature {
public:
    MultiplayerCreature(uint32_t id, IMultiplayerCallbacks *callbacks);

    void setClientTag(const std::string &clientTag);

    bool isControlled() const;
    const std::string &clientTag() const;

private:
    IMultiplayerCallbacks *_callbacks { nullptr };;
    std::string _clientTag;

    void animate(const std::string &anim, int flags) override;
    void updateTransform() override;
    void setMovementType(MovementType type) override;
};

} // namespace game

} // namespace reone
