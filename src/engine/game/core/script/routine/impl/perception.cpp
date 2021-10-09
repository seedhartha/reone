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
 *  Implementation of routines related to perception.
 */

#include "declarations.h"

#include "../../../../../common/logutil.h"
#include "../../../../../script/types.h"

#include "../../../object/creature.h"

#include "../context.h"

#include "argutil.h"
#include "objectutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable getLastPerceived(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto object = caller->perception().lastPerceived;

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getLastPerceptionHeard(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool heard = caller->perception().lastPerception == PerceptionType::Heard;

    return Variable::ofInt(static_cast<int>(heard));
}

Variable getLastPerceptionInaudible(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool notHeard = caller->perception().lastPerception == PerceptionType::NotHeard;

    return Variable::ofInt(static_cast<int>(notHeard));
}

Variable getLastPerceptionSeen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool seen = caller->perception().lastPerception == PerceptionType::Seen;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getLastPerceptionVanished(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool vanished = caller->perception().lastPerception == PerceptionType::NotSeen;

    return Variable::ofInt(static_cast<int>(vanished));
}

Variable getObjectSeen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getCreature(args, 0, ctx);
    auto source = getCreatureOrCaller(args, 1, ctx);

    bool seen = source->perception().seen.count(target) > 0;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getObjectHeard(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getCreature(args, 0, ctx);
    auto source = getCreatureOrCaller(args, 1, ctx);

    bool heard = source->perception().heard.count(target) > 0;

    return Variable::ofInt(static_cast<int>(heard));
}

} // namespace routine

} // namespace game

} // namespace reone
