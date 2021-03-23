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

#include "../../common/log.h"

#include "../object/creature.h"
#include "../types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getLastPerceived(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result.object = caller->perception().lastPerceived;
    } else {
        debug("Script: getLastPerceived: caller is invalid");
    }

    return move(result);
}

Variable Routines::getLastPerceptionSeen(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result.intValue = caller->perception().lastPerception == PerceptionType::Seen;
    } else {
        debug("Script: getLastPerceptionSeen: caller is invalid");
    }

    return move(result);
}

Variable Routines::getLastPerceptionVanished(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result.intValue = caller->perception().lastPerception == PerceptionType::NotSeen;
    } else {
        debug("Script: getLastPerceptionVanished: caller is invalid");
    }

    return move(result);
}

Variable Routines::getLastPerceptionHeard(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result.intValue = caller->perception().lastPerception == PerceptionType::Heard;
    } else {
        debug("Script: getLastPerceptionHeard: caller is invalid");
    }

    return move(result);
}

Variable Routines::getLastPerceptionInaudible(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result.intValue = caller->perception().lastPerception == PerceptionType::NotHeard;
    } else {
        debug("Script: getLastPerceptionInaudible: caller is invalid");
    }

    return move(result);
}

Variable Routines::getObjectSeen(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto target = getCreature(args, 0);
    if (!target) {
        debug("Script: getObjectSeen: target is invalid");
    }

    auto source = getCreatureOrCaller(args, 1, ctx);
    if (!source) {
        debug("Script: getObjectSeen: source is invalid");
    }

    if (target && source) {
        result.intValue = source->perception().seen.count(target) > 0 ? 1 : 0;
    }

    return move(result);
}

Variable Routines::getObjectHeard(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto target = getCreature(args, 0);
    if (!target) {
        debug("Script: getObjectHeard: target is invalid");
    }

    auto source = getCreatureOrCaller(args, 1, ctx);
    if (!source) {
        debug("Script: getObjectHeard: source is invalid");
    }

    if (target && source) {
        result.intValue = source->perception().heard.count(target) > 0 ? 1 : 0;
    }

    return move(result);
}

} // namespace game

} // namespace reone
