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

#pragma once

namespace reone {

namespace resource {

class Gff;

}

namespace game {

namespace gffschema {

struct UTC_ClassList_KnownList0 {
    uint16_t Spell {0};
    uint8_t SpellFlags {0};
    uint8_t SpellMetaMagic {0};
};

struct UTC_SpecAbilityList {
    uint16_t Spell {0};
    uint8_t SpellCasterLevel {0};
    uint8_t SpellFlags {0};
};

struct UTC_SkillList {
    uint8_t Rank {0};
};

struct UTC_ItemList {
    uint8_t Dropable {0};
    std::string InventoryRes;
    uint16_t Repos_PosX {0};
    uint16_t Repos_Posy {0};
};

struct UTC_FeatList {
    uint16_t Feat {0};
};

struct UTC_Equip_ItemList {
    uint8_t Dropable {0};
    std::string EquippedRes;
};

struct UTC_ClassList {
    int Class {0};
    int16_t ClassLevel {0};
    std::vector<UTC_ClassList_KnownList0> KnownList0;
};

struct UTC {
    uint16_t Appearance_Type {0};
    float BlindSpot {0.0f};
    uint8_t BodyBag {0};
    uint8_t BodyVariation {0};
    uint8_t Cha {0};
    float ChallengeRating {0.0f};
    std::vector<UTC_ClassList> ClassList;
    std::string Comment;
    uint8_t Con {0};
    std::string Conversation;
    int16_t CurrentForce {0};
    int16_t CurrentHitPoints {0};
    std::string Deity;
    std::pair<int, std::string> Description;
    uint8_t Dex {0};
    uint8_t Disarmable {0};
    std::vector<UTC_Equip_ItemList> Equip_ItemList;
    uint16_t FactionID {0};
    std::vector<UTC_FeatList> FeatList;
    std::pair<int, std::string> FirstName;
    int16_t ForcePoints {0};
    uint8_t Gender {0};
    uint8_t GoodEvil {0};
    int16_t HitPoints {0};
    uint8_t Hologram {0};
    uint8_t IgnoreCrePath {0};
    uint8_t Int {0};
    uint8_t Interruptable {0};
    uint8_t IsPC {0};
    std::vector<UTC_ItemList> ItemList;
    std::pair<int, std::string> LastName;
    uint8_t LawfulChaotic {0};
    int16_t MaxHitPoints {0};
    uint8_t Min1HP {0};
    uint8_t MultiplierSet {0};
    uint8_t NaturalAC {0};
    uint8_t NoPermDeath {0};
    uint8_t NotReorienting {0};
    uint8_t PaletteID {0};
    uint8_t PartyInteract {0};
    uint8_t PerceptionRange {0};
    int Phenotype {0};
    uint8_t Plot {0};
    uint16_t PortraitId {0};
    uint8_t Race {0};
    std::string ScriptAttacked;
    std::string ScriptDamaged;
    std::string ScriptDeath;
    std::string ScriptDialogue;
    std::string ScriptDisturbed;
    std::string ScriptEndDialogu;
    std::string ScriptEndRound;
    std::string ScriptHeartbeat;
    std::string ScriptOnBlocked;
    std::string ScriptOnNotice;
    std::string ScriptRested;
    std::string ScriptSpawn;
    std::string ScriptSpellAt;
    std::string ScriptUserDefine;
    std::vector<UTC_SkillList> SkillList;
    uint16_t SoundSetFile {0};
    std::vector<UTC_SpecAbilityList> SpecAbilityList;
    uint8_t Str {0};
    std::string Subrace;
    uint8_t SubraceIndex {0};
    std::string Tag;
    std::vector<void *> TemplateList;
    std::string TemplateResRef;
    uint8_t TextureVar {0};
    int WalkRate {0};
    uint8_t WillNotRender {0};
    uint8_t Wis {0};
    int16_t fortbonus {0};
    int16_t refbonus {0};
    int16_t willbonus {0};
};

UTC parseUTC(const resource::Gff &gff);

} // namespace gffschema

} // namespace game

} // namespace reone
