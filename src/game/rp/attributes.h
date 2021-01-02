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

#pragma once

#include <map>
#include <utility>
#include <vector>

#include "types.h"

namespace reone {

namespace game {

class CreatureAttributes {
public:
    void addClassLevels(ClassType clazz, int levels);

    ClassType getClassByPosition(int position) const;
    int getLevelByPosition(int position) const;
    int getClassLevel(ClassType clazz) const;
    int getHitDice() const { return _hitDice; }

    // Abilities

    int getAbilityScore(Ability ability) const { return _abilities.find(ability)->second; }

    int strength() const { return _abilities.find(Ability::Strength)->second; }
    int dexterity() const { return _abilities.find(Ability::Dexterity)->second; }
    int constitution() const { return _abilities.find(Ability::Constitution)->second; }
    int intelligence() const { return _abilities.find(Ability::Intelligence)->second; }
    int wisdom() const { return _abilities.find(Ability::Wisdom)->second; }
    int charisma() const { return _abilities.find(Ability::Charisma)->second; }

    void setAbilityScore(Ability ability, int score);

    // END Abilities

    // Skills

    bool hasSkill(Skill skill) const;

    int getSkillRank(Skill skill) const;

    int computerUse() const { return _skills.find(Skill::ComputerUse)->second; }
    int demolitions() const { return _skills.find(Skill::Demolitions)->second; }
    int stealth() const { return _skills.find(Skill::Stealth)->second; }
    int awareness() const { return _skills.find(Skill::Awareness)->second; }
    int persuade() const { return _skills.find(Skill::Persuade)->second; }
    int repair() const { return _skills.find(Skill::Repair)->second; }
    int security() const { return _skills.find(Skill::Security)->second; }
    int treatInjury() const { return _skills.find(Skill::TreatInjury)->second; }

    void setSkillRank(Skill ability, int rank);

    // END Skills

private:
    std::vector<std::pair<ClassType, int>> _classLevels;
    int _hitDice { 0 };
    std::map<Ability, int> _abilities;
    std::map<Skill, int> _skills;

    void computeHitDice();
};

} // namespace game

} // namespace reone
