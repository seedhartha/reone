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

#include "classes.h"

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
}

ClassType CreatureAttributes::getClassByPosition(int position) const {
    return position > 0 && (position - 1) < static_cast<int>(_classLevels.size()) ?
        _classLevels[static_cast<size_t>(position) - 1].first :
        ClassType::Invalid;
}

ClassType CreatureAttributes::getEffectiveClass() const {
    return _classLevels.empty() ? ClassType::Invalid : _classLevels.back().first;
}

int CreatureAttributes::getLevelByPosition(int position) const {
    return position > 0 && (position - 1) < static_cast<int>(_classLevels.size()) ?
        _classLevels[static_cast<size_t>(position) - 1].second :
        0;
}

int CreatureAttributes::getClassLevel(ClassType clazz) const {
    auto maybeClassLevel = find_if(
        _classLevels.begin(), _classLevels.end(),
        [&clazz](auto &classLevel) { return classLevel.first == clazz; });

    return maybeClassLevel != _classLevels.end() ? maybeClassLevel->second : 0;
}

int CreatureAttributes::getAggregateHitDie() const {
    int result = 0;
    for (auto &pair : _classLevels) {
        result += pair.second * Classes::instance().get(pair.first)->hitdie();
    }
    return result;
}

int CreatureAttributes::getAggregateLevel() const {
    int result = 0;
    for (auto &pair : _classLevels) {
        result += pair.second;
    }
    return result;
}

SavingThrows CreatureAttributes::getAggregateSavingThrows() const {
    SavingThrows result;
    result.fortitude = 1;
    result.reflex = 1;
    result.will = 1;
    for (auto &pair : _classLevels) {
        auto classThrows = Classes::instance().get(pair.first)->getSavingThrows(pair.second);
        result.fortitude += classThrows.fortitude;
        result.reflex += classThrows.reflex;
        result.will += classThrows.will;
    }
    return move(result);
}

int CreatureAttributes::getDefense() const {
    return 10 + _abilities.getModifier(Ability::Dexterity);
}

void CreatureAttributes::setAbilities(CreatureAbilities abilities) {
    _abilities = move(abilities);
}

void CreatureAttributes::setSkills(CreatureSkills skills) {
    _skills = move(skills);
}

} // namespace game

} // namespace reone
