#pragma once

#include "object/creature.h"

namespace reone {

namespace game {

constexpr static size_t MAX_NUM_FACTION = 25;

bool getIsEnemy(const std::shared_ptr<Creature>& oTarget, const std::shared_ptr<Creature>& oSource);

} // namespace game

} // namespace reone
