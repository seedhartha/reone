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

#include "../../graphics/texture/textures.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../game.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Creature::loadUTC(const GffStruct &utc) {
    _blueprintResRef = boost::to_lower_copy(utc.getString("TemplateResRef"));
    _race = utc.getEnum("Race", RacialType::Invalid); // index into racialtypes.2da
    _subrace = utc.getEnum("SubraceIndex", Subrace::None); // index into subrace.2da
    _appearance = utc.getInt("Appearance_Type"); // index into appearance.2da
    _gender = utc.getEnum("Gender", Gender::None); // index into gender.2da
    _portraitId = utc.getInt("PortraitId"); // index into portrait.2da
    _tag = boost::to_lower_copy(utc.getString("Tag"));
    _conversation = boost::to_lower_copy(utc.getString("Conversation"));
    _isPC = utc.getBool("IsPC"); // always 0
    _faction = utc.getEnum("FactionID", Faction::Invalid); // index into repute.2da
    _disarmable = utc.getBool("Disarmable");
    _plot = utc.getBool("Plot");
    _interruptable = utc.getBool("Interruptable");
    _noPermDeath = utc.getBool("NoPermDeath");
    _notReorienting = utc.getBool("NotReorienting");
    _bodyVariation = utc.getInt("BodyVariation");
    _textureVar = utc.getInt("TextureVar");
    _minOneHP = utc.getBool("Min1HP");
    _partyInteract = utc.getBool("PartyInteract");
    _walkRate = utc.getInt("WalkRate"); // index into creaturespeed.2da
    _naturalAC = utc.getInt("NaturalAC");
    _hitPoints = utc.getInt("HitPoints");
    _currentHitPoints = utc.getInt("CurrentHitPoints");
    _maxHitPoints = utc.getInt("MaxHitPoints");
    _forcePoints = utc.getInt("ForcePoints");
    _currentForce = utc.getInt("CurrentForce");
    _refBonus = utc.getInt("refbonus");
    _willBonus = utc.getInt("willbonus");
    _fortBonus = utc.getInt("fortbonus");
    _goodEvil = utc.getInt("GoodEvil");
    _challengeRating = utc.getInt("ChallengeRating");

    _onHeartbeat = boost::to_lower_copy(utc.getString("ScriptHeartbeat"));
    _onNotice = boost::to_lower_copy(utc.getString("ScriptOnNotice"));
    _onSpellAt = boost::to_lower_copy(utc.getString("ScriptSpellAt"));
    _onAttacked = boost::to_lower_copy(utc.getString("ScriptAttacked"));
    _onDamaged = boost::to_lower_copy(utc.getString("ScriptDamaged"));
    _onDisturbed = boost::to_lower_copy(utc.getString("ScriptDisturbed"));
    _onEndRound = boost::to_lower_copy(utc.getString("ScriptEndRound"));
    _onEndDialogue = boost::to_lower_copy(utc.getString("ScriptEndDialogu"));
    _onDialogue = boost::to_lower_copy(utc.getString("ScriptDialogue"));
    _onSpawn = boost::to_lower_copy(utc.getString("ScriptSpawn"));
    _onDeath = boost::to_lower_copy(utc.getString("ScriptDeath"));
    _onUserDefined = boost::to_lower_copy(utc.getString("ScriptUserDefine"));
    _onBlocked = boost::to_lower_copy(utc.getString("ScriptOnBlocked"));

    loadNameFromUTC(utc);
    loadSoundSetFromUTC(utc);
    loadBodyBagFromUTC(utc);
    loadAttributesFromUTC(utc);
    loadPerceptionRangeFromUTC(utc);

    for (auto &item : utc.getList("Equip_ItemList")) {
        equip(boost::to_lower_copy(item->getString("EquippedRes")));
    }
    for (auto &itemGffs : utc.getList("ItemList")) {
        string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
        bool dropable = itemGffs->getBool("Dropable");
        addItem(resRef, 1, dropable);
    }

    // Unused fields:
    //
    // - Phenotype (not applicable, always 0)
    // - Description (not applicable)
    // - Subrace (unknown, we already use SubraceIndex)
    // - Deity (not applicable, always empty)
    // - LawfulChaotic (not applicable)
    // - ScriptRested (not applicable, mostly empty)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Creature::loadNameFromUTC(const GffStruct &utc) {
    string firstName(_game->services().resource().strings().get(utc.getInt("FirstName")));
    string lastName(_game->services().resource().strings().get(utc.getInt("LastName")));
    if (!firstName.empty() && !lastName.empty()) {
        _name = firstName + " " + lastName;
    } else if (!firstName.empty()) {
        _name = firstName;
    }
}

void Creature::loadSoundSetFromUTC(const GffStruct &utc) {
    uint32_t soundSetIdx = utc.getUint("SoundSetFile");
    if (soundSetIdx != 0xffff) {
        shared_ptr<TwoDA> soundSetTable(_game->services().resource().resources().get2DA("soundset"));
        string soundSetResRef(soundSetTable->getString(soundSetIdx, "resref"));
        if (!soundSetResRef.empty()) {
            _soundSet = _game->services().soundSets().get(soundSetResRef);
        }
    }
}

void Creature::loadBodyBagFromUTC(const GffStruct &utc) {
    int bodyBag = utc.getInt("BodyBag");
    shared_ptr<TwoDA> bodyBags(_game->services().resource().resources().get2DA("bodybag"));
    _bodyBag.name = _game->services().resource().strings().get(bodyBags->getInt(bodyBag, "name"));
    _bodyBag.appearance = bodyBags->getInt(bodyBag, "appearance");
    _bodyBag.corpse = bodyBags->getBool(bodyBag, "corpse");
}

void Creature::loadAttributesFromUTC(const GffStruct &utc) {
    CreatureAttributes &attributes = _attributes;
    attributes.setAbilityScore(Ability::Strength, utc.getInt("Str"));
    attributes.setAbilityScore(Ability::Dexterity, utc.getInt("Dex"));
    attributes.setAbilityScore(Ability::Constitution, utc.getInt("Con"));
    attributes.setAbilityScore(Ability::Intelligence, utc.getInt("Int"));
    attributes.setAbilityScore(Ability::Wisdom, utc.getInt("Wis"));
    attributes.setAbilityScore(Ability::Charisma, utc.getInt("Cha"));

    for (auto &classGffs : utc.getList("ClassList")) {
        int clazz = classGffs->getInt("Class");
        int level = classGffs->getInt("ClassLevel");
        attributes.addClassLevels(_game->services().classes().get(static_cast<ClassType>(clazz)).get(), level);
        for (auto &spellGffs : classGffs->getList("KnownList0")) {
            auto spell = static_cast<ForcePower>(spellGffs->getUint("Spell"));
            attributes.addSpell(spell);
        }
    }

    vector<shared_ptr<GffStruct>> skillsUtc(utc.getList("SkillList"));
    for (int i = 0; i < static_cast<int>(skillsUtc.size()); ++i) {
        SkillType skill = static_cast<SkillType>(i);
        attributes.setSkillRank(skill, skillsUtc[i]->getInt("Rank"));
    }

    for (auto &featGffs : utc.getList("FeatList")) {
        auto feat = static_cast<FeatType>(featGffs->getUint("Feat"));
        _attributes.addFeat(feat);
    }
}

void Creature::loadPerceptionRangeFromUTC(const GffStruct &utc) {
    int rangeIdx = utc.getInt("PerceptionRange");
    shared_ptr<TwoDA> ranges(_game->services().resource().resources().get2DA("ranges"));
    _perception.sightRange = ranges->getFloat(rangeIdx, "primaryrange");
    _perception.hearingRange = ranges->getFloat(rangeIdx, "secondaryrange");
}

} // namespace game

} // namespace reone
