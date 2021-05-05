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
 *  Implementation of routines related to creature perception.
 */

#include "routines.h"

#include "../../common/log.h"

#include "../object/creature.h"
#include "../types.h"

#include "objectutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getLastPerceived(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> object;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        object = caller->perception().lastPerceived;
    } else {
        debug("Script: getLastPerceived: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable Routines::getLastPerceptionSeen(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result = caller->perception().lastPerception == PerceptionType::Seen;
    } else {
        debug("Script: getLastPerceptionSeen: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLastPerceptionVanished(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result = caller->perception().lastPerception == PerceptionType::NotSeen;
    } else {
        debug("Script: getLastPerceptionVanished: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLastPerceptionHeard(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result = caller->perception().lastPerception == PerceptionType::Heard;
    } else {
        debug("Script: getLastPerceptionHeard: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLastPerceptionInaudible(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result = caller->perception().lastPerception == PerceptionType::NotHeard;
    } else {
        debug("Script: getLastPerceptionInaudible: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getObjectSeen(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0, ctx);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = source->perception().seen.count(target) > 0;
    } else if (!target) {
        debug("Script: getObjectSeen: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getObjectSeen: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getObjectHeard(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0, ctx);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = source->perception().heard.count(target) > 0;
    } else if (!target) {
        debug("Script: getObjectHeard: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getObjectHeard: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(move(result)));
}

} // namespace game

} // namespace reone
