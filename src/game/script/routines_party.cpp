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

    string blueprint(getString(args, 1));
    boost::to_lower(blueprint);

    bool added = _game->party().addAvailableMember(npc, blueprint);

    return added ? 1 : 0;
}

Variable Routines::showPartySelectionGUI(const VariablesList &args, ExecutionContext &ctx) {
    string exitScript(getString(args, 0, ""));
    boost::to_lower(exitScript);

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
    auto creature = getCreature(args, 0);
    if (!creature) {
        warn("Routines: getIsPC: creature is invalid");
        return 0;
    }
    auto player = _game->party().player();
    return player == creature ? 1 : 0;
}

Variable Routines::isAvailableCreature(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isAvailable = _game->party().isMemberAvailable(npc);
    return isAvailable ? 1 : 0;
}

Variable Routines::isObjectPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (!creature) {
        warn("Routines: isObjectPartyMember: creature is invalid");
        return 0;
    }
    return _game->party().isMember(*creature) ? 1 : 0;
}

Variable Routines::getPartyMemberByIndex(const VariablesList &args, ExecutionContext &ctx) {
    int index = getInt(args, 0);
    return static_pointer_cast<ScriptObject>(_game->party().getMember(index));
}

Variable Routines::getPCSpeaker(const VariablesList &args, ExecutionContext &ctx) {
    return static_pointer_cast<ScriptObject>(_game->party().player());
}

Variable Routines::isNPCPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isMember = _game->party().isMember(npc);
    return isMember ? 1 : 0;
}

Variable Routines::setPartyLeader(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    _game->party().setPartyLeader(npc);
    return Variable();
}

Variable Routines::addPartyMember(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 1);
    if (!creature) {
        warn("Routines: addPartyMember: creature is invalid");
        return 0;
    }
    int npc = getInt(args, 0);
    _game->party().addAvailableMember(npc, creature->blueprintResRef());

    return 1;
}

Variable Routines::removePartyMember(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    if (!_game->party().isMember(npc)) return 0;

    _game->party().removeMember(npc);

    shared_ptr<Area> area(_game->module()->area());
    area->unloadParty();
    area->reloadParty();

    return 1;
}

Variable Routines::getFirstPC(const VariablesList &args, ExecutionContext &ctx) {
    return static_pointer_cast<ScriptObject>(_game->party().player());
}

Variable Routines::getPartyMemberCount(const VariablesList &args, ExecutionContext &ctx) {
    return _game->party().getSize();
}

Variable Routines::removeAvailableNPC(const VariablesList &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool removed = _game->party().removeAvailableMember(npc);
    return removed ? 1 : 0;
}

Variable Routines::getPartyLeader(const VariablesList &args, ExecutionContext &ctx) {
    return static_pointer_cast<ScriptObject>(_game->party().getLeader());
}

} // namespace game

} // namespace reone
