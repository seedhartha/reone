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
 *  Implementation of routines related to creature factions.
 */

#include "routines.h"

#include "../../common/log.h"

#include "../reputes.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::changeFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto objectToChangeFaction = getCreature(args, 0);
    auto memberOfFactionToJoin = getCreature(args, 1);

    if (objectToChangeFaction && memberOfFactionToJoin) {
        objectToChangeFaction->setFaction(memberOfFactionToJoin->faction());
    } else if (!objectToChangeFaction) {
        debug("Script: changeFaction: objectToChangeFaction is invalid", 1, DebugChannels::script);
    } else if (!memberOfFactionToJoin) {
        debug("Script: changeFaction: memberOfFactionToJoin is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::changeToStandardFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto creatureToChange = getCreature(args, 0);
    auto faction = getEnum<Faction>(args, 1);

    if (creatureToChange) {
        creatureToChange->setFaction(faction);
    } else {
        debug("Script: changeToStandardFaction: creatureToChange is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getFactionEqual(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto firstObject = getCreature(args, 0);
    auto secondObject = getCreatureOrCaller(args, 1, ctx);

    if (firstObject && secondObject) {
        result = firstObject->faction() == secondObject->faction();
    } else if (!firstObject) {
        debug("Script: getStandardFaction: firstObject is invalid", 1, DebugChannels::script);
    } else if (!secondObject) {
        debug("Script: getStandardFaction: secondObject is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getStandardFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto result = Faction::Invalid;

    auto object = getCreature(args, 0);
    if (object) {
        result = object->faction();
    } else {
        debug("Script: getStandardFaction: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getIsEnemy(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = Reputes::instance().getIsEnemy(*target, *source);
    } else if (!target) {
        debug("Script: getIsEnemy: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsEnemy: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getIsFriend(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = Reputes::instance().getIsFriend(*target, *source);
    } else if (!target) {
        debug("Script: getIsFriend: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsFriend: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getIsNeutral(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = Reputes::instance().getIsNeutral(*target, *source);
    } else if (!target) {
        debug("Script: getIsNeutral: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsNeutral: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

} // namespace game

} // namespace reone
