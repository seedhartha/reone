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

#include "skills.h"

namespace reone {

namespace game {

constexpr int kDefaultSkillRank = 0;

bool CreatureSkills::contains(Skill skill) const {
    auto maybeRank = _ranks.find(skill);
    return maybeRank != _ranks.end() ? maybeRank->second > 0 : false;
}

int CreatureSkills::getRank(Skill skill) const {
    auto maybeRank = _ranks.find(skill);
    return maybeRank != _ranks.end() ? maybeRank->second : kDefaultSkillRank;
}

int CreatureSkills::computerUse() const {
    return getRank(Skill::ComputerUse);
}

int CreatureSkills::demolitions() const {
    return getRank(Skill::Demolitions);
}

int CreatureSkills::stealth() const {
    return getRank(Skill::Stealth);
}

int CreatureSkills::awareness() const {
    return getRank(Skill::Awareness);
}

int CreatureSkills::persuade() const {
    return getRank(Skill::Persuade);
}

int CreatureSkills::repair() const {
    return getRank(Skill::Repair);
}

int CreatureSkills::security() const {
    return getRank(Skill::Security);
}

int CreatureSkills::treatInjury() const {
    return getRank(Skill::TreatInjury);
}

void CreatureSkills::setRank(Skill skill, int rank) {
    _ranks[skill] = rank;
}

} // namespace game

} // namespace reone
