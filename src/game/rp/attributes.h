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
    /**
     * @return the sum of (level * hitdie) of all classes
     */
    int getAggregateHitDie() const;

    // Classes

    void addClassLevels(ClassType clazz, int levels);

    ClassType getClassByPosition(int position) const;
    ClassType getLatestClass() const;
    int getLevelByPosition(int position) const;
    int getClassLevel(ClassType clazz) const;

    /**
     * @return the sum of all class levels, aka "hit dice"
     */
    int getAggregateLevel() const;

    // END Classes

    // Abilities

    int getAbilityScore(Ability ability) const;
    int getAbilityModifier(Ability ability) const;

    int strength() const;
    int dexterity() const;
    int constitution() const;
    int intelligence() const;
    int wisdom() const;
    int charisma() const;

    void setAbilityScore(Ability ability, int score);

    // END Abilities

    // Skills

    bool hasSkill(Skill skill) const;

    int getSkillRank(Skill skill) const;

    int computerUse() const;
    int demolitions() const;
    int stealth() const;
    int awareness() const;
    int persuade() const;
    int repair() const;
    int security() const;
    int treatInjury() const;

    void setSkillRank(Skill ability, int rank);

    // END Skills

private:
    std::vector<std::pair<ClassType, int>> _classLevels;
    std::map<Ability, int> _abilities;
    std::map<Skill, int> _skills;
};

} // namespace game

} // namespace reone
