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
#include "../../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

static const char kSkillsTwoDaResRef[] = "skills";

CreatureClass::CreatureClass(ClassType type) : _type(type) {
    _defaultAttributes.addClassLevels(type, 1);
}

void CreatureClass::load(const TwoDA &twoDa, int row) {
    _name = Strings::instance().get(twoDa.getInt(row, "name"));
    _description = Strings::instance().get(twoDa.getInt(row, "description"));
    _hitdie = twoDa.getInt(row, "hitdie");
    _skillPointBase = twoDa.getInt(row, "skillpointbase");

    CreatureAbilities &abilities = _defaultAttributes.abilities();
    abilities.setScore(Ability::Strength, twoDa.getInt(row, "str"));
    abilities.setScore(Ability::Dexterity, twoDa.getInt(row, "dex"));
    abilities.setScore(Ability::Constitution, twoDa.getInt(row, "con"));
    abilities.setScore(Ability::Intelligence, twoDa.getInt(row, "int"));
    abilities.setScore(Ability::Wisdom, twoDa.getInt(row, "wis"));
    abilities.setScore(Ability::Charisma, twoDa.getInt(row, "cha"));

    string skillsTable(boost::to_lower_copy(twoDa.getString(row, "skillstable")));
    loadClassSkills(skillsTable);

    string savingThrowTable(boost::to_lower_copy(twoDa.getString(row, "savingthrowtable")));
    loadSavingThrows(savingThrowTable);

    string attackBonusTable(boost::to_lower_copy(twoDa.getString(row, "attackbonustable")));
    loadAttackBonuses(attackBonusTable);
}

void CreatureClass::loadClassSkills(const string &skillsTable) {
    shared_ptr<TwoDA> skills(Resources::instance().get2DA(kSkillsTwoDaResRef));
    for (int row = 0; row < skills->getRowCount(); ++row) {
        if (skills->getInt(row, skillsTable + "_class") == 1) {
            _classSkills.insert(static_cast<Skill>(row));
        }
    }
}

void CreatureClass::loadSavingThrows(const string &savingThrowTable) {
    shared_ptr<TwoDA> twoDa(Resources::instance().get2DA(savingThrowTable));
    for (int row = 0; row < twoDa->getRowCount(); ++row) {
        int level = twoDa->getInt(row, "level");

        SavingThrows throws;
        throws.fortitude = twoDa->getInt(row, "fortsave");
        throws.reflex = twoDa->getInt(row, "refsave");
        throws.will = twoDa->getInt(row, "willsave");

        _savingThrowsByLevel.insert(make_pair(level, move(throws)));
    }
}

void CreatureClass::loadAttackBonuses(const string &attackBonusTable) {
    shared_ptr<TwoDA> twoDa(Resources::instance().get2DA(attackBonusTable));
    for (int row = 0; row < twoDa->getRowCount(); ++row) {
        _attackBonuses.push_back(twoDa->getInt(row, "bab"));
    }
}

bool CreatureClass::isClassSkill(Skill skill) const {
    return _classSkills.count(skill) > 0;
}

const SavingThrows &CreatureClass::getSavingThrows(int level) const {
    auto maybeThrows = _savingThrowsByLevel.find(level);
    if (maybeThrows == _savingThrowsByLevel.end()) {
        throw logic_error("Saving throws not found for level " + to_string(level));
    }
    return maybeThrows->second;
}

int CreatureClass::getAttackBonus(int level) const {
    if (level < 1 || level > static_cast<int>(_attackBonuses.size())) {
        throw invalid_argument("level is invalid");
    }
    return _attackBonuses[level - 1];
}

} // namespace game

} // namespace reone
