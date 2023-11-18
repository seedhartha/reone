/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/gffschema/utc.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace gffschema {

static UTC_ClassList_KnownList0 parseUTC_ClassList_KnownList0(const Gff &gff) {
    UTC_ClassList_KnownList0 strct;
    strct.Spell = gff.getUint("Spell");
    strct.SpellFlags = gff.getUint("SpellFlags");
    strct.SpellMetaMagic = gff.getUint("SpellMetaMagic");
    return strct;
}

static UTC_SpecAbilityList parseUTC_SpecAbilityList(const Gff &gff) {
    UTC_SpecAbilityList strct;
    strct.Spell = gff.getUint("Spell");
    strct.SpellCasterLevel = gff.getUint("SpellCasterLevel");
    strct.SpellFlags = gff.getUint("SpellFlags");
    return strct;
}

static UTC_SkillList parseUTC_SkillList(const Gff &gff) {
    UTC_SkillList strct;
    strct.Rank = gff.getUint("Rank");
    return strct;
}

static UTC_ItemList parseUTC_ItemList(const Gff &gff) {
    UTC_ItemList strct;
    strct.Dropable = gff.getUint("Dropable");
    strct.InventoryRes = gff.getString("InventoryRes");
    strct.Repos_PosX = gff.getUint("Repos_PosX");
    strct.Repos_Posy = gff.getUint("Repos_Posy");
    return strct;
}

static UTC_FeatList parseUTC_FeatList(const Gff &gff) {
    UTC_FeatList strct;
    strct.Feat = gff.getUint("Feat");
    return strct;
}

static UTC_Equip_ItemList parseUTC_Equip_ItemList(const Gff &gff) {
    UTC_Equip_ItemList strct;
    strct.Dropable = gff.getUint("Dropable");
    strct.EquippedRes = gff.getString("EquippedRes");
    return strct;
}

static UTC_ClassList parseUTC_ClassList(const Gff &gff) {
    UTC_ClassList strct;
    strct.Class = gff.getInt("Class");
    strct.ClassLevel = gff.getInt("ClassLevel");
    for (auto &item : gff.getList("KnownList0")) {
        strct.KnownList0.push_back(parseUTC_ClassList_KnownList0(*item));
    }
    return strct;
}

UTC parseUTC(const Gff &gff) {
    UTC strct;
    strct.Appearance_Type = gff.getUint("Appearance_Type");
    strct.BlindSpot = gff.getFloat("BlindSpot");
    strct.BodyBag = gff.getUint("BodyBag");
    strct.BodyVariation = gff.getUint("BodyVariation");
    strct.Cha = gff.getUint("Cha");
    strct.ChallengeRating = gff.getFloat("ChallengeRating");
    for (auto &item : gff.getList("ClassList")) {
        strct.ClassList.push_back(parseUTC_ClassList(*item));
    }
    strct.Comment = gff.getString("Comment");
    strct.Con = gff.getUint("Con");
    strct.Conversation = gff.getString("Conversation");
    strct.CurrentForce = gff.getInt("CurrentForce");
    strct.CurrentHitPoints = gff.getInt("CurrentHitPoints");
    strct.Deity = gff.getString("Deity");
    strct.Description = std::make_pair(gff.getInt("Description"), gff.getString("Description"));
    strct.Dex = gff.getUint("Dex");
    strct.Disarmable = gff.getUint("Disarmable");
    for (auto &item : gff.getList("Equip_ItemList")) {
        strct.Equip_ItemList.push_back(parseUTC_Equip_ItemList(*item));
    }
    strct.FactionID = gff.getUint("FactionID");
    for (auto &item : gff.getList("FeatList")) {
        strct.FeatList.push_back(parseUTC_FeatList(*item));
    }
    strct.FirstName = std::make_pair(gff.getInt("FirstName"), gff.getString("FirstName"));
    strct.ForcePoints = gff.getInt("ForcePoints");
    strct.Gender = gff.getUint("Gender");
    strct.GoodEvil = gff.getUint("GoodEvil");
    strct.HitPoints = gff.getInt("HitPoints");
    strct.Hologram = gff.getUint("Hologram");
    strct.IgnoreCrePath = gff.getUint("IgnoreCrePath");
    strct.Int = gff.getUint("Int");
    strct.Interruptable = gff.getUint("Interruptable");
    strct.IsPC = gff.getUint("IsPC");
    for (auto &item : gff.getList("ItemList")) {
        strct.ItemList.push_back(parseUTC_ItemList(*item));
    }
    strct.LastName = std::make_pair(gff.getInt("LastName"), gff.getString("LastName"));
    strct.LawfulChaotic = gff.getUint("LawfulChaotic");
    strct.MaxHitPoints = gff.getInt("MaxHitPoints");
    strct.Min1HP = gff.getUint("Min1HP");
    strct.MultiplierSet = gff.getUint("MultiplierSet");
    strct.NaturalAC = gff.getUint("NaturalAC");
    strct.NoPermDeath = gff.getUint("NoPermDeath");
    strct.NotReorienting = gff.getUint("NotReorienting");
    strct.PaletteID = gff.getUint("PaletteID");
    strct.PartyInteract = gff.getUint("PartyInteract");
    strct.PerceptionRange = gff.getUint("PerceptionRange");
    strct.Phenotype = gff.getInt("Phenotype");
    strct.Plot = gff.getUint("Plot");
    strct.PortraitId = gff.getUint("PortraitId");
    strct.Race = gff.getUint("Race");
    strct.ScriptAttacked = gff.getString("ScriptAttacked");
    strct.ScriptDamaged = gff.getString("ScriptDamaged");
    strct.ScriptDeath = gff.getString("ScriptDeath");
    strct.ScriptDialogue = gff.getString("ScriptDialogue");
    strct.ScriptDisturbed = gff.getString("ScriptDisturbed");
    strct.ScriptEndDialogu = gff.getString("ScriptEndDialogu");
    strct.ScriptEndRound = gff.getString("ScriptEndRound");
    strct.ScriptHeartbeat = gff.getString("ScriptHeartbeat");
    strct.ScriptOnBlocked = gff.getString("ScriptOnBlocked");
    strct.ScriptOnNotice = gff.getString("ScriptOnNotice");
    strct.ScriptRested = gff.getString("ScriptRested");
    strct.ScriptSpawn = gff.getString("ScriptSpawn");
    strct.ScriptSpellAt = gff.getString("ScriptSpellAt");
    strct.ScriptUserDefine = gff.getString("ScriptUserDefine");
    for (auto &item : gff.getList("SkillList")) {
        strct.SkillList.push_back(parseUTC_SkillList(*item));
    }
    strct.SoundSetFile = gff.getUint("SoundSetFile");
    for (auto &item : gff.getList("SpecAbilityList")) {
        strct.SpecAbilityList.push_back(parseUTC_SpecAbilityList(*item));
    }
    strct.Str = gff.getUint("Str");
    strct.Subrace = gff.getString("Subrace");
    strct.SubraceIndex = gff.getUint("SubraceIndex");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.TextureVar = gff.getUint("TextureVar");
    strct.WalkRate = gff.getInt("WalkRate");
    strct.WillNotRender = gff.getUint("WillNotRender");
    strct.Wis = gff.getUint("Wis");
    strct.fortbonus = gff.getInt("fortbonus");
    strct.refbonus = gff.getInt("refbonus");
    strct.willbonus = gff.getInt("willbonus");
    return strct;
}

} // namespace gffschema

} // namespace game

} // namespace reone
