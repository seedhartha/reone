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

#include <algorithm>

using namespace std;

namespace reone {

namespace game {

constexpr int kDefaultAbilityScore = 8;
constexpr int kDefaultSkillRank = 0;

void CreatureAttributes::addClassLevels(ClassType clazz, int levels) {
    for (int i = 0; i < static_cast<int>(_classLevels.size()); ++i) {
        if (_classLevels[i].first == clazz) {
            _classLevels[i].second += levels;
            return;
        }
    }
    _classLevels.push_back(make_pair(clazz, levels));
    computeHitDice();
}

void CreatureAttributes::computeHitDice() {
    _hitDice = 0;
    for (auto &classLevel : _classLevels) {
        _hitDice += classLevel.second;
    }
}

int CreatureAttributes::getAbilityScore(Ability ability) const {
    auto maybeAbility = _abilities.find(ability);
    return maybeAbility != _abilities.end() ? maybeAbility->second : kDefaultAbilityScore;
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
    _abilities[ability] = score;
}

void CreatureAttributes::setSkillRank(Skill skill, int rank) {
    _skills[skill] = rank;
}

ClassType CreatureAttributes::getClassByPosition(int position) const {
    return (position - 1) < static_cast<int>(_classLevels.size()) ?
        _classLevels[static_cast<size_t>(position) - 1].first :
        ClassType::Invalid;
}

int CreatureAttributes::getLevelByPosition(int position) const {
    return (position - 1) < static_cast<int>(_classLevels.size()) ?
        _classLevels[static_cast<size_t>(position) - 1].second :
        0;
}

int CreatureAttributes::getClassLevel(ClassType clazz) const {
    auto maybeClassLevel = find_if(
        _classLevels.begin(), _classLevels.end(),
        [&clazz](auto &classLevel) { return classLevel.first == clazz; });

    return maybeClassLevel != _classLevels.end() ? maybeClassLevel->second : 0;
}

bool CreatureAttributes::hasSkill(Skill skill) const {
    auto maybeSkill = _skills.find(skill);
    return maybeSkill != _skills.end() ? maybeSkill->second > 0 : false;
}

int CreatureAttributes::getSkillRank(Skill skill) const {
    auto maybeSkill = _skills.find(skill);
    return maybeSkill != _skills.end() ? maybeSkill->second : kDefaultSkillRank;
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

} // namespace game

} // namespace reone
