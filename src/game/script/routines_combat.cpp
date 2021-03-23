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

#include "../game.h"
#include "../object/creature.h"
#include "../types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getNPCAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto creature = getCreature(args, 0);
    if (creature) {
        result.intValue = static_cast<int>(creature->aiStyle());
    } else {
        debug("Script: getNPCAIStyle: creature is invalid");
    }

    return move(result);
}

Variable Routines::setNPCAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (creature) {
        auto style = static_cast<NPCAIStyle>(getInt(args, 1));
        creature->setAIStyle(style);
    } else {
        debug("Script: setNPCAIStyle: creature is invalid");
    }

    return Variable();
}

Variable Routines::getAttackTarget(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        result.object = creature->combat().attackTarget;
    } else {
        debug("Script: getAttackTarget: creature is invalid");
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
        debug("Script: getAttemptedAttackTarget: caller is invalid");
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
        debug("Script: getSpellTarget: creature is invalid");
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
        debug("Script: getAttemptedSpellTarget: caller is invalid");
    }

    return move(result);
}

Variable Routines::getIsDebilitated(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        result.intValue = creature->combat().debilitated ? 1 : 0;
    } else {
        debug("Script: getIsDebilitated: creature is invalid");
    }

    return move(result);
}

Variable Routines::getLastHostileTarget(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto attacker = getCreatureOrCaller(args, 0, ctx);
    if (attacker) {
        result.object = attacker->combat().lastHostileTarget;
    } else {
        debug("Script: getLastHostileTarget: attacker is invalid");
    }

    return move(result);
}

Variable Routines::getLastAttackAction(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto attacker = getCreatureOrCaller(args, 0, ctx);
    if (attacker) {
        result.intValue = attacker->combat().debilitated ? 1 : 0;
    } else {
        debug("Script: getLastAttackAction: attacker is invalid");
    }

    return move(result);
}

Variable Routines::getPlayerRestrictMode(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: why is this object necessary?
    auto object = getCreatureOrCaller(args, 0, ctx);

    return Variable(_game->module()->player().isRestrictMode() ? 1 : 0);
}

Variable Routines::setPlayerRestrictMode(const VariablesList &args, ExecutionContext &ctx) {
    bool restrict = getBool(args, 0);
    _game->module()->player().setRestrictMode(restrict);

    return Variable();
}

Variable Routines::getUserActionsPending(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    shared_ptr<Creature> leader(_game->party().getLeader());
    if (leader) {
        result.intValue = leader->actionQueue().containsUserActions() ? 1 : 0;
    }

    return move(result);
}

} // namespace game

} // namespace reone
