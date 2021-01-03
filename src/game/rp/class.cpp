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

#include "class.h"

#include "../../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

CreatureClass::CreatureClass(ClassType type) : _type(type) {
}

void CreatureClass::load(const TwoDaRow &row) {
    _name = Resources::instance().getString(row.getInt("name"));
    _description = Resources::instance().getString(row.getInt("description"));
    _hitdie = row.getInt("hitdie");

    _defaultAttributes.setAbilityScore(Ability::Strength, row.getInt("str"));
    _defaultAttributes.setAbilityScore(Ability::Dexterity, row.getInt("dex"));
    _defaultAttributes.setAbilityScore(Ability::Constitution, row.getInt("con"));
    _defaultAttributes.setAbilityScore(Ability::Intelligence, row.getInt("int"));
    _defaultAttributes.setAbilityScore(Ability::Wisdom, row.getInt("wis"));
    _defaultAttributes.setAbilityScore(Ability::Charisma, row.getInt("cha"));
}

int CreatureClass::getHitPoints(int level) {
    return level * _hitdie;
}

int CreatureClass::getDefenseBonus(int level) {
    switch (_type) {
        case ClassType::JediConsular:
        case ClassType::JediGuardian:
        case ClassType::JediSentinel:
        case ClassType::Scoundrel:
            return 2 + (2 * (level / 6));
        default:
            return 0;
    }
}

const string &CreatureClass::name() const {
    return _name;
}

const string &CreatureClass::description() const {
    return _description;
}

const CreatureAttributes &CreatureClass::defaultAttributes() const {
    return _defaultAttributes;
}

} // namespace game

} // namespace reone
