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

#include "abilities.h"
#include "skills.h"
#include "types.h"

namespace reone {

namespace game {

/**
 * Aggregate for creature role-playing attributes: classes, abilities, skills,
 * feats and Force Powers.
 */
class CreatureAttributes {
public:
    void addClassLevels(ClassType clazz, int levels);

    ClassType getClassByPosition(int position) const;
    ClassType getEffectiveClass() const;
    int getLevelByPosition(int position) const;
    int getClassLevel(ClassType clazz) const;

    /**
     * @return the sum of all class levels, aka "hit dice"
     */
    int getAggregateLevel() const;

    /**
    * @return the sum of (level * hitdie) of all classes
    */
    int getAggregateHitDie() const;

    CreatureAbilities &abilities();
    CreatureSkills &skills();

    void setAbilities(CreatureAbilities abilities);
    void setSkills(CreatureSkills skills);

private:
    std::vector<std::pair<ClassType, int>> _classLevels;
    CreatureAbilities _abilities;
    CreatureSkills _skills;
};

} // namespace game

} // namespace reone
