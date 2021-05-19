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
#include <set>
#include <vector>

#include "../types.h"

#include "savingthrows.h"

namespace reone {

namespace game {

/**
 * Aggregate for creature role-playing attributes: classes, abilities, skills,
 * feats and Force Powers.
 */
class CreatureAttributes {
public:
    int getDefense() const;

    // Class Levels

    void addClassLevels(ClassType clazz, int levels);

    /**
     * @return class type at the specified position (1-based)
     */
    ClassType getClassByPosition(int position) const;

    /**
     * @return class level at the specified position (1-based)
     */
    int getLevelByPosition(int position) const;

    /**
     * @return the sum of all class levels, aka "hit dice"
     */
    int getAggregateLevel() const;

    /**
     * @return the sum of (level * hitdie) of all classes
     */
    int getAggregateHitDie() const;

    ClassType getEffectiveClass() const;
    int getClassLevel(ClassType clazz) const;
    int getAggregateAttackBonus() const;
    SavingThrows getAggregateSavingThrows() const;

    // END Class Levels

    // Abilities

    int getAbilityScore(Ability ability) const;
    int getAbilityModifier(Ability ability) const;

    const std::map<Ability, int> &abilityScores() const { return _abilityScores; }
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

    const std::map<Skill, int> &skillRanks() const { return _skillRanks; }
    int computerUse() const;
    int demolitions() const;
    int stealth() const;
    int awareness() const;
    int persuade() const;
    int repair() const;
    int security() const;
    int treatInjury() const;

    void setSkillRank(Skill skill, int rank);

    // END Skills

    // Feats

    bool hasFeat(FeatType type) const { return _feats.count(type) > 0; }

    void addFeat(FeatType type) { _feats.insert(type); }
    void removeFeat(FeatType type) { _feats.erase(type); }

    // END Feats

    // Force Powers

    bool hasSpell(ForcePower type) const { return _spells.count(type) > 0; }

    void addSpell(ForcePower type) { _spells.insert(type); }
    void removeSpell(ForcePower type) { _spells.erase(type); }

    // END Force Powers

private:
    std::vector<std::pair<ClassType, int>> _classLevels;
    std::map<Ability, int> _abilityScores;
    std::map<Skill, int> _skillRanks;
    std::set<FeatType> _feats;
    std::set<ForcePower> _spells;
};

} // namespace game

} // namespace reone
