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

#include "../../../../script/exception/notimpl.h"

#include "../../../core/object/creature.h"
#include "../../../core/party.h"
#include "../../../core/script/routine/context.h"
#include "../../../core/script/routine/objectutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

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

} // namespace routine

} // namespace game

} // namespace reone
