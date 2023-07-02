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

#include "reone/game/object/template/utc.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace schema {

static UTC_007 parseUTC_007(const Gff &gff) {
    UTC_007 strct;
    strct.Spell = gff.getUint("Spell");
    strct.SpellFlags = gff.getUint("SpellFlags");
    strct.SpellMetaMagic = gff.getUint("SpellMetaMagic");
    return strct;
}

static UTC_006 parseUTC_006(const Gff &gff) {
    UTC_006 strct;
    strct.Spell = gff.getUint("Spell");
    strct.SpellCasterLevel = gff.getUint("SpellCasterLevel");
    strct.SpellFlags = gff.getUint("SpellFlags");
    return strct;
}

static UTC_001 parseUTC_001(const Gff &gff) {
    UTC_001 strct;
    strct.Rank = gff.getUint("Rank");
    return strct;
}

static UTC_005 parseUTC_005(const Gff &gff) {
    UTC_005 strct;
    strct.Dropable = gff.getUint("Dropable");
    strct.InventoryRes = gff.getString("InventoryRes");
    strct.Repos_PosX = gff.getUint("Repos_PosX");
    strct.Repos_Posy = gff.getUint("Repos_Posy");
    return strct;
}

static UTC_002 parseUTC_002(const Gff &gff) {
    UTC_002 strct;
    strct.Feat = gff.getUint("Feat");
    return strct;
}

static UTC_004 parseUTC_004(const Gff &gff) {
    UTC_004 strct;
    strct.Dropable = gff.getUint("Dropable");
    strct.EquippedRes = gff.getString("EquippedRes");
    return strct;
}

static UTC_003 parseUTC_003(const Gff &gff) {
    UTC_003 strct;
    strct.Class = gff.getInt("Class");
    strct.ClassLevel = gff.getInt("ClassLevel");
    for (auto &item : gff.getList("KnownList0")) {
        strct.KnownList0.push_back(parseUTC_007(*item));
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
        strct.ClassList.push_back(parseUTC_003(*item));
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
        strct.Equip_ItemList.push_back(parseUTC_004(*item));
    }
    strct.FactionID = gff.getUint("FactionID");
    for (auto &item : gff.getList("FeatList")) {
        strct.FeatList.push_back(parseUTC_002(*item));
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
        strct.ItemList.push_back(parseUTC_005(*item));
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
        strct.SkillList.push_back(parseUTC_001(*item));
    }
    strct.SoundSetFile = gff.getUint("SoundSetFile");
    for (auto &item : gff.getList("SpecAbilityList")) {
        strct.SpecAbilityList.push_back(parseUTC_006(*item));
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

} // namespace schema

} // namespace game

} // namespace reone
