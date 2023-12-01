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

#include "reone/resource/template/generated/utp.h"

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

namespace generated {

static UTP_ItemList parseUTP_ItemList(const Gff &gff) {
    UTP_ItemList strct;
    strct.InventoryRes = gff.getString("InventoryRes");
    strct.Repos_PosX = gff.getUint("Repos_PosX");
    strct.Repos_Posy = gff.getUint("Repos_Posy");
    return strct;
}

UTP parseUTP(const Gff &gff) {
    UTP strct;
    strct.AnimationState = gff.getUint("AnimationState");
    strct.Appearance = gff.getUint("Appearance");
    strct.AutoRemoveKey = gff.getUint("AutoRemoveKey");
    strct.BodyBag = gff.getUint("BodyBag");
    strct.CloseLockDC = gff.getUint("CloseLockDC");
    strct.Comment = gff.getString("Comment");
    strct.Conversation = gff.getString("Conversation");
    strct.CurrentHP = gff.getInt("CurrentHP");
    strct.Description = std::make_pair(gff.getInt("Description"), gff.getString("Description"));
    strct.DisarmDC = gff.getUint("DisarmDC");
    strct.Faction = gff.getUint("Faction");
    strct.Fort = gff.getUint("Fort");
    strct.HP = gff.getInt("HP");
    strct.Hardness = gff.getUint("Hardness");
    strct.HasInventory = gff.getUint("HasInventory");
    strct.Interruptable = gff.getUint("Interruptable");
    strct.IsComputer = gff.getUint("IsComputer");
    for (auto &item : gff.getList("ItemList")) {
        strct.ItemList.push_back(parseUTP_ItemList(*item));
    }
    strct.KeyName = gff.getString("KeyName");
    strct.KeyRequired = gff.getUint("KeyRequired");
    strct.LocName = std::make_pair(gff.getInt("LocName"), gff.getString("LocName"));
    strct.Lockable = gff.getUint("Lockable");
    strct.Locked = gff.getUint("Locked");
    strct.Min1HP = gff.getUint("Min1HP");
    strct.NotBlastable = gff.getUint("NotBlastable");
    strct.OnClosed = gff.getString("OnClosed");
    strct.OnDamaged = gff.getString("OnDamaged");
    strct.OnDeath = gff.getString("OnDeath");
    strct.OnDisarm = gff.getString("OnDisarm");
    strct.OnEndDialogue = gff.getString("OnEndDialogue");
    strct.OnFailToOpen = gff.getString("OnFailToOpen");
    strct.OnHeartbeat = gff.getString("OnHeartbeat");
    strct.OnInvDisturbed = gff.getString("OnInvDisturbed");
    strct.OnLock = gff.getString("OnLock");
    strct.OnMeleeAttacked = gff.getString("OnMeleeAttacked");
    strct.OnOpen = gff.getString("OnOpen");
    strct.OnSpellCastAt = gff.getString("OnSpellCastAt");
    strct.OnTrapTriggered = gff.getString("OnTrapTriggered");
    strct.OnUnlock = gff.getString("OnUnlock");
    strct.OnUsed = gff.getString("OnUsed");
    strct.OnUserDefined = gff.getString("OnUserDefined");
    strct.OpenLockDC = gff.getUint("OpenLockDC");
    strct.OpenLockDiff = gff.getUint("OpenLockDiff");
    strct.OpenLockDiffMod = gff.getInt("OpenLockDiffMod");
    strct.PaletteID = gff.getUint("PaletteID");
    strct.PartyInteract = gff.getUint("PartyInteract");
    strct.Plot = gff.getUint("Plot");
    strct.Portrait = gff.getString("Portrait");
    strct.PortraitId = gff.getUint("PortraitId");
    strct.Ref = gff.getUint("Ref");
    strct.Static = gff.getUint("Static");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.TrapDetectDC = gff.getUint("TrapDetectDC");
    strct.TrapDetectable = gff.getUint("TrapDetectable");
    strct.TrapDisarmable = gff.getUint("TrapDisarmable");
    strct.TrapFlag = gff.getUint("TrapFlag");
    strct.TrapOneShot = gff.getUint("TrapOneShot");
    strct.TrapType = gff.getUint("TrapType");
    strct.Type = gff.getUint("Type");
    strct.Useable = gff.getUint("Useable");
    strct.Will = gff.getUint("Will");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
