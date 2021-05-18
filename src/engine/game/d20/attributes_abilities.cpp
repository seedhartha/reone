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

#include "attributes.h"

#include "../../common/collectionutil.h"

namespace reone {

namespace game {

static constexpr int kDefaultAbilityScore = 8;

int CreatureAttributes::getAbilityScore(Ability ability) const {
    return getFromLookupOrElse(_abilityScores, ability, kDefaultAbilityScore);
}

int CreatureAttributes::getAbilityModifier(Ability ability) const {
    int score = getAbilityScore(ability);
    return (score - 10) / 2;
}

int CreatureAttributes::strength() const {
    return getAbilityScore(Ability::Strength);
}

int CreatureAttributes::dexterity() const {
    return getAbilityScore(Ability::Dexterity);
}

int CreatureAttributes::constitution() const {
    return getAbilityScore(Ability::Constitution);
}

int CreatureAttributes::intelligence() const {
    return getAbilityScore(Ability::Intelligence);
}

int CreatureAttributes::wisdom() const {
    return getAbilityScore(Ability::Wisdom);
}

int CreatureAttributes::charisma() const {
    return getAbilityScore(Ability::Charisma);
}

void CreatureAttributes::setAbilityScore(Ability ability, int score) {
    _abilityScores[ability] = score;
}

} // namespace game

} // namespace reone
