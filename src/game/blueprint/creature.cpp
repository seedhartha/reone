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

#include "creature.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void CreatureBlueprint::load(const GffStruct &utc) {
    _tag = utc.getString("Tag");
    boost::to_lower(_tag);

    for (auto &item : utc.getList("Equip_ItemList")) {
        string itemResRef(item.getString("EquippedRes"));
        boost::to_lower(itemResRef);

        _equipment.push_back(move(itemResRef));
    }

    _appearance = utc.getInt("Appearance_Type");
    _portraitId = utc.getInt("PortraitId", -1);
    _conversation = utc.getString("Conversation");

    int firstNameStrRef = utc.getInt("FirstName", -1);
    if (firstNameStrRef != -1) {
        _firstName = Resources::instance().getString(firstNameStrRef);
    }

    int lastNameStrRef = utc.getInt("LastName", -1);
    if (lastNameStrRef != -1) {
        _lastName = Resources::instance().getString(lastNameStrRef);
    }

    loadAttributes(utc);
    loadScripts(utc);
}

void CreatureBlueprint::loadAttributes(const GffStruct &utc) {
    for (auto &classGff : utc.getList("ClassList")) {
        int clazz = classGff.getInt("Class");
        int level = classGff.getInt("ClassLevel");
        _attributes.classLevels.push_back(make_pair(static_cast<ClassType>(clazz), level));
    }
    loadAbilities(utc);
    loadSkills(utc);
    _attributes.computeHitDice();
}

void CreatureBlueprint::loadAbilities(const GffStruct &utc) {
    _attributes.abilities[Ability::Strength] = utc.getInt("Str");
    _attributes.abilities[Ability::Dexterity] = utc.getInt("Dex");
    _attributes.abilities[Ability::Constitution] = utc.getInt("Con");
    _attributes.abilities[Ability::Intelligence] = utc.getInt("Int");
    _attributes.abilities[Ability::Wisdom] = utc.getInt("Wis");
    _attributes.abilities[Ability::Charisma] = utc.getInt("Cha");
}

void CreatureBlueprint::loadSkills(const GffStruct &utc) {
    const vector<GffStruct> &skills = utc.getList("SkillList");
    for (int i = 0; i < static_cast<int>(skills.size()); ++i) {
        Skill skill = static_cast<Skill>(i);
        _attributes.skills[skill] = skills[i].getInt("Rank");
    }
}

void CreatureBlueprint::loadScripts(const GffStruct &utc) {
    _onSpawn = utc.getString("ScriptSpawn");
    _onUserDefined = utc.getString("ScriptUserDefine");
}

const string &CreatureBlueprint::tag() const {
    return _tag;
}

const string &CreatureBlueprint::firstName() const {
    return _firstName;
}

const string &CreatureBlueprint::lastName() const {
    return _lastName;
}

const vector<string> &CreatureBlueprint::equipment() const {
    return _equipment;
}

int CreatureBlueprint::appearance() const {
    return _appearance;
}

int CreatureBlueprint::portraitId() const {
    return _portraitId;
}

const string &CreatureBlueprint::conversation() const {
    return _conversation;
}

const CreatureAttributes &CreatureBlueprint::attributes() const {
    return _attributes;
}

const string &CreatureBlueprint::onSpawn() const {
    return _onSpawn;
}

const string &CreatureBlueprint::onUserDefined() const {
    return _onUserDefined;
}

} // namespace game

} // namespace reone
