/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "abilities.h"

namespace reone {

namespace game {

static constexpr int kDefaultAbilityScore = 8;

int CreatureAbilities::getScore(Ability ability) const {
    auto maybeScore = _scores.find(ability);
    return maybeScore != _scores.end() ? maybeScore->second : kDefaultAbilityScore;
}

int CreatureAbilities::getModifier(Ability ability) const {
    int score = getScore(ability);
    return (score - 10) / 2;
}

int CreatureAbilities::strength() const {
    return getScore(Ability::Strength);
}

int CreatureAbilities::dexterity() const {
    return getScore(Ability::Dexterity);
}

int CreatureAbilities::constitution() const {
    return getScore(Ability::Constitution);
}

int CreatureAbilities::intelligence() const {
    return getScore(Ability::Intelligence);
}

int CreatureAbilities::wisdom() const {
    return getScore(Ability::Wisdom);
}

int CreatureAbilities::charisma() const {
    return getScore(Ability::Charisma);
}

void CreatureAbilities::setScore(Ability ability, int score) {
    _scores[ability] = score;
}

} // namespace game

} // namespace reone
