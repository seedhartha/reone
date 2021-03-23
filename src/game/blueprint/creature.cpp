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

#include "creature.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../../common/log.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../object/creature.h"
#include "../portraitutil.h"
#include "../soundsets.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

CreatureBlueprint::CreatureBlueprint(const string &resRef, const shared_ptr<GffStruct> &utc) :
    _resRef(resRef),
    _utc(utc) {

    if (!utc) {
        throw invalid_argument("utc must not be null");
    }
}

void CreatureBlueprint::load(Creature &creature) {
    creature._blueprintResRef = _resRef;
    creature._tag = boost::to_lower_copy(_utc->getString("Tag"));
    creature._appearance = getAppearanceFromUtc();

    for (auto &item : _utc->getList("Equip_ItemList")) {
        creature.equip(boost::to_lower_copy(item->getString("EquippedRes")));
    }

    string portrait(getPortrait(_utc->getInt("PortraitId", -1)));
    creature._portrait = Textures::instance().get(portrait, TextureUsage::GUI);

    creature._faction = static_cast<Faction>(_utc->getInt("FactionID"));
    creature._conversation = boost::to_lower_copy(_utc->getString("Conversation"));
    creature._minOneHP = _utc->getBool("Min1HP");
    creature._hitPoints = _utc->getInt("HitPoints");
    creature._currentHitPoints = _utc->getInt("CurrentHitPoints");
    creature._maxHitPoints = _utc->getInt("MaxHitPoints");
    creature._racialType = static_cast<RacialType>(_utc->getInt("Race"));

    loadName(creature);
    loadAttributes(creature);
    loadScripts(creature);
    loadItems(creature);
    loadSoundSet(creature);
    loadBodyBag(creature);
    loadPerception(creature);
}

int CreatureBlueprint::getAppearanceFromUtc() const {
    return _utc->getInt("Appearance_Type");
}

void CreatureBlueprint::loadName(Creature &creature) {
    string firstName, lastName;

    int firstNameStrRef = _utc->getInt("FirstName", -1);
    if (firstNameStrRef != -1) {
        firstName = Strings::instance().get(firstNameStrRef);
    }
    int lastNameStrRef = _utc->getInt("LastName", -1);
    if (lastNameStrRef != -1) {
        lastName = Strings::instance().get(lastNameStrRef);
    }

    if (!firstName.empty() && !lastName.empty()) {
        creature._name = firstName + " " + lastName;
    } else if (!firstName.empty()) {
        creature._name = firstName;
    }
}

void CreatureBlueprint::loadAttributes(Creature &creature) {
    CreatureAttributes &attributes = creature.attributes();
    for (auto &classGff : _utc->getList("ClassList")) {
        int clazz = classGff->getInt("Class");
        int level = classGff->getInt("ClassLevel");
        attributes.addClassLevels(static_cast<ClassType>(clazz), level);
    }
    loadAbilities(attributes.abilities());
    loadSkills(attributes.skills());
}

void CreatureBlueprint::loadAbilities(CreatureAbilities &abilities) {
    abilities.setScore(Ability::Strength, _utc->getInt("Str"));
    abilities.setScore(Ability::Dexterity, _utc->getInt("Dex"));
    abilities.setScore(Ability::Constitution, _utc->getInt("Con"));
    abilities.setScore(Ability::Intelligence, _utc->getInt("Int"));
    abilities.setScore(Ability::Wisdom, _utc->getInt("Wis"));
    abilities.setScore(Ability::Charisma, _utc->getInt("Cha"));
}

void CreatureBlueprint::loadSkills(CreatureSkills &skills) {
    vector<shared_ptr<GffStruct>> skillsUtc(_utc->getList("SkillList"));
    for (int i = 0; i < static_cast<int>(skillsUtc.size()); ++i) {
        Skill skill = static_cast<Skill>(i);
        skills.setRank(skill, skillsUtc[i]->getInt("Rank"));
    }
}

void CreatureBlueprint::loadScripts(Creature &creature) {
    creature._heartbeat = boost::to_lower_copy(_utc->getString("ScriptHeartbeat"));
    creature._onSpawn = boost::to_lower_copy(_utc->getString("ScriptSpawn"));
    creature._onDeath = boost::to_lower_copy(_utc->getString("ScriptDeath"));
    creature._onUserDefined = boost::to_lower_copy(_utc->getString("ScriptUserDefine"));
    creature._onNotice = boost::to_lower_copy(_utc->getString("ScriptOnNotice"));
}

void CreatureBlueprint::loadItems(Creature &creature) {
    for (auto &itemGffs : _utc->getList("ItemList")) {
        string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
        bool dropable = itemGffs->getBool("Dropable");
        creature.addItem(resRef, 1, dropable);
    }
}

void CreatureBlueprint::loadSoundSet(Creature &creature) {
    int soundSetIdx = _utc->getInt("SoundSetFile");
    if (soundSetIdx == 0xffff) return;

    shared_ptr<TwoDA> soundSetTable(Resources::instance().get2DA("soundset"));
    string soundSetResRef(soundSetTable->getString(soundSetIdx, "resref"));
    if (soundSetResRef.empty()) return;

    creature._soundSet = SoundSets::instance().get(soundSetResRef);
}

void CreatureBlueprint::loadBodyBag(Creature &creature) {
}

void CreatureBlueprint::loadPerception(Creature &creature) {
    int rangeIdx = _utc->getInt("PerceptionRange", 0);
    shared_ptr<TwoDA> ranges(Resources::instance().get2DA("ranges"));
    creature._perception.sightRange = ranges->getFloat(rangeIdx, "primaryrange");
    creature._perception.hearingRange = ranges->getFloat(rangeIdx, "secondaryrange");
}

void StaticCreatureBlueprint::load(Creature &creature) {
    creature._appearance = _appearance;
    creature._attributes = _attributes;
    creature._currentHitPoints = creature._hitPoints = creature._maxHitPoints = _attributes.getAggregateHitDie();

    for (auto &item : _equipment) {
        creature.equip(item);
    }
}

void StaticCreatureBlueprint::clearEquipment() {
    _equipment.clear();
}

void StaticCreatureBlueprint::addEquippedItem(const string &resRef) {
    _equipment.push_back(resRef);
}

void StaticCreatureBlueprint::setGender(Gender gender) {
    _gender = gender;
}

void StaticCreatureBlueprint::setAppearance(int appearance) {
    _appearance = appearance;
}

void StaticCreatureBlueprint::setAttributes(CreatureAttributes attributes) {
    _attributes = move(attributes);
}

} // namespace game

} // namespace reone
