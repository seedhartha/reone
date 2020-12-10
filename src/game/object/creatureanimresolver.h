/*
 * Copyright (c) 2020 The reone project contributors
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

#include <string>

#include "../types.h"

namespace reone {

namespace game {

class Creature;

class CreatureAnimationResolver {
public:
    CreatureAnimationResolver(const Creature *creature);

    std::string getDieAnimation() const;
    std::string getDeadAnimation() const;
    std::string getPauseAnimation() const;
    std::string getRunAnimation() const;
    std::string getWalkAnimation() const;

    std::string getUnlockDoorAnimation() const;
    std::string getTalkAnimation() const;
    std::string getHeadTalkAnimation() const;

    // Attack animations

    std::string getDuelAttackAnimation() const;
    std::string getBashAttackAnimation() const;
    std::string getDodgeAnimation() const;
    std::string getKnockdownAnimation() const;

    // END Attack animations

private:
    const Creature *_creature { nullptr };

    bool getWeaponInfo(WeaponType &type, WeaponWield &wield) const;
    int getWeaponWieldNumber(WeaponWield wield) const;
};

} // namespace game

} // namespace reone
