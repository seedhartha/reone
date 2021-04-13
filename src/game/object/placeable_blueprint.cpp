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
 *  Placeable functions related to blueprint loading.
 */

#include "placeable.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Placeable::loadUTP(const GffStruct &utp) {
    _tag = boost::to_lower_copy(utp.getString("Tag"));
    _name = Strings::instance().get(utp.getInt("LocName"));
    _blueprintResRef = boost::to_lower_copy(utp.getString("TemplateResRef"));
    _conversation = boost::to_lower_copy(utp.getString("Conversation"));
    _interruptable = utp.getBool("Interruptable");
    _faction = utp.getEnum("Faction", Faction::Invalid);
    _plot = utp.getBool("Plot");
    _minOneHP = utp.getBool("Min1HP");
    _keyRequired = utp.getBool("KeyRequired");
    _lockable = utp.getBool("Lockable");
    _locked = utp.getBool("Locked");
    _openLockDC = utp.getInt("OpenLockDC");
    _animationState = utp.getInt("AnimationState");
    _appearance = utp.getInt("Appearance");
    _hitPoints = utp.getInt("HP");
    _currentHitPoints = utp.getInt("CurrentHP");
    _hardness = utp.getInt("Hardness");
    _fortitude = utp.getInt("Fort");
    _hasInventory = utp.getBool("HasInventory");
    _partyInteract = utp.getBool("PartyInteract");
    _static = utp.getBool("Static");
    _usable = utp.getBool("Useable");

    _onClosed = boost::to_lower_copy(utp.getString("OnClosed"));
    _onDamaged = boost::to_lower_copy(utp.getString("OnDamaged")); // always empty, but could be useful
    _onDeath = boost::to_lower_copy(utp.getString("OnDeath"));
    _onHeartbeat = boost::to_lower_copy(utp.getString("OnHeartbeat"));
    _onLock = boost::to_lower_copy(utp.getString("OnLock")); // always empty, but could be useful
    _onMeleeAttacked = boost::to_lower_copy(utp.getString("OnMeleeAttacked")); // always empty, but could be useful
    _onOpen = boost::to_lower_copy(utp.getString("OnOpen"));
    _onSpellCastAt = boost::to_lower_copy(utp.getString("OnSpellCastAt"));
    _onUnlock = boost::to_lower_copy(utp.getString("OnUnlock")); // always empty, but could be useful
    _onUserDefined = boost::to_lower_copy(utp.getString("OnUserDefined"));
    _onEndDialogue = boost::to_lower_copy(utp.getString("OnEndDialogue"));
    _onInvDisturbed = boost::to_lower_copy(utp.getString("OnInvDisturbed"));
    _onUsed = boost::to_lower_copy(utp.getString("OnUsed"));

    for (auto &itemGffs : utp.getList("ItemList")) {
        string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
        addItem(resRef, 1, true);
    }

    // These fields are ignored as being most likely unused:
    //
    // - Description (not applicable, mostly -1)
    // - AutoRemoveKey (not applicable, always 0)
    // - CloseLockDC (not applicable, always 0)
    // - PortraitId (not applicable, always 0)
    // - TrapDetectable (not applicable, always 1)
    // - TrapDetectDC (not applicable, always 0)
    // - TrapDisarmable (not applicable, always 1)
    // - DisarmDC (not applicable)
    // - TrapFlag (not applicable, always 0)
    // - TrapOneShot (not applicable, always 1)
    // - TrapType (not applicable, mostly 0)
    // - KeyName (not applicable, always empty)
    // - Ref (not applicable, always 0)
    // - Will (not applicable, always 0)
    // - Type (obsolete, always 0)
    // - OnDisarm (not applicable, always empty)
    // - OnTrapTriggered (not applicable, always empty)
    // - BodyBag (not applicable, always 0)
    // - Type (obsolete, always 0)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

} // namespace game

} // namespace reone
