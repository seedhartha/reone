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
 *  Implementation of routines related to party management.
 */

#include "routines.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../common/log.h"

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::addAvailableNPCByTemplate(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    string blueprint(boost::to_lower_copy(getString(args, 1)));

    bool added = _game->party().addAvailableMember(npc, blueprint);

    return Variable::ofInt(static_cast<int>(added));
}

Variable Routines::showPartySelectionGUI(const VariablesList &args, ExecutionContext &ctx) {
    string exitScript(boost::to_lower_copy(getString(args, 0, "")));
    int forceNpc1 = getInt(args, 1);
    int forceNpc2 = getInt(args, 2);

    PartySelection::Context partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    _game->openPartySelection(partyCtx);

    return Variable();
}

Variable Routines::getIsPC(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature == _game->party().player();
    } else {
        debug("Script: getIsPC: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::isAvailableCreature(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isAvailable = _game->party().isMemberAvailable(npc);
    return Variable::ofInt(static_cast<int>(isAvailable));
}

Variable Routines::isObjectPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = _game->party().isMember(*creature);
    } else {
        debug("Script: isObjectPartyMember: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getPartyMemberByIndex(const VariablesList &args, ExecutionContext &ctx) {
    int index = getInt(args, 0);
    return Variable::ofObject(_game->party().getMember(index));
}

Variable Routines::getPCSpeaker(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(_game->party().player());
}

Variable Routines::isNPCPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isMember = _game->party().isMember(npc);
    return Variable::ofInt(static_cast<int>(isMember));
}

Variable Routines::setPartyLeader(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    _game->party().setPartyLeader(npc);
    return Variable();
}

Variable Routines::addPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    int npc = getInt(args, 0);
    auto creature = getCreature(args, 1);

    if (creature) {
        _game->party().addAvailableMember(npc, creature->blueprintResRef());
    } else {
        debug("Script: addPartyMember: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::removePartyMember(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    int npc = getInt(args, 0);

    if (_game->party().isMember(npc)) {
        _game->party().removeMember(npc);

        shared_ptr<Area> area(_game->module()->area());
        area->unloadParty();
        area->reloadParty();

        result = true;
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getFirstPC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(_game->party().player());
}

Variable Routines::getPartyMemberCount(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game->party().getSize());
}

Variable Routines::removeAvailableNPC(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool removed = _game->party().removeAvailableMember(npc);
    return Variable::ofInt(static_cast<int>(removed));
}

Variable Routines::getPartyLeader(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(_game->party().getLeader());
}

Variable Routines::getSoloMode(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(_game->party().isSoloMode()));
}

Variable Routines::setSoloMode(const VariablesList &args, ExecutionContext &ctx) {
    auto activate = getBool(args, 0);
    _game->party().setSoloMode(activate);
    return Variable();
}

} // namespace game

} // namespace reone
