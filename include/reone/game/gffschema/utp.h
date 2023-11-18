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

struct UTP_ItemList {
    std::string InventoryRes;
    uint16_t Repos_PosX {0};
    uint16_t Repos_Posy {0};
};

struct UTP {
    uint8_t AnimationState {0};
    uint32_t Appearance {0};
    uint8_t AutoRemoveKey {0};
    uint8_t BodyBag {0};
    uint8_t CloseLockDC {0};
    std::string Comment;
    std::string Conversation;
    int16_t CurrentHP {0};
    std::pair<int, std::string> Description;
    uint8_t DisarmDC {0};
    uint32_t Faction {0};
    uint8_t Fort {0};
    int16_t HP {0};
    uint8_t Hardness {0};
    uint8_t HasInventory {0};
    uint8_t Interruptable {0};
    uint8_t IsComputer {0};
    std::vector<UTP_ItemList> ItemList;
    std::string KeyName;
    uint8_t KeyRequired {0};
    std::pair<int, std::string> LocName;
    uint8_t Lockable {0};
    uint8_t Locked {0};
    uint8_t Min1HP {0};
    uint8_t NotBlastable {0};
    std::string OnClosed;
    std::string OnDamaged;
    std::string OnDeath;
    std::string OnDisarm;
    std::string OnEndDialogue;
    std::string OnFailToOpen;
    std::string OnHeartbeat;
    std::string OnInvDisturbed;
    std::string OnLock;
    std::string OnMeleeAttacked;
    std::string OnOpen;
    std::string OnSpellCastAt;
    std::string OnTrapTriggered;
    std::string OnUnlock;
    std::string OnUsed;
    std::string OnUserDefined;
    uint8_t OpenLockDC {0};
    uint8_t OpenLockDiff {0};
    char OpenLockDiffMod {'\0'};
    uint8_t PaletteID {0};
    uint8_t PartyInteract {0};
    uint8_t Plot {0};
    std::string Portrait;
    uint16_t PortraitId {0};
    uint8_t Ref {0};
    uint8_t Static {0};
    std::string Tag;
    std::string TemplateResRef;
    uint8_t TrapDetectDC {0};
    uint8_t TrapDetectable {0};
    uint8_t TrapDisarmable {0};
    uint8_t TrapFlag {0};
    uint8_t TrapOneShot {0};
    uint8_t TrapType {0};
    uint8_t Type {0};
    uint8_t Useable {0};
    uint8_t Will {0};
};

UTP parseUTP(const resource::Gff &gff);

} // namespace gffschema

} // namespace game

} // namespace reone
