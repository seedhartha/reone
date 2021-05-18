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

bool CreatureAttributes::hasSkill(Skill skill) const {
    return getSkillRank(skill) > 0;
}

int CreatureAttributes::getSkillRank(Skill skill) const {
    return getFromLookupOrElse(_skillRanks, skill, kDefaultSkillRank);
}

int CreatureAttributes::computerUse() const {
    return getSkillRank(Skill::ComputerUse);
}

int CreatureAttributes::demolitions() const {
    return getSkillRank(Skill::Demolitions);
}

int CreatureAttributes::stealth() const {
    return getSkillRank(Skill::Stealth);
}

int CreatureAttributes::awareness() const {
    return getSkillRank(Skill::Awareness);
}

int CreatureAttributes::persuade() const {
    return getSkillRank(Skill::Persuade);
}

int CreatureAttributes::repair() const {
    return getSkillRank(Skill::Repair);
}

int CreatureAttributes::security() const {
    return getSkillRank(Skill::Security);
}

int CreatureAttributes::treatInjury() const {
    return getSkillRank(Skill::TreatInjury);
}

void CreatureAttributes::setSkillRank(Skill skill, int rank) {
    _skillRanks[skill] = rank;
}

} // namespace game

} // namespace reone
