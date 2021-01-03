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

#include <boost/algorithm/string.hpp>

#include "../../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

static const char kSkillsTableResRef[] = "skills";

CreatureClass::CreatureClass(ClassType type) : _type(type) {
}

void CreatureClass::load(const TwoDaRow &row) {
    _name = Resources::instance().getString(row.getInt("name"));
    _description = Resources::instance().getString(row.getInt("description"));
    _hitdie = row.getInt("hitdie");
    _skillPointBase = row.getInt("skillpointbase");

    _defaultAttributes.setAbilityScore(Ability::Strength, row.getInt("str"));
    _defaultAttributes.setAbilityScore(Ability::Dexterity, row.getInt("dex"));
    _defaultAttributes.setAbilityScore(Ability::Constitution, row.getInt("con"));
    _defaultAttributes.setAbilityScore(Ability::Intelligence, row.getInt("int"));
    _defaultAttributes.setAbilityScore(Ability::Wisdom, row.getInt("wis"));
    _defaultAttributes.setAbilityScore(Ability::Charisma, row.getInt("cha"));

    string alias(boost::to_lower_copy(row.getString("skillstable")));
    shared_ptr<TwoDaTable> skills(Resources::instance().get2DA(kSkillsTableResRef));
    const vector<TwoDaRow> &rows = skills->rows();
    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        if (row.getInt(alias + "_class") == 1) {
            _classSkills.insert(static_cast<Skill>(i));
        }
    }
}

bool CreatureClass::isClassSkill(Skill skill) const {
    return _classSkills.count(skill) > 0;
}

int CreatureClass::getDefenseBonus(int level) const {
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

int CreatureClass::hitdie() const {
    return _hitdie;
}

const CreatureAttributes &CreatureClass::defaultAttributes() const {
    return _defaultAttributes;
}

int CreatureClass::skillPointBase() const {
    return _skillPointBase;
}

} // namespace game

} // namespace reone
