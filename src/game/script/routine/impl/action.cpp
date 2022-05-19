/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../declarations.h"

#include "../../../../common/logutil.h"

#include "../../../game.h"
#include "../../../object/creature.h"

#include "../argutil.h"
#include "../context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable actionRandomWalk(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable actionMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);
    bool run = getIntAsBoolOrElse(args, 1, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionMoveAwayFromObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto fleeFrom = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 40.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionEquipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    int inventorySlot = getInt(args, 1);
    bool instant = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionUnequipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    bool instant = getIntAsBoolOrElse(args, 1, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionAttack(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attackee = getObject(args, 0, ctx);
    bool passive = getIntAsBoolOrElse(args, 1, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionPlayAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto animation = getIntAsEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float durationSeconds = getFloatOrElse(args, 2, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionOpenDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionCloseDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionCastSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getIntAsBoolOrElse(args, 3, false);
    int domainLevel = getIntOrElse(args, 4, 0);
    auto projectilePathType = getIntAsEnumOrElse(args, 5, ProjectilePathType::Default);
    bool instantSpell = getIntAsBoolOrElse(args, 6, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionGiveItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionTakeItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto takeFrom = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionJumpToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getIntAsBoolOrElse(args, 1, true);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionWait(const vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionStartConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectToConverse = getObject(args, 0, ctx);
    string dialogResRef = getStringOrElse(args, 1, "");
    bool privateConversation = getIntAsBoolOrElse(args, 2, false);
    auto conversationType = getIntAsEnumOrElse(args, 3, ConversationType::Cinematic);
    bool ignoreStartRange = getIntAsBoolOrElse(args, 4, false);
    string nameObjectToIgnore1 = getStringOrElse(args, 5, "");
    string nameObjectToIgnore2 = getStringOrElse(args, 6, "");
    string nameObjectToIgnore3 = getStringOrElse(args, 7, "");
    string nameObjectToIgnore4 = getStringOrElse(args, 8, "");
    string nameObjectToIgnore5 = getStringOrElse(args, 9, "");
    string nameObjectToIgnore6 = getStringOrElse(args, 10, "");
    bool useLeader = getIntAsBoolOrElse(args, 11, false);
    int barkX = getIntOrElse(args, 12, -1);
    int barkY = getIntOrElse(args, 13, -1);
    bool dontClearAllActions = getIntAsBoolOrElse(args, 14, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionPauseConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable actionResumeConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable actionJumpToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationArgument(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionUseSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    auto skill = getIntAsEnum<SkillType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int subSkill = getIntOrElse(args, 2, 0);
    auto itemUsed = getObjectAsItemOrNull(args, 3, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionDoCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = getAction(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionUseTalentOnObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto chosenTalent = getTalent(args, 0);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionInteractObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto placeable = getObjectAsPlaceable(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionSurrenderToEnemies(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable actionForceMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float timeout = getFloatOrElse(args, 2, 30.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionForceMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);
    float timeout = getFloatOrElse(args, 3, 30.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionEquipMostDamagingMelee(const vector<Variable> &args, const RoutineContext &ctx) {
    auto versus = getObjectOrNull(args, 0, ctx);
    bool offHand = getIntAsBoolOrElse(args, 1, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionEquipMostDamagingRanged(const vector<Variable> &args, const RoutineContext &ctx) {
    auto versus = getObjectOrNull(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionUnlockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionLockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto target = getObject(args, 1, ctx);
    auto projectilePathType = getIntAsEnum<ProjectilePathType>(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto location = getLocationArgument(args, 1);
    auto projectilePathType = getIntAsEnum<ProjectilePathType>(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionBarkString(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionFollowLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

// TSL

Variable actionFollowOwner(const vector<Variable> &args, const RoutineContext &ctx) {
    float range = getFloatOrElse(args, 0, 2.5f);

    // TODO: implement

    return Variable::ofNull();
}

Variable actionSwitchWeapons(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

// END TSL

} // namespace routine

} // namespace game

} // namespace reone
