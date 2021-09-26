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
 *  Implementation of action-related routines.
 */

#include "declarations.h"

#include "../../../../common/logutil.h"
#include "../../../../script/exception/argument.h"

#include "../../../action/factory.h"
#include "../../../game.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable actionRandomWalk(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto action = game.actionFactory().newRandomWalk();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    bool run = getBoolOrElse(args, 1, false);

    auto action = game.actionFactory().newMoveToLocation(move(destination), run);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto moveTo = getObject(game, args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);

    auto action = game.actionFactory().newMoveToObject(move(moveTo), run, range);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveAwayFromObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto fleeFrom = getObject(game, args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 40.0f);

    auto action = game.actionFactory().newMoveAwayFromObject(move(fleeFrom), run, range);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    int inventorySlot = getInt(args, 1);
    bool instant = getBoolOrElse(args, 2, false);

    auto action = game.actionFactory().newEquipItem(move(item), inventorySlot, instant);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnequipItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    bool instant = getBoolOrElse(args, 1, false);

    auto action = game.actionFactory().newUnequipItem(move(item), instant);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPickUpItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);

    auto action = game.actionFactory().newPickUpItem(move(item));
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPutDownItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);

    auto action = game.actionFactory().newPutDownItem(move(item));
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionAttack(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    auto attackee = getSpatialObject(game, args, 0, ctx);
    bool passive = getBoolOrElse(args, 1, false);

    auto action = game.actionFactory().newAttack(attackee, caller->getAttackRange(), false, passive);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSpeakString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    string stringToSpeak(getString(args, 0));
    auto talkVolume = getEnumOrElse(args, 1, TalkVolume::Talk);

    auto action = game.actionFactory().newSpeakString(move(stringToSpeak), talkVolume);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPlayAnimation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto animation = getEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float durationSeconds = getFloatOrElse(args, 2, 0.0f);

    auto action = game.actionFactory().newPlayAnimation(animation, speed, durationSeconds);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionOpenDoor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto door = getObject(game, args, 0, ctx);

    auto action = game.actionFactory().newOpenDoor(door);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCloseDoor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto door = getObject(game, args, 0, ctx);

    auto action = game.actionFactory().newCloseDoor(door);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastSpellAtObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto spell = getEnum<ForcePower>(args, 0);
    auto target = getObject(game, args, 1, ctx);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getBoolOrElse(args, 3, false);
    int domainLevel = getIntOrElse(args, 4, 0);
    auto projectilePathType = getEnumOrElse(args, 5, ProjectilePathType::Default);
    bool instantSpell = getBoolOrElse(args, 6, false);

    auto action = game.actionFactory().newCastSpellAtObject();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionGiveItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    auto giveTo = getObject(game, args, 1, ctx);

    auto action = game.actionFactory().newGiveItem();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionTakeItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    auto takeFrom = getObject(game, args, 1, ctx);

    auto action = game.actionFactory().newTakeItem();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceFollowObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto follow = getObject(game, args, 0, ctx);
    float followDistance = getFloatOrElse(args, 1, 0.0f);

    auto action = game.actionFactory().newForceFollowObject();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(game, args, 0, ctx);
    bool walkStraightLine = getBoolOrElse(args, 1, true);

    auto action = game.actionFactory().newJumpToObject(move(jumpTo));
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionWait(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float seconds = getFloat(args, 0);

    auto action = game.actionFactory().newWait(seconds);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionStartConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: figure out all arguments
    auto objectToConverse = getObject(game, args, 0, ctx);
    string dialogResRef(getStringOrElse(args, 1, ""));
    bool ignoreStartRange = getBoolOrElse(args, 4, false);

    auto caller = getCaller(game, ctx);
    if (dialogResRef.empty()) {
        dialogResRef = caller->conversation();
    }
    auto action = game.actionFactory().newStartConversation(move(objectToConverse), move(dialogResRef), ignoreStartRange);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPauseConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto action = game.actionFactory().newPauseConversation();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionResumeConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto action = game.actionFactory().newResumeConversation();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);

    auto action = game.actionFactory().newJumpToLocation(move(location));
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastSpellAtLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto spell = getEnum<ForcePower>(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getBoolOrElse(args, 3, false);
    auto projectilePathType = getEnumOrElse(args, 4, ProjectilePathType::Default);
    bool instantSpell = getBoolOrElse(args, 5, false);

    auto action = game.actionFactory().newCastSpellAtLocation();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSpeakStringByStrRef(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    auto talkVolume = getEnumOrElse(args, 1, TalkVolume::Talk);

    auto action = game.actionFactory().newSpeakStringByStrRef();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseFeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto feat = getEnum<FeatType>(args, 0);
    auto target = getObject(game, args, 1, ctx);

    auto action = game.actionFactory().newUseFeat(target, feat);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseSkill(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto skill = getEnum<SkillType>(args, 0);
    auto target = getObject(game, args, 1, ctx);
    int subSkill = getIntOrElse(args, 2, 0);
    auto itemUsed = getObject(game, args, 3, ctx);

    auto action = game.actionFactory().newUseSkill(target, skill);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionDoCommand(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto action = getAction(args, 0);

    auto commandAction = game.actionFactory().newDoCommand(move(action));
    getCaller(game, ctx)->addAction(move(commandAction));

    return Variable::ofNull();
}

Variable actionUseTalentOnObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto chosenTalen = getTalent(args, 0);
    auto target = getObject(game, args, 1, ctx);

    auto action = game.actionFactory().newUseTalentOnObject();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseTalentAtLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto chosenTalen = getTalent(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);

    auto action = game.actionFactory().newUseTalentAtLocation();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionInteractObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newInteractObject();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveAwayFromLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newMoveAwayFromLocation();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSurrenderToEnemies(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newSurrenderToEnemies();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto destination = getLocationEngineType(args, 0);
    bool run = getBoolOrElse(args, 1, false);
    float timeout = getFloatOrElse(args, 2, 30.0f);

    auto action = game.actionFactory().newMoveToLocation(move(destination));
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto moveTo = getObject(game, args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);
    float timeout = getFloatOrElse(args, 3, 30.0f);

    auto action = game.actionFactory().newMoveToObject(move(moveTo), run, range);
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingMelee(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newEquipMostDamagingMelee();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingRanged(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newEquipMostDamagingRanged();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostEffectiveArmor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newEquipMostEffectiveArmor();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnlockObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newUnlockObject();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionLockObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newLockObject();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newCastFakeSpellAtObject();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newCastFakeSpellAtLocation();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionBarkString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);

    auto action = game.actionFactory().newBarkString();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionFollowLeader(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto action = game.actionFactory().newFollowLeader();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionFollowOwner(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float range = getFloatOrElse(args, 0, 2.5f);

    auto action = game.actionFactory().newFollowOwner();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSwitchWeapons(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: arguments

    auto action = game.actionFactory().newSwitchWeapons();
    getCaller(game, ctx)->addAction(move(action));

    return Variable::ofNull();
}

} // namespace routine

} // namespace game

} // namespace reone
