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

#include "../../../../common/logutil.h"
#include "../../../../script/types.h"

#include "../../../object/creature.h"

#include "argutil.h"
#include "objectutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable getLastPerceived(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    auto object = caller->perception().lastPerceived;

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getLastPerceptionHeard(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    bool heard = caller->perception().lastPerception == PerceptionType::Heard;

    return Variable::ofInt(static_cast<int>(heard));
}

Variable getLastPerceptionInaudible(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    bool notHeard = caller->perception().lastPerception == PerceptionType::NotHeard;

    return Variable::ofInt(static_cast<int>(notHeard));
}

Variable getLastPerceptionSeen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    bool seen = caller->perception().lastPerception == PerceptionType::Seen;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getLastPerceptionVanished(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    bool vanished = caller->perception().lastPerception == PerceptionType::NotSeen;

    return Variable::ofInt(static_cast<int>(vanished));
}

Variable getObjectSeen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    bool seen = source->perception().seen.count(target) > 0;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getObjectHeard(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    bool heard = source->perception().heard.count(target) > 0;

    return Variable::ofInt(static_cast<int>(heard));
}

} // namespace routine

} // namespace game

} // namespace reone
