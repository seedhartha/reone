/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "routines_common.h"

#include <cassert>
#include <cstdint>

#include "SDL2/SDL_timer.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable delayCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Float &&
        args[1].type == VariableType::Action);

    if (ctx.delayCommand) {
        uint32_t timestamp = SDL_GetTicks() + static_cast<int>(args[0].floatValue * 1000.0f);
        ctx.delayCommand(timestamp, args[1].context);
    }

    return Variable();
}

Variable assignCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Action);

    ExecutionContext newCtx(args[1].context);
    newCtx.callerId = args[0].objectId;

    ctx.delayCommand(SDL_GetTicks(), newCtx);

    return Variable();
}

Variable getEnteringObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.enteringObjectId;
    return move(result);
}

Variable getIsPC(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);
    return Variable(args[0].objectId == ctx.playerId);
}

Variable getIsObjectValid(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);
    return Variable(args[0].objectId != kObjectInvalid);
}

Variable getFirstPC(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.playerId;
    return move(result);
}

Variable getObjectByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::String);

    Variable result(VariableType::Object);
    result.objectId = ctx.getObjectByTag ? ctx.getObjectByTag(args[0].strValue) : 0;

    return move(result);
}

Variable getLevelByClass(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        !args.empty() &&
        args[0].type == VariableType::Int &&
        args[1].type == VariableType::Object);

    int clazz = args[0].intValue;
    int objectId = args.size() < 2 ? 0 : args[1].objectId;

    // TODO: return value based on class

    return Variable(1);
}

Variable getGender(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    int objectId = args[0].objectId;

    return Variable();
}

Variable actionStartConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    int objectId = args[0].objectId;
    string resRef(args.size() >= 2 ? args[1].strValue : "");

    if (ctx.startDialog) {
        ctx.startDialog(objectId, resRef);
    }

    return Variable();
}

} // namespace game

} // namespace reone
