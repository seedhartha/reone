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

#include "declarations.h"

#include "../../../../common/logutil.h"
#include "../../../../common/randomutil.h"
#include "../../../../script/exception/notimpl.h"
#include "../../../../script/types.h"

#include "../../../game.h"

#include "argutil.h"
#include "objectutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable setPartyLeader(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);

    game.services().party().setPartyLeader(npc);

    return Variable::ofNull();
}

Variable getPartyMemberCount(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(game.services().party().getSize());
}

Variable addToParty(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable removeFromParty(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addPartyMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    auto creature = getCreature(game, args, 1, ctx);

    bool result = game.services().party().addAvailableMember(npc, creature->blueprintResRef());

    return Variable::ofInt(static_cast<int>(result));
}

Variable removePartyMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    int npc = getInt(args, 0);

    if (game.services().party().isMember(npc)) {
        game.services().party().removeMember(npc);

        shared_ptr<Area> area(game.module()->area());
        area->unloadParty();
        area->reloadParty();

        result = true;
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable isObjectPartyMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(game.services().party().isMember(*creature)));
}

Variable getPartyMemberByIndex(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int index = getInt(args, 0);
    auto member = game.services().party().getMember(index);

    return Variable::ofObject(getObjectIdOrInvalid(member));
}

Variable addAvailableNPCByObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable removeAvailableNPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool removed = game.services().party().removeAvailableMember(npc);

    return Variable::ofInt(static_cast<int>(removed));
}

Variable isAvailableCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isAvailable = game.services().party().isMemberAvailable(npc);

    return Variable::ofInt(static_cast<int>(isAvailable));
}

Variable addAvailableNPCByTemplate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    string blueprint(boost::to_lower_copy(getString(args, 1)));

    bool added = game.services().party().addAvailableMember(npc, blueprint);

    return Variable::ofInt(static_cast<int>(added));
}

Variable spawnAvailableNPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable isNPCPartyMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = getInt(args, 0);
    bool isMember = game.services().party().isMember(npc);

    return Variable::ofInt(static_cast<int>(isMember));
}

Variable getPartyAIStyle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: implement
    return Variable::ofInt(static_cast<int>(PartyAIStyle::Aggressive));
}

Variable setPartyAIStyle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable saveNPCState(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setAvailableNPCId(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addAvailablePUPByTemplate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addAvailablePUPByObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable assignPUP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable spawnAvailablePUP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addPartyPuppet(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPUPOwner(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPuppet(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPartyLeader(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPartyLeader(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto player = game.services().party().getLeader();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable removeNPCFromPartyToBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable saveNPCByObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable savePUPByObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable rebuildPartyTable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
