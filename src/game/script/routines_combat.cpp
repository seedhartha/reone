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

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getAttackTarget(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        result.object = creature->combat().attackTarget;
    } else {
        warn("Script: getAttackTarget: creature is invalid");
    }

    return move(result);
}

Variable Routines::getAttemptedAttackTarget(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result.object = caller->combat().attemptedAttackTarget;
    } else {
        warn("Script: getAttemptedAttackTarget: caller is invalid");
    }

    return move(result);
}

Variable Routines::getSpellTarget(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        result.object = creature->combat().spellTarget;
    } else {
        warn("Script: getSpellTarget: creature is invalid");
    }

    return move(result);
}

Variable Routines::getAttemptedSpellTarget(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result.object = caller->combat().attemptedSpellTarget;
    } else {
        warn("Script: getAttemptedSpellTarget: caller is invalid");
    }

    return move(result);
}

} // namespace game

} // namespace reone
