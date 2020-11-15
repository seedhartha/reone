/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

int CreatureAttributes::getClassLevel(ClassType clazz) const {
    auto maybeClassLevel = find_if(
        _classLevels.begin(), _classLevels.end(),
        [&clazz](auto &classLevel) { return classLevel.first == clazz; });

    return maybeClassLevel != _classLevels.end() ? maybeClassLevel->second : 0;
}

} // namespace game

} // namespace reone
