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

/** @file
 *  Creature functions related to blueprint loading.
 */

#include "creature.h"

#include <boost/algorithm/string.hpp>

#include "../../render/textures.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../portraits.h"
#include "../soundsets.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

void Creature::loadUTC(const GffStruct &utc) {
    _appearance = utc.getInt("Appearance_Type");
    _blueprintResRef = boost::to_lower_copy(utc.getString("TemplateResRef"));
    _conversation = boost::to_lower_copy(utc.getString("Conversation"));
    _currentHitPoints = utc.getInt("CurrentHitPoints");
    _faction = static_cast<Faction>(utc.getInt("FactionID"));
    _hitPoints = utc.getInt("HitPoints");
    _maxHitPoints = utc.getInt("MaxHitPoints");
    _minOneHP = utc.getBool("Min1HP");
    _portraitId = utc.getInt("PortraitId");
    _racialType = static_cast<RacialType>(utc.getInt("Race"));
    _subrace = static_cast<Subrace>(utc.getInt("SubraceIndex"));
    _tag = boost::to_lower_copy(utc.getString("Tag"));

    loadNameFromUTC(utc);
    loadAttributesFromUTC(utc);
    loadPerceptionRangeFromUTC(utc);
    loadSoundSetFromUTC(utc);
    loadScriptsFromUTC(utc);

    for (auto &item : utc.getList("Equip_ItemList")) {
        equip(boost::to_lower_copy(item->getString("EquippedRes")));
    }
    for (auto &itemGffs : utc.getList("ItemList")) {
        string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
        bool dropable = itemGffs->getBool("Dropable");
        addItem(resRef, 1, dropable);
    }
}

void Creature::loadNameFromUTC(const GffStruct &utc) {
    string firstName, lastName;
    int firstNameStrRef = utc.getInt("FirstName", -1);
    if (firstNameStrRef != -1) {
        firstName = Strings::instance().get(firstNameStrRef);
    }
    int lastNameStrRef = utc.getInt("LastName", -1);
    if (lastNameStrRef != -1) {
        lastName = Strings::instance().get(lastNameStrRef);
    }
    if (!firstName.empty() && !lastName.empty()) {
        _name = firstName + " " + lastName;
    } else if (!firstName.empty()) {
        _name = firstName;
    }
}

void Creature::loadAttributesFromUTC(const GffStruct &utc) {
    CreatureAttributes &attributes = _attributes;
    for (auto &classGff : utc.getList("ClassList")) {
        int clazz = classGff->getInt("Class");
        int level = classGff->getInt("ClassLevel");
        attributes.addClassLevels(static_cast<ClassType>(clazz), level);
    }

    CreatureAbilities &abilities = attributes.abilities();
    abilities.setScore(Ability::Strength, utc.getInt("Str"));
    abilities.setScore(Ability::Dexterity, utc.getInt("Dex"));
    abilities.setScore(Ability::Constitution, utc.getInt("Con"));
    abilities.setScore(Ability::Intelligence, utc.getInt("Int"));
    abilities.setScore(Ability::Wisdom, utc.getInt("Wis"));
    abilities.setScore(Ability::Charisma, utc.getInt("Cha"));

    vector<shared_ptr<GffStruct>> skillsUtc(utc.getList("SkillList"));
    for (int i = 0; i < static_cast<int>(skillsUtc.size()); ++i) {
        Skill skill = static_cast<Skill>(i);
        attributes.skills().setRank(skill, skillsUtc[i]->getInt("Rank"));
    }
}

void Creature::loadPerceptionRangeFromUTC(const GffStruct &utc) {
    int rangeIdx = utc.getInt("PerceptionRange");
    shared_ptr<TwoDA> ranges(Resources::instance().get2DA("ranges"));
    _perception.sightRange = ranges->getFloat(rangeIdx, "primaryrange");
    _perception.hearingRange = ranges->getFloat(rangeIdx, "secondaryrange");
}

void Creature::loadSoundSetFromUTC(const GffStruct &utc) {
    uint32_t soundSetIdx = utc.getUint("SoundSetFile");
    if (soundSetIdx != 0xffff) {
        shared_ptr<TwoDA> soundSetTable(Resources::instance().get2DA("soundset"));
        string soundSetResRef(soundSetTable->getString(soundSetIdx, "resref"));
        if (!soundSetResRef.empty()) {
            _soundSet = SoundSets::instance().get(soundSetResRef);
        }
    }
}

void Creature::loadScriptsFromUTC(const GffStruct &utc) {
    _heartbeat = boost::to_lower_copy(utc.getString("ScriptHeartbeat"));
    _onDeath = boost::to_lower_copy(utc.getString("ScriptDeath"));
    _onEndRound = boost::to_lower_copy(utc.getString("ScriptEndRound"));
    _onNotice = boost::to_lower_copy(utc.getString("ScriptOnNotice"));
    _onSpawn = boost::to_lower_copy(utc.getString("ScriptSpawn"));
    _onUserDefined = boost::to_lower_copy(utc.getString("ScriptUserDefine"));
}

} // namespace game

} // namespace reone
