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
 *  Implementation of action factory routines.
 */

#include "../declarations.h"

#include "../../../../common/logutil.h"
#include "../../../../game/action/factory.h"
#include "../../../../game/game.h"
#include "../../../../game/object/creature.h"

#include "../../../../script/exception/argument.h"

#include "../argutil.h"
#include "../context.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

Variable actionRandomWalk(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newRandomWalk();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    bool run = getBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newMoveToLocation(move(destination), run);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);

    auto action = ctx.game.actionFactory().newMoveToObject(move(moveTo), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveAwayFromObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto fleeFrom = getObject(args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 40.0f);

    auto action = ctx.game.actionFactory().newMoveAwayFromObject(move(fleeFrom), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    int inventorySlot = getInt(args, 1);
    bool instant = getBoolOrElse(args, 2, false);

    auto action = ctx.game.actionFactory().newEquipItem(move(item), inventorySlot, instant);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnequipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    bool instant = getBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newUnequipItem(move(item), instant);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionAttack(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto attackee = getSpatialObject(args, 0, ctx);
    bool passive = getBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newAttack(attackee, caller->getAttackRange(), false, passive);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPlayAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto animation = getEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float durationSeconds = getFloatOrElse(args, 2, 0.0f);

    auto action = ctx.game.actionFactory().newPlayAnimation(animation, speed, durationSeconds);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionOpenDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObject(args, 0, ctx);

    auto action = ctx.game.actionFactory().newOpenDoor(door);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCloseDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObject(args, 0, ctx);

    auto action = ctx.game.actionFactory().newCloseDoor(door);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getEnum<ForcePower>(args, 0);
    auto target = getObject(args, 1, ctx);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getBoolOrElse(args, 3, false);
    int domainLevel = getIntOrElse(args, 4, 0);
    auto projectilePathType = getEnumOrElse(args, 5, ProjectilePathType::Default);
    bool instantSpell = getBoolOrElse(args, 6, false);

    auto action = ctx.game.actionFactory().newCastSpellAtObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionGiveItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newGiveItem();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionTakeItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    auto takeFrom = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newTakeItem();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getBoolOrElse(args, 1, true);

    auto action = ctx.game.actionFactory().newJumpToObject(move(jumpTo));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionWait(const vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);

    auto action = ctx.game.actionFactory().newWait(seconds);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionStartConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: figure out all arguments
    auto objectToConverse = getObject(args, 0, ctx);
    string dialogResRef(getStringOrElse(args, 1, ""));
    bool ignoreStartRange = getBoolOrElse(args, 4, false);

    auto caller = getCaller(ctx);
    if (dialogResRef.empty()) {
        dialogResRef = caller->conversation();
    }
    auto action = ctx.game.actionFactory().newStartConversation(move(objectToConverse), move(dialogResRef), ignoreStartRange);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPauseConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newPauseConversation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionResumeConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newResumeConversation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationEngineType(args, 0);

    auto action = ctx.game.actionFactory().newJumpToLocation(move(location));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto skill = getEnum<SkillType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int subSkill = getIntOrElse(args, 2, 0);
    auto itemUsed = getObject(args, 3, ctx);

    auto action = ctx.game.actionFactory().newUseSkill(target, skill);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionDoCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = getAction(args, 0);

    auto commandAction = ctx.game.actionFactory().newDoCommand(move(action));
    getCaller(ctx)->addAction(move(commandAction));

    return Variable::ofNull();
}

Variable actionUseTalentOnObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto chosenTalen = getTalent(args, 0);
    auto target = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newUseTalentOnObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionInteractObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newInteractObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSurrenderToEnemies(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newSurrenderToEnemies();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto destination = getLocationEngineType(args, 0);
    bool run = getBoolOrElse(args, 1, false);
    float timeout = getFloatOrElse(args, 2, 30.0f);

    auto action = ctx.game.actionFactory().newMoveToLocation(move(destination));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto moveTo = getObject(args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);
    float timeout = getFloatOrElse(args, 3, 30.0f);

    auto action = ctx.game.actionFactory().newMoveToObject(move(moveTo), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingMelee(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newEquipMostDamagingMelee();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingRanged(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newEquipMostDamagingRanged();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnlockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newUnlockObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionLockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newLockObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newCastFakeSpellAtObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newCastFakeSpellAtLocation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionBarkString(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);

    auto action = ctx.game.actionFactory().newBarkString();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionFollowLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newFollowLeader();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

// TSL

Variable actionFollowOwner(const vector<Variable> &args, const RoutineContext &ctx) {
    float range = getFloatOrElse(args, 0, 2.5f);

    auto action = ctx.game.actionFactory().newFollowOwner();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSwitchWeapons(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.game.actionFactory().newSwitchWeapons();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

// END TSL

} // namespace routine

} // namespace kotor

} // namespace reone
