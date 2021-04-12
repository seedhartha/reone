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
 *  Door functions related to blueprint loading.
 */

#include "door.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Door::loadUTD(const GffStruct &utd) {
    _tag = boost::to_lower_copy(utd.getString("Tag"));
    _name = Strings::instance().get(utd.getInt("LocName"));
    _blueprintResRef = boost::to_lower_copy(utd.getString("TemplateResRef"));
    _conversation = boost::to_lower_copy(utd.getString("Conversation"));
    _interruptable = utd.getBool("Interruptable");
    _faction = utd.getEnum("Faction", Faction::Invalid);
    _plot = utd.getBool("Plot");
    _minOneHP = utd.getBool("Min1HP");
    _keyRequired = utd.getBool("KeyRequired");
    _locked = utd.getBool("Locked");
    _openLockDC = utd.getInt("OpenLockDC");
    _animationState = utd.getInt("AnimationState");
    _appearance = utd.getInt("Appearance");
    _hitPoints = utd.getInt("HP");
    _currentHitPoints = utd.getInt("CurrentHP");
    _hardness = utd.getInt("Hardness");
    _fortitude = utd.getInt("Fort");
    _reflex = utd.getInt("Ref");
    _will = utd.getInt("Will");
    _loadScreenID = utd.getInt("LoadScreenID");
    _genericType = utd.getInt("GenericType");
    _static = utd.getBool("Static");

    _onClosed = utd.getString("OnClosed");
    _onDamaged = utd.getString("OnDamaged");
    _onDeath = utd.getString("OnDeath");
    _onHeartbeat = utd.getString("OnHeartbeat");
    _onLock = utd.getString("OnLock");
    _onMeleeAttacked = utd.getString("OnMeleeAttacked");
    _onOpen = utd.getString("OnOpen");
    _onSpellCastAt = utd.getString("OnSpellCastAt");
    _onUnlock = utd.getString("OnUnlock");
    _onUserDefined = utd.getString("OnUserDefined");
    _onClick = utd.getString("OnClick");
    _onFailToOpen = utd.getString("OnFailToOpen");

    // These fields are ignored as being most likely unused:
    //
    // - AutoRemoveKey
    // - CloseLockDC
    // - Lockable
    // - PortraitId
    // - TrapDetectable
    // - TrapDetectDC
    // - TrapDisarmable
    // - DisarmDC
    // - TrapFlag
    // - TrapOneShot
    // - TrapType
    // - KeyName
    // - OnDisarm
    // - OnLock
    // - OnTrapTriggered
}

} // namespace game

} // namespace reone
