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

static constexpr int kDefaultSkillRank = 0;

bool CreatureAttributes::hasSkill(SkillType skill) const {
    return getSkillRank(skill) > 0;
}

int CreatureAttributes::getSkillRank(SkillType skill) const {
    return getFromLookupOrElse(_skillRanks, skill, kDefaultSkillRank);
}

int CreatureAttributes::computerUse() const {
    return getSkillRank(SkillType::ComputerUse);
}

int CreatureAttributes::demolitions() const {
    return getSkillRank(SkillType::Demolitions);
}

int CreatureAttributes::stealth() const {
    return getSkillRank(SkillType::Stealth);
}

int CreatureAttributes::awareness() const {
    return getSkillRank(SkillType::Awareness);
}

int CreatureAttributes::persuade() const {
    return getSkillRank(SkillType::Persuade);
}

int CreatureAttributes::repair() const {
    return getSkillRank(SkillType::Repair);
}

int CreatureAttributes::security() const {
    return getSkillRank(SkillType::Security);
}

int CreatureAttributes::treatInjury() const {
    return getSkillRank(SkillType::TreatInjury);
}

void CreatureAttributes::setSkillRank(SkillType skill, int rank) {
    _skillRanks[skill] = rank;
}

} // namespace game

} // namespace reone
