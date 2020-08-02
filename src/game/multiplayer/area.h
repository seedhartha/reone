#pragma once

#include "../area.h"

#include "creature.h"

namespace reone {

namespace game {

class MultiplayerArea : public Area {
public:
    MultiplayerArea(resources::GameVersion version, const std::string &name, IMultiplayerCallbacks *callbacks);

    void execute(const Command &cmd);

    const std::shared_ptr<Object> findCreatureByClientTag(const std::string &clientTag) const;

private:
    IMultiplayerCallbacks *_callbacks { nullptr };

    std::shared_ptr<Creature> makeCreature() override;
    std::shared_ptr<Door> makeDoor() override;
    void updateCreature(Creature &creature, float dt) override;

    void executeLoadCreature(const Command &cmd);
    void executeSetPlayerRole(const Command &cmd);
    void executeSetObjectTransform(const Command &cmd);
    void executeSetObjectAnimation(const Command &cmd);
    void executeSetCreatureMovementType(const Command &cmd);
    void executeSetDoorOpen(const Command &cmd);
};

} // namespace game

} // namespace reone
