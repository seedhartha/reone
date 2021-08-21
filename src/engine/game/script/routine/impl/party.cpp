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
 *  Implementation of party-related routines.
 */

#include "../../routines.h"

#include "../../../../common/log.h"
#include "../../../../common/random.h"

#include "../../../game.h"

#include "objectutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::setPartyLeader(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    _game.services().party().setPartyLeader(npc);
    return Variable();
}

Variable Routines::getPartyMemberCount(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game.services().party().getSize());
}

Variable Routines::addToParty(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::removeFromParty(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    int npc = getInt(args, 0);
    auto creature = getCreature(args, 1, ctx);

    if (creature) {
        _game.services().party().addAvailableMember(npc, creature->blueprintResRef());
    } else {
        debug("Script: addPartyMember: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::removePartyMember(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    int npc = getInt(args, 0);

    if (_game.services().party().isMember(npc)) {
        _game.services().party().removeMember(npc);

        shared_ptr<Area> area(_game.module()->area());
        area->unloadParty();
        area->reloadParty();

        result = true;
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::isObjectPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = _game.services().party().isMember(*creature);
    } else {
        debug("Script: isObjectPartyMember: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getPartyMemberByIndex(const VariablesList &args, ExecutionContext &ctx) {
    int index = getInt(args, 0);
    auto member = _game.services().party().getMember(index);
    return Variable::ofObject(getObjectIdOrInvalid(member));
}

Variable Routines::addAvailableNPCByObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::removeAvailableNPC(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool removed = _game.services().party().removeAvailableMember(npc);
    return Variable::ofInt(static_cast<int>(removed));
}

Variable Routines::isAvailableCreature(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isAvailable = _game.services().party().isMemberAvailable(npc);
    return Variable::ofInt(static_cast<int>(isAvailable));
}

Variable Routines::addAvailableNPCByTemplate(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    string blueprint(boost::to_lower_copy(getString(args, 1)));

    bool added = _game.services().party().addAvailableMember(npc, blueprint);

    return Variable::ofInt(static_cast<int>(added));
}

Variable Routines::spawnAvailableNPC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::isNPCPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isMember = _game.services().party().isMember(npc);
    return Variable::ofInt(static_cast<int>(isMember));
}

Variable Routines::getPartyAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setPartyAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::saveNPCState(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setAvailableNPCId(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addAvailablePUPByTemplate(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addAvailablePUPByObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::assignPUP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::spawnAvailablePUP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addPartyPuppet(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getPUPOwner(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsPuppet(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsPartyLeader(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getPartyLeader(const VariablesList &args, ExecutionContext &ctx) {
    auto player = _game.services().party().getLeader();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable Routines::removeNPCFromPartyToBase(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::saveNPCByObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::savePUPByObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::rebuildPartyTable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

} // namespace game

} // namespace reone
