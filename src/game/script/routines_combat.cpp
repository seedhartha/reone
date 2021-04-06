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
 *  Implementation of combat-related routines.
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
    auto result = NPCAIStyle::DefaultAttack;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature->aiStyle();
    } else {
        debug("Script: getNPCAIStyle: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setNPCAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    auto style = getEnum<NPCAIStyle>(args, 1);

    if (creature) {
        creature->setAIStyle(style);
    } else {
        debug("Script: setNPCAIStyle: creature is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getAttackTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> result;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        creature->getAttackTarget();
    } else {
        debug("Script: getAttackTarget: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getAttemptedAttackTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> result;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        caller->getAttemptedAttackTarget();
    } else {
        debug("Script: getAttemptedAttackTarget: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getSpellTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> result;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        // TODO: implement
    } else {
        debug("Script: getSpellTarget: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getAttemptedSpellTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> result;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        // TODO: implement
    } else {
        debug("Script: getAttemptedSpellTarget: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getIsDebilitated(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        result = creature->isDebilitated();
    } else {
        debug("Script: getIsDebilitated: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLastHostileTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> result;

    auto attacker = getCreatureOrCaller(args, 0, ctx);
    if (attacker) {
        // TODO: implement
    } else {
        debug("Script: getLastHostileTarget: attacker is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(result);
}

Variable Routines::getLastAttackAction(const VariablesList &args, ExecutionContext &ctx) {
    auto result = ActionType::QueueEmpty;

    auto attacker = getCreatureOrCaller(args, 0, ctx);
    if (attacker) {
        // TODO: implement
    } else {
        debug("Script: getLastAttackAction: attacker is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getPlayerRestrictMode(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: why is this object necessary?
    auto object = getCreatureOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(_game->module()->player().isRestrictMode()));
}

Variable Routines::setPlayerRestrictMode(const VariablesList &args, ExecutionContext &ctx) {
    bool restrict = getBool(args, 0);
    _game->module()->player().setRestrictMode(restrict);
    return Variable();
}

Variable Routines::getUserActionsPending(const VariablesList &args, ExecutionContext &ctx) {
    bool result = 0;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result = caller->actionQueue().containsUserActions();
    } else {
        debug("Script: getUserActionsPending: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

} // namespace game

} // namespace reone
