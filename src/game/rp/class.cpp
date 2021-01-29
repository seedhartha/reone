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
    _defaultAttributes.addClassLevels(type, 1);
}

void CreatureClass::load(const TwoDaRow &row) {
    _name = Resources::instance().getString(row.getInt("name"));
    _description = Resources::instance().getString(row.getInt("description"));
    _hitdie = row.getInt("hitdie");
    _skillPointBase = row.getInt("skillpointbase");

    CreatureAbilities &abilities = _defaultAttributes.abilities();
    abilities.setScore(Ability::Strength, row.getInt("str"));
    abilities.setScore(Ability::Dexterity, row.getInt("dex"));
    abilities.setScore(Ability::Constitution, row.getInt("con"));
    abilities.setScore(Ability::Intelligence, row.getInt("int"));
    abilities.setScore(Ability::Wisdom, row.getInt("wis"));
    abilities.setScore(Ability::Charisma, row.getInt("cha"));

    string skillsTable(boost::to_lower_copy(row.getString("skillstable")));
    loadClassSkills(skillsTable);

    string savingThrowTable(boost::to_lower_copy(row.getString("savingthrowtable")));
    loadSavingThrows(savingThrowTable);
}

void CreatureClass::loadClassSkills(const string &skillsTable) {
    shared_ptr<TwoDaTable> skills(Resources::instance().get2DA(kSkillsTableResRef));
    const vector<TwoDaRow> &rows = skills->rows();
    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        if (rows[i].getInt(skillsTable + "_class") == 1) {
            _classSkills.insert(static_cast<Skill>(i));
        }
    }
}

void CreatureClass::loadSavingThrows(const string &savingThrowTable) {
    shared_ptr<TwoDaTable> table(Resources::instance().get2DA(savingThrowTable));
    for (auto &row : table->rows()) {
        int level = row.getInt("level");

        SavingThrows throws;
        throws.fortitude = row.getInt("fortsave");
        throws.reflex = row.getInt("refsave");
        throws.will = row.getInt("willsave");

        _savingThrowsByLevel.insert(make_pair(level, move(throws)));
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

const SavingThrows &CreatureClass::getSavingThrows(int level) const {
    auto maybeThrows = _savingThrowsByLevel.find(level);
    if (maybeThrows == _savingThrowsByLevel.end()) {
        throw logic_error("Saving throws not found for level " + to_string(level));
    }
    return maybeThrows->second;
}

} // namespace game

} // namespace reone
