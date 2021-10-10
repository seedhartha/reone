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

#include "../declarations.h"

#include "../../../../../common/logutil.h"
#include "../../../../../common/randomutil.h"
#include "../../../../../script/exception/notimpl.h"
#include "../../../../../script/types.h"

#include "../../../game.h"
#include "../../../object/creature.h"
#include "../../../party.h"
#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"
#include "../../../script/routine/objectutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable setPartyLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    ctx.party.setPartyLeader(npc);

    return Variable::ofNull();
}

Variable getPartyMemberCount(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.party.getSize());
}

Variable addToParty(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable removeFromParty(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable addPartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto creature = getCreature(args, 1, ctx);

    bool result = ctx.party.addAvailableMember(npc, creature->blueprintResRef());

    return Variable::ofInt(static_cast<int>(result));
}

Variable removePartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
    bool result = false;
    int npc = getInt(args, 0);

    if (ctx.party.isMember(npc)) {
        ctx.party.removeMember(npc);

        shared_ptr<Area> area(ctx.game.module()->area());
        area->unloadParty();
        area->reloadParty();

        result = true;
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable isObjectPartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(ctx.party.isMember(*creature)));
}

Variable getPartyMemberByIndex(const vector<Variable> &args, const RoutineContext &ctx) {
    int index = getInt(args, 0);
    auto member = ctx.party.getMember(index);

    return Variable::ofObject(getObjectIdOrInvalid(member));
}

Variable addAvailableNPCByObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable removeAvailableNPC(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool removed = ctx.party.removeAvailableMember(npc);

    return Variable::ofInt(static_cast<int>(removed));
}

Variable isAvailableCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool isAvailable = ctx.party.isMemberAvailable(npc);

    return Variable::ofInt(static_cast<int>(isAvailable));
}

Variable addAvailableNPCByTemplate(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    string blueprint(boost::to_lower_copy(getString(args, 1)));

    bool added = ctx.party.addAvailableMember(npc, blueprint);

    return Variable::ofInt(static_cast<int>(added));
}

Variable spawnAvailableNPC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable isNPCPartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool isMember = ctx.party.isMember(npc);

    return Variable::ofInt(static_cast<int>(isMember));
}

Variable getPartyAIStyle(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement
    return Variable::ofInt(static_cast<int>(PartyAIStyle::Aggressive));
}

Variable setPartyAIStyle(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable saveNPCState(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setAvailableNPCId(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

// TSL

Variable addAvailablePUPByTemplate(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable addAvailablePUPByObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable assignPUP(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable spawnAvailablePUP(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable addPartyPuppet(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getPUPOwner(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPuppet(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPartyLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getPartyLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.party.getLeader();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable removeNPCFromPartyToBase(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable saveNPCByObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable savePUPByObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable rebuildPartyTable(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

// END TSL

} // namespace routine

} // namespace game

} // namespace reone
