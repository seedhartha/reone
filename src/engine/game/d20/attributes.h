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

#include <vector>

#include "../types.h"

#include "abilities.h"
#include "savingthrows.h"
#include "skills.h"

namespace reone {

namespace game {

/**
 * Aggregate for creature role-playing attributes: classes, abilities, skills,
 * feats and Force Powers.
 */
class CreatureAttributes {
public:
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

    SavingThrows getAggregateSavingThrows() const;
    int getAggregateAttackBonus() const;
    int getDefense() const;

    ClassType getEffectiveClass() const;
    int getClassLevel(ClassType clazz) const;

    CreatureAbilities &abilities() { return _abilities; }
    const CreatureAbilities &abilities() const { return _abilities; }
    CreatureSkills &skills() { return _skills; }
    const CreatureSkills &skills() const { return _skills; }

    void setAbilities(CreatureAbilities abilities);
    void setSkills(CreatureSkills skills);

private:
    std::vector<std::pair<ClassType, int>> _classLevels;
    CreatureAbilities _abilities;
    CreatureSkills _skills;
};

} // namespace game

} // namespace reone
