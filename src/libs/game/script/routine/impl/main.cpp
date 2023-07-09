/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/event.h"
#include "reone/game/game.h"
#include "reone/game/script/routine/argutil.h"
#include "reone/game/script/routine/context.h"
#include "reone/game/script/routine/objectutil.h"
#include "reone/game/script/routines.h"
#include "reone/script/executioncontext.h"
#include "reone/script/routine/exception/argument.h"
#include "reone/script/routine/exception/notimplemented.h"
#include "reone/script/variable.h"
#include "reone/system/logutil.h"
#include "reone/system/randomutil.h"

#define R_VOID script::VariableType::Void
#define R_INT script::VariableType::Int
#define R_FLOAT script::VariableType::Float
#define R_OBJECT script::VariableType::Object
#define R_STRING script::VariableType::String
#define R_EFFECT script::VariableType::Effect
#define R_EVENT script::VariableType::Event
#define R_LOCATION script::VariableType::Location
#define R_TALENT script::VariableType::Talent
#define R_VECTOR script::VariableType::Vector
#define R_ACTION script::VariableType::Action

#ifdef _DEBUG
static constexpr bool kShipBuild = false;
#else
static constexpr bool kShipBuild = true;
#endif

using namespace reone::script;
using namespace reone::scene;

namespace reone {

namespace game {

static Variable Random(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nMaxInteger = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(randomInt(0, nMaxInteger - 1));
}

static Variable PrintString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    info(sString);
    return Variable::ofNull();
}

static Variable PrintFloat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFloat = getFloat(args, 0);
    auto nWidth = getIntOrElse(args, 1, 18);
    auto nDecimals = getIntOrElse(args, 2, 9);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintFloat");
}

static Variable FloatToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFloat = getFloat(args, 0);
    auto nWidth = getIntOrElse(args, 1, 18);
    auto nDecimals = getIntOrElse(args, 2, 9);

    // Transform

    // Execute
    return Variable::ofString(std::to_string(fFloat));
}

static Variable PrintInteger(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintInteger");
}

static Variable PrintObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintObject");
}

static Variable AssignCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oActionSubject = getObject(args, 0, ctx);
    auto aActionToAssign = getAction(args, 1);

    // Transform

    // Execute
    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(aActionToAssign));
    oActionSubject->addAction(std::move(commandAction));
    return Variable::ofNull();
}

static Variable DelayCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fSeconds = getFloat(args, 0);
    auto aActionToDelay = getAction(args, 1);

    // Transform

    // Execute
    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(aActionToDelay));
    getCaller(ctx)->delayAction(std::move(commandAction), fSeconds);
    return Variable::ofNull();
}

static Variable ExecuteScript(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sScript = getString(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nScriptVar = getIntOrElse(args, 2, -1);

    // Transform

    // Execute
    ctx.game.scriptRunner().run(sScript, oTarget->id(), kObjectInvalid, kObjectInvalid, nScriptVar);
    return Variable::ofNull();
}

static Variable ClearAllActions(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    getCaller(ctx)->clearAllActions();
    return Variable::ofNull();
}

static Variable SetFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fDirection = getFloat(args, 0);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    caller->setFacing(glm::radians(fDirection));
    return Variable::ofNull();
}

static Variable SwitchPlayerCharacter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SwitchPlayerCharacter");
}

static Variable SetTime(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nHour = getInt(args, 0);
    auto nMinute = getInt(args, 1);
    auto nSecond = getInt(args, 2);
    auto nMillisecond = getInt(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTime");
}

static Variable SetPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    ctx.game.party().setPartyLeader(nNPC);
    return Variable::ofNull();
}

static Variable SetAreaUnescapable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bUnescapable = getInt(args, 0);

    // Transform
    auto unescapable = static_cast<bool>(bUnescapable);

    // Execute
    ctx.game.module()->area()->setUnescapable(unescapable);
    return Variable::ofNull();
}

static Variable GetAreaUnescapable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    bool unescapable = ctx.game.module()->area()->isUnescapable();
    return Variable::ofInt(static_cast<int>(unescapable));
}

static Variable GetTimeHour(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeHour");
}

static Variable GetTimeMinute(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeMinute");
}

static Variable GetTimeSecond(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeSecond");
}

static Variable GetTimeMillisecond(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeMillisecond");
}

static Variable GetArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto area = ctx.game.module()->area();
    return Variable::ofObject(getObjectIdOrInvalid(area));
}

static Variable GetEnteringObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetExitingObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofVector(oTarget->position());
}

static Variable GetFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    float facing = glm::degrees(oTarget->getFacing());
    return Variable::ofFloat(facing);
}

static Variable GetItemPossessor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetItemPossessor");
}

static Variable GetItemPossessedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto sItemTag = getString(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);
    auto itemTag = boost::to_lower_copy(sItemTag);

    // Execute
    if (itemTag.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }
    auto item = creature->getItemByTag(itemTag);
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable CreateItemOnObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sItemTemplate = getString(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);
    auto nStackSize = getIntOrElse(args, 2, 1);
    auto nHideMessage = getIntOrElse(args, 3, 0);

    // Transform
    auto itemTemplate = boost::to_lower_copy(sItemTemplate);

    // Execute
    if (itemTemplate.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }
    auto item = oTarget->addItem(itemTemplate, nStackSize, true);
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable GetLastAttacker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttackee = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttacker");
}

static Variable GetNearestCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFirstCriteriaType = getInt(args, 0);
    auto nFirstCriteriaValue = getInt(args, 1);
    auto oTarget = getObjectOrCaller(args, 2, ctx);
    auto nNth = getIntOrElse(args, 3, 1);
    auto nSecondCriteriaType = getIntOrElse(args, 4, -1);
    auto nSecondCriteriaValue = getIntOrElse(args, 5, -1);
    auto nThirdCriteriaType = getIntOrElse(args, 6, -1);
    auto nThirdCriteriaValue = getIntOrElse(args, 7, -1);

    // Transform
    auto firstCriteriaType = static_cast<CreatureType>(nFirstCriteriaType);
    auto secondCriteriaType = static_cast<CreatureType>(nSecondCriteriaType);
    auto thirdCriteriaType = static_cast<CreatureType>(nThirdCriteriaType);

    // Execute
    Area::SearchCriteriaList criterias;
    criterias.push_back(std::make_pair(firstCriteriaType, nFirstCriteriaValue));
    if (secondCriteriaType != CreatureType::Invalid) {
        criterias.push_back(std::make_pair(secondCriteriaType, nSecondCriteriaValue));
    }
    if (thirdCriteriaType != CreatureType::Invalid) {
        criterias.push_back(std::make_pair(thirdCriteriaType, nThirdCriteriaValue));
    }
    auto creature = ctx.game.module()->area()->getNearestCreature(oTarget, criterias, nNth - 1);
    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

static Variable GetDistanceToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    return Variable::ofFloat(caller->getDistanceTo(*oObject));
}

static Variable GetIsObjectValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool valid;
    try {
        auto oObject = getObject(args, 0, ctx);
        valid = static_cast<bool>(oObject);
    } catch (const RoutineArgumentException &ignored) {
        valid = false;
    }
    return Variable::ofInt(static_cast<bool>(valid));
}

static Variable SetCameraFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fDirection = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCameraFacing");
}

static Variable PlaySound(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sSoundName = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlaySound");
}

static Variable GetSpellTargetObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellTargetObject");
}

static Variable GetCurrentHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    int hitPoints = oObject->currentHitPoints();
    return Variable::ofInt(hitPoints);
}

static Variable GetMaxHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    int hitPoints = oObject->maxHitPoints();
    return Variable::ofInt(hitPoints);
}

static Variable GetLastItemEquipped(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastItemEquipped");
}

static Variable GetSubScreenID(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSubScreenID");
}

static Variable CancelCombat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CancelCombat");
}

static Variable GetCurrentForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCurrentForcePoints");
}

static Variable GetMaxForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetMaxForcePoints");
}

static Variable PauseGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bPause = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PauseGame");
}

static Variable SetPlayerRestrictMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bRestrict = getInt(args, 0);

    // Transform
    auto restrict = static_cast<bool>(bRestrict);

    // Execute
    ctx.game.module()->player().setRestrictMode(restrict);
    return Variable::ofNull();
}

static Variable GetStringLength(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(sString.length()));
}

static Variable GetStringUpperCase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetStringUpperCase");
}

static Variable GetStringLowerCase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetStringLowerCase");
}

static Variable GetStringRight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nCount = getInt(args, 1);

    // Transform

    // Execute
    std::string right;
    if (sString.size() >= nCount) {
        right = sString.substr(sString.length() - nCount, nCount);
    }
    return Variable::ofString(std::move(right));
}

static Variable GetStringLeft(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nCount = getInt(args, 1);

    // Transform

    // Execute
    std::string left;
    if (sString.size() >= nCount) {
        left = sString.substr(0, nCount);
    }
    return Variable::ofString(std::move(left));
}

static Variable InsertString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sDestination = getString(args, 0);
    auto sString = getString(args, 1);
    auto nPosition = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("InsertString");
}

static Variable GetSubString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nStart = getInt(args, 1);
    auto nCount = getInt(args, 2);

    // Transform

    // Execute
    return Variable::ofString(sString.substr(nStart, nStart));
}

static Variable FindSubString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto sSubString = getString(args, 1);

    // Transform

    // Execute
    size_t pos = sString.find(sSubString);
    return Variable::ofInt(pos != std::string::npos ? static_cast<int>(pos) : -1);
}

static Variable fabs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("fabs");
}

static Variable cos(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("cos");
}

static Variable sin(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("sin");
}

static Variable tan(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("tan");
}

static Variable acos(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("acos");
}

static Variable asin(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("asin");
}

static Variable atan(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("atan");
}

static Variable log(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("log");
}

static Variable pow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);
    auto fExponent = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("pow");
}

static Variable sqrt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("sqrt");
}

static Variable abs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nValue = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(std::abs(nValue));
}

static Variable GetPlayerRestrictMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    bool restrict = ctx.game.module()->player().isRestrictMode();
    return Variable::ofInt(static_cast<int>(restrict));
}

static Variable GetCasterLevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCasterLevel");
}

static Variable GetFirstEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofEffect(creature->getFirstEffect());
}

static Variable GetNextEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofEffect(creature->getNextEffect());
}

static Variable RemoveEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto eEffect = getEffect(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveEffect");
}

static Variable GetIsEffectValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool valid;
    try {
        auto eEffect = getEffect(args, 0);
        valid = static_cast<bool>(eEffect);
    } catch (const RoutineArgumentException &ignored) {
        valid = false;
    }
    return Variable::ofInt(static_cast<int>(valid));
}

static Variable GetEffectDurationType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectDurationType");
}

static Variable GetEffectSubType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectSubType");
}

static Variable GetEffectCreator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectCreator");
}

static Variable IntToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofString(std::to_string(nInteger));
}

static Variable GetFirstObjectInArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObjectOrNull(args, 0, ctx);
    auto nObjectFilter = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstObjectInArea");
}

static Variable GetNextObjectInArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObjectOrNull(args, 0, ctx);
    auto nObjectFilter = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextObjectInArea");
}

static Variable d2(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 2);
    }
    return Variable::ofInt(total);
}

static Variable d3(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 3);
    }
    return Variable::ofInt(total);
}

static Variable d4(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 4);
    }
    return Variable::ofInt(total);
}

static Variable d6(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 6);
    }
    return Variable::ofInt(total);
}

static Variable d8(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 8);
    }
    return Variable::ofInt(total);
}

static Variable d10(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 10);
    }
    return Variable::ofInt(total);
}

static Variable d12(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 12);
    }
    return Variable::ofInt(total);
}

static Variable d20(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 20);
    }
    return Variable::ofInt(total);
}

static Variable d100(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::randomInt(1, 100);
    }
    return Variable::ofInt(total);
}

static Variable VectorMagnitude(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VectorMagnitude");
}

static Variable GetMetaMagicFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetMetaMagicFeat");
}

static Variable GetObjectType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oTarget->type()));
}

static Variable GetRacialType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->racialType()));
}

static Variable FortitudeSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nDC = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);
    auto oSaveVersus = getObjectOrCaller(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FortitudeSave");
}

static Variable ReflexSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nDC = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);
    auto oSaveVersus = getObjectOrCaller(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ReflexSave");
}

static Variable WillSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nDC = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);
    auto oSaveVersus = getObjectOrCaller(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("WillSave");
}

static Variable GetSpellSaveDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellSaveDC");
}

static Variable MagicalEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MagicalEffect");
}

static Variable SupernaturalEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SupernaturalEffect");
}

static Variable ExtraordinaryEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ExtraordinaryEffect");
}

static Variable GetAC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nForFutureUse = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAC");
}

static Variable RoundsToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nRounds = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofFloat(nRounds / 6.0f);
}

static Variable HoursToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nHours = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(nHours * 3600);
}

static Variable TurnsToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nTurns = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TurnsToSeconds");
}

static Variable SoundObjectSetFixedVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto fFixedVariance = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetFixedVariance");
}

static Variable GetGoodEvilValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetGoodEvilValue");
}

static Variable GetPartyMemberCount(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(ctx.game.party().getSize());
}

static Variable GetAlignmentGoodEvil(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAlignmentGoodEvil");
}

static Variable GetFirstObjectInShape(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nShape = getInt(args, 0);
    auto fSize = getFloat(args, 1);
    auto lTarget = getLocationArgument(args, 2);
    auto bLineOfSight = getIntOrElse(args, 3, 0);
    auto nObjectFilter = getIntOrElse(args, 4, 1);
    auto vOrigin = getVectorOrElse(args, 5, glm::vec3(0.0f, 0.0f, 0.0f));

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstObjectInShape");
}

static Variable GetNextObjectInShape(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nShape = getInt(args, 0);
    auto fSize = getFloat(args, 1);
    auto lTarget = getLocationArgument(args, 2);
    auto bLineOfSight = getIntOrElse(args, 3, 0);
    auto nObjectFilter = getIntOrElse(args, 4, 1);
    auto vOrigin = getVectorOrElse(args, 5, glm::vec3(0.0f, 0.0f, 0.0f));

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextObjectInShape");
}

static Variable SignalEvent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto evToRun = getEvent(args, 1);

    // Transform

    // Execute
    debug(boost::format("Event signalled: %s %s") % oObject->tag() % evToRun->number(), LogChannel::Script);
    ctx.game.scriptRunner().run(oObject->getOnUserDefined(), oObject->id(), kObjectInvalid, evToRun->number());
    return Variable::ofNull();
}

static Variable EventUserDefined(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nUserDefinedEventNumber = getInt(args, 0);

    // Transform

    // Execute
    auto event = std::make_shared<Event>(nUserDefinedEventNumber);
    return Variable::ofEvent(std::move(event));
}

static Variable VectorNormalize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);

    // Transform

    // Execute
    return Variable::ofVector(glm::normalize(vVector));
}

static Variable GetItemStackSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    return Variable::ofInt(item->stackSize());
}

static Variable GetAbilityScore(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nAbilityType = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);
    auto ability = static_cast<Ability>(nAbilityType);

    // Execute
    return Variable::ofInt(creature->attributes().getAbilityScore(ability));
}

static Variable GetIsDead(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->isDead()));
}

static Variable PrintVector(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);
    auto bPrepend = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintVector");
}

static Variable Vector(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto x = getFloatOrElse(args, 0, 0.0f);
    auto y = getFloatOrElse(args, 1, 0.0f);
    auto z = getFloatOrElse(args, 2, 0.0f);

    // Transform

    // Execute
    return Variable::ofVector(glm::vec3(x, y, z));
}

static Variable SetFacingPoint(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vTarget = getVector(args, 0);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    caller->face(vTarget);
    return Variable::ofNull();
}

static Variable AngleToVector(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fAngle = getFloat(args, 0);

    // Transform

    // Execute
    auto vector = glm::vec3(glm::cos(fAngle), glm::sin(fAngle), 0.0f);
    return Variable::ofVector(std::move(vector));
}

static Variable VectorToAngle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VectorToAngle");
}

static Variable TouchAttackMelee(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto bDisplayFeedback = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TouchAttackMelee");
}

static Variable TouchAttackRanged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto bDisplayFeedback = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TouchAttackRanged");
}

static Variable SetItemStackSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto nStackSize = getInt(args, 1);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    item->setStackSize(nStackSize);
    return Variable::ofNull();
}

static Variable GetDistanceBetween(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectA = getObject(args, 0, ctx);
    auto oObjectB = getObject(args, 1, ctx);

    // Transform

    // Execute
    return Variable::ofFloat(oObjectA->getDistanceTo(*oObjectB));
}

static Variable SetReturnStrref(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bShow = getInt(args, 0);
    auto srStringRef = getIntOrElse(args, 1, 0);
    auto srReturnQueryStrRef = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetReturnStrref");
}

static Variable GetItemInSlot(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInventorySlot = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    auto item = creature->getEquippedItem(nInventorySlot);
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable SetGlobalString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto sValue = getString(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalString(sIdentifier, sValue);
    return Variable::ofNull();
}

static Variable SetCommandable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bCommandable = getInt(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);

    // Transform
    bool commandable = static_cast<bool>(bCommandable);

    // Execute
    oTarget->setCommandable(commandable);
    return Variable::ofNull();
}

static Variable GetCommandable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oTarget->isCommandable()));
}

static Variable GetHitDice(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(creature->attributes().getAggregateLevel());
}

static Variable GetTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofString(oObject->tag());
}

static Variable ResistForce(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSource = getObject(args, 0, ctx);
    auto oTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ResistForce");
}

static Variable GetEffectType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(eEffect->type()));
}

static Variable GetFactionEqual(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFirstObject = getObject(args, 0, ctx);
    auto oSecondObject = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto firstObject = checkCreature(oFirstObject);
    auto secondObject = checkCreature(oSecondObject);

    // Execute
    return Variable::ofInt(static_cast<int>(firstObject->faction() == secondObject->faction()));
}

static Variable ChangeFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectToChangeFaction = getObject(args, 0, ctx);
    auto oMemberOfFactionToJoin = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeFaction");
}

static Variable GetIsListening(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsListening");
}

static Variable SetListening(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto bValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetListening");
}

static Variable SetListenPattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto sPattern = getString(args, 1);
    auto nNumber = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetListenPattern");
}

static Variable TestStringAgainstPattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sPattern = getString(args, 0);
    auto sStringToTest = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TestStringAgainstPattern");
}

static Variable GetMatchedSubstring(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nString = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetMatchedSubstring");
}

static Variable GetMatchedSubstringsCount(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetMatchedSubstringsCount");
}

static Variable GetFactionWeakestMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionWeakestMember");
}

static Variable GetFactionStrongestMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionStrongestMember");
}

static Variable GetFactionMostDamagedMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionMostDamagedMember");
}

static Variable GetFactionLeastDamagedMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionLeastDamagedMember");
}

static Variable GetFactionGold(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionGold");
}

static Variable GetFactionAverageReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSourceFactionMember = getObject(args, 0, ctx);
    auto oTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageReputation");
}

static Variable GetFactionAverageGoodEvilAlignment(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageGoodEvilAlignment");
}

static Variable SoundObjectGetFixedVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectGetFixedVariance");
}

static Variable GetFactionAverageLevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageLevel");
}

static Variable GetFactionAverageXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageXP");
}

static Variable GetFactionMostFrequentClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionMostFrequentClass");
}

static Variable GetFactionWorstAC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionWorstAC");
}

static Variable GetFactionBestAC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionBestAC");
}

static Variable GetGlobalString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    return Variable::ofString(ctx.game.getGlobalString(sIdentifier));
}

static Variable GetListenPatternNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetListenPatternNumber");
}

static Variable GetWaypointByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sWaypointTag = getString(args, 0);

    // Transform
    auto waypointTag = boost::to_lower_copy(sWaypointTag);

    // Execute
    std::shared_ptr<Object> waypoint;
    for (auto &object : ctx.game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (object->tag() == waypointTag) {
            waypoint = object;
            break;
        }
    }
    return Variable::ofObject(getObjectIdOrInvalid(waypoint));
}

static Variable GetTransitionTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTransition = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTransitionTarget");
}

static Variable GetObjectByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTag = getString(args, 0);
    auto nNth = getIntOrElse(args, 1, 0);

    // Transform
    auto tag = boost::to_lower_copy(sTag);

    // Execute
    std::shared_ptr<Object> object;
    if (!tag.empty()) {
        object = ctx.game.module()->area()->getObjectByTag(tag, nNth);
    } else {
        object = ctx.game.party().player();
    }
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable AdjustAlignment(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSubject = getObject(args, 0, ctx);
    auto nAlignment = getInt(args, 1);
    auto nShift = getInt(args, 2);
    auto bDontModifyNPCs = getIntOrElse(args, 3, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustAlignment");
}

static Variable SetAreaTransitionBMP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPredefinedAreaTransition = getInt(args, 0);
    auto sCustomAreaTransitionBMP = getStringOrElse(args, 1, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAreaTransitionBMP");
}

static Variable GetReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSource = getObject(args, 0, ctx);
    auto oTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetReputation");
}

static Variable AdjustReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSourceFactionMember = getObject(args, 1, ctx);
    auto nAdjustment = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustReputation");
}

static Variable GetModuleFileName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleFileName");
}

static Variable GetGoingToBeAttackedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetGoingToBeAttackedBy");
}

static Variable GetLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto location = std::make_shared<game::Location>(oObject->position(), oObject->getFacing());
    return Variable::ofLocation(std::move(location));
}

static Variable CreateLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vPosition = getVector(args, 0);
    auto fOrientation = getFloat(args, 1);

    // Transform
    auto orientation = glm::radians(fOrientation);

    // Execute
    auto location = std::make_shared<game::Location>(std::move(vPosition), orientation);
    return Variable::ofLocation(location);
}

static Variable ApplyEffectAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDurationType = getInt(args, 0);
    auto eEffect = getEffect(args, 1);
    auto lLocation = getLocationArgument(args, 2);
    auto fDuration = getFloatOrElse(args, 3, 0.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ApplyEffectAtLocation");
}

static Variable GetIsPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    bool pc = creature == ctx.game.party().player();
    return Variable::ofInt(static_cast<int>(pc));
}

static Variable FeetToMeters(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFeet = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FeetToMeters");
}

static Variable YardsToMeters(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fYards = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("YardsToMeters");
}

static Variable ApplyEffectToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDurationType = getInt(args, 0);
    auto eEffect = getEffect(args, 1);
    auto oTarget = getObject(args, 2, ctx);
    auto fDuration = getFloatOrElse(args, 3, 0.0f);

    // Transform
    auto durationType = static_cast<DurationType>(nDurationType);

    // Execute
    oTarget->applyEffect(eEffect, durationType, fDuration);
    return Variable::ofNull();
}

static Variable SpeakString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sStringToSpeak = getString(args, 0);
    auto nTalkVolume = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpeakString");
}

static Variable GetSpellTargetLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellTargetLocation");
}

static Variable GetPositionFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocation = getLocationArgument(args, 0);

    // Transform

    // Execute
    return Variable::ofVector(lLocation->position());
}

static Variable GetFacingFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocation = getLocationArgument(args, 0);

    // Transform

    // Execute
    return Variable::ofFloat(glm::degrees(lLocation->facing()));
}

static Variable GetNearestCreatureToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFirstCriteriaType = getInt(args, 0);
    auto nFirstCriteriaValue = getInt(args, 1);
    auto lLocation = getLocationArgument(args, 2);
    auto nNth = getIntOrElse(args, 3, 1);
    auto nSecondCriteriaType = getIntOrElse(args, 4, -1);
    auto nSecondCriteriaValue = getIntOrElse(args, 5, -1);
    auto nThirdCriteriaType = getIntOrElse(args, 6, -1);
    auto nThirdCriteriaValue = getIntOrElse(args, 7, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNearestCreatureToLocation");
}

static Variable GetNearestObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nObjectType = getIntOrElse(args, 0, 32767);
    auto oTarget = getObjectOrCaller(args, 1, ctx);
    auto nNth = getIntOrElse(args, 2, 1);

    // Transform
    auto objectType = static_cast<ObjectType>(nObjectType);

    // Execute
    auto object = ctx.game.module()->area()->getNearestObject(oTarget->position(), nNth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    });
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable GetNearestObjectToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nObjectType = getInt(args, 0);
    auto lLocation = getLocationArgument(args, 1);
    auto nNth = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNearestObjectToLocation");
}

static Variable GetNearestObjectByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTag = getString(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);
    auto nNth = getIntOrElse(args, 2, 1);

    // Transform
    auto tag = boost::to_lower_copy(sTag);

    // Execute
    auto object = ctx.game.module()->area()->getNearestObject(oTarget->position(), nNth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    });
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable IntToFloat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofFloat(static_cast<float>(nInteger));
}

static Variable FloatToInt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFloat = getFloat(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(fFloat));
}

static Variable StringToInt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sNumber = getString(args, 0);

    // Transform

    // Execute
    int intValue = 0;
    if (!sNumber.empty()) {
        intValue = stoi(sNumber);
    }
    return Variable::ofInt(intValue);
}

static Variable StringToFloat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sNumber = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("StringToFloat");
}

static Variable GetIsEnemy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto target = checkCreature(oTarget);
    auto source = checkCreature(oSource);

    // Execute
    bool enemy = ctx.services.game.reputes.getIsEnemy(*target, *source);
    return Variable::ofInt(static_cast<int>(enemy));
}

static Variable GetIsFriend(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto target = checkCreature(oTarget);
    auto source = checkCreature(oSource);

    // Execute
    bool isFriend = ctx.services.game.reputes.getIsFriend(*target, *source);
    return Variable::ofInt(static_cast<int>(isFriend));
}

static Variable GetIsNeutral(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto target = checkCreature(oTarget);
    auto source = checkCreature(oSource);

    // Execute
    bool neutral = ctx.services.game.reputes.getIsNeutral(*target, *source);
    return Variable::ofInt(static_cast<int>(neutral));
}

static Variable GetPCSpeaker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

static Variable GetStringByStrRef(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRef = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofString(ctx.services.resource.strings.get(nStrRef));
}

static Variable DestroyObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oDestroy = getObject(args, 0, ctx);
    auto fDelay = getFloatOrElse(args, 1, 0.0f);
    auto bNoFade = getIntOrElse(args, 2, 0);
    auto fDelayUntilFade = getFloatOrElse(args, 3, 0.0f);
    auto nHideFeedback = getIntOrElse(args, 4, 0);

    // Transform

    // Execute
    ctx.game.module()->area()->destroyObject(*oDestroy);
    return Variable::ofNull();
}

static Variable GetModule(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofObject(getObjectIdOrInvalid(ctx.game.module()));
}

static Variable CreateObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nObjectType = getInt(args, 0);
    auto sTemplate = getString(args, 1);
    auto lLocation = getLocationArgument(args, 2);
    auto bUseAppearAnimation = getIntOrElse(args, 3, 0);

    // Transform
    auto objectType = static_cast<ObjectType>(nObjectType);
    auto tmplt = boost::to_lower_copy(sTemplate);

    // Execute
    auto object = ctx.game.module()->area()->createObject(objectType, tmplt, lLocation);
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable EventSpellCastAt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCaster = getObject(args, 0, ctx);
    auto nSpell = getInt(args, 1);
    auto bHarmful = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EventSpellCastAt");
}

static Variable GetLastSpellCaster(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpellCaster");
}

static Variable GetLastSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpell");
}

static Variable GetUserDefinedEventNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(ctx.execution.userDefinedEventNumber);
}

static Variable GetSpellId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellId");
}

static Variable RandomName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("RandomName");
}

static Variable GetLoadFromSaveGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLoadFromSaveGame");
}

static Variable GetName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofString(oObject->name());
}

static Variable GetLastSpeaker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpeaker");
}

static Variable BeginConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sResRef = getStringOrElse(args, 0, "");
    auto oObjectToDialog = getObjectOrNull(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("BeginConversation");
}

static Variable GetLastPerceived(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    auto perceived = caller->perception().lastPerceived;
    return Variable::ofObject(getObjectIdOrInvalid(perceived));
}

static Variable GetLastPerceptionHeard(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool heard = caller->perception().lastPerception == PerceptionType::Heard;
    return Variable::ofInt(static_cast<int>(heard));
}

static Variable GetLastPerceptionInaudible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool inaudible = caller->perception().lastPerception == PerceptionType::NotHeard;
    return Variable::ofInt(static_cast<int>(inaudible));
}

static Variable GetLastPerceptionSeen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool seen = caller->perception().lastPerception == PerceptionType::Seen;
    return Variable::ofInt(static_cast<int>(seen));
}

static Variable GetLastClosedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetLastPerceptionVanished(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool vanished = caller->perception().lastPerception == PerceptionType::NotSeen;
    return Variable::ofInt(static_cast<int>(vanished));
}

static Variable GetFirstInPersistentObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPersistentObject = getObjectOrCaller(args, 0, ctx);
    auto nResidentObjectType = getIntOrElse(args, 1, 1);
    auto nPersistentZone = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstInPersistentObject");
}

static Variable GetNextInPersistentObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPersistentObject = getObjectOrCaller(args, 0, ctx);
    auto nResidentObjectType = getIntOrElse(args, 1, 1);
    auto nPersistentZone = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextInPersistentObject");
}

static Variable GetAreaOfEffectCreator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAreaOfEffectObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAreaOfEffectCreator");
}

static Variable ShowLevelUpGUI(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ShowLevelUpGUI");
}

static Variable SetItemNonEquippable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto bNonEquippable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetItemNonEquippable");
}

static Variable GetButtonMashCheck(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetButtonMashCheck");
}

static Variable SetButtonMashCheck(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCheck = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetButtonMashCheck");
}

static Variable GiveItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto oGiveTo = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GiveItem");
}

static Variable ObjectToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofString(str(boost::format("%x") % oObject->id()));
}

static Variable GetIsImmune(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nImmunityType = getInt(args, 1);
    auto oVersus = getObjectOrNull(args, 2, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsImmune");
}

static Variable GetEncounterActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterActive");
}

static Variable SetEncounterActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewValue = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterActive");
}

static Variable GetEncounterSpawnsMax(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterSpawnsMax");
}

static Variable SetEncounterSpawnsMax(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewValue = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterSpawnsMax");
}

static Variable GetEncounterSpawnsCurrent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterSpawnsCurrent");
}

static Variable SetEncounterSpawnsCurrent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewValue = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterSpawnsCurrent");
}

static Variable GetModuleItemAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemAcquired");
}

static Variable GetModuleItemAcquiredFrom(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemAcquiredFrom");
}

static Variable SetCustomToken(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCustomTokenNumber = getInt(args, 0);
    auto sTokenValue = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCustomToken");
}

static Variable GetHasFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto feat = static_cast<FeatType>(nFeat);
    auto creature = checkCreature(oCreature);

    // Execute
    bool hasFeat = creature->attributes().hasFeat(feat);
    return Variable::ofInt(static_cast<int>(hasFeat));
}

static Variable GetHasSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto skill = static_cast<SkillType>(nSkill);
    auto creature = checkCreature(oCreature);

    // Execute
    bool hasSkill = creature->attributes().hasSkill(skill);
    return Variable::ofInt(static_cast<int>(hasSkill));
}

static Variable GetObjectSeen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto source = checkCreature(oSource);

    // Execute
    bool seen = source->perception().seen.count(oTarget) > 0;
    return Variable::ofInt(static_cast<int>(seen));
}

static Variable GetObjectHeard(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetObjectHeard");
}

static Variable GetLastPlayerDied(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastPlayerDied");
}

static Variable GetModuleItemLost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemLost");
}

static Variable GetModuleItemLostBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemLostBy");
}

static Variable EventConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("EventConversation");
}

static Variable SetEncounterDifficulty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nEncounterDifficulty = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterDifficulty");
}

static Variable GetEncounterDifficulty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterDifficulty");
}

static Variable GetDistanceBetweenLocations(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocationA = getLocationArgument(args, 0);
    auto lLocationB = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetDistanceBetweenLocations");
}

static Variable GetReflexAdjustedDamage(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamage = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nDC = getInt(args, 2);
    auto nSaveType = getIntOrElse(args, 3, 0);
    auto oSaveVersus = getObjectOrCaller(args, 4, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetReflexAdjustedDamage");
}

static Variable PlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAnimation = getInt(args, 0);
    auto fSpeed = getFloatOrElse(args, 1, 1.0f);
    auto fSeconds = getFloatOrElse(args, 2, 0.0f);

    // Transform
    auto animation = static_cast<AnimationType>(nAnimation);

    // Execute
    AnimationProperties properties;
    properties.speed = fSpeed;
    auto caller = getCaller(ctx);
    caller->playAnimation(animation, std::move(properties));
    return Variable::ofNull();
}

static Variable TalentSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);

    // Transform

    // Execute
    auto talent = std::make_shared<Talent>(TalentType::Spell, nSpell);
    return Variable::ofTalent(std::move(talent));
}

static Variable TalentFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);

    // Transform

    // Execute
    auto talent = std::make_shared<Talent>(TalentType::Feat, nFeat);
    return Variable::ofTalent(std::move(talent));
}

static Variable TalentSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TalentSkill");
}

static Variable GetHasSpellEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oObject = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasSpellEffect");
}

static Variable GetEffectSpellId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eSpellEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectSpellId");
}

static Variable GetCreatureHasTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureHasTalent");
}

static Variable GetCreatureTalentRandom(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCategory = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);
    auto nInclusion = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureTalentRandom");
}

static Variable GetCreatureTalentBest(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCategory = getInt(args, 0);
    auto nCRMax = getInt(args, 1);
    auto oCreature = getObjectOrCaller(args, 2, ctx);
    auto nInclusion = getIntOrElse(args, 3, 0);
    auto nExcludeType = getIntOrElse(args, 4, -1);
    auto nExcludeId = getIntOrElse(args, 5, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureTalentBest");
}

static Variable GetGoldPieceValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetGoldPieceValue");
}

static Variable GetIsPlayableRacialType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPlayableRacialType");
}

static Variable JumpToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lDestination = getLocationArgument(args, 0);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newJumpToLocation(std::move(lDestination));
    getCaller(ctx)->addActionOnTop(std::move(action));
    return Variable::ofNull();
}

static Variable GetSkillRank(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto skill = static_cast<SkillType>(nSkill);
    auto target = checkCreature(oTarget);

    // Execute
    return Variable::ofInt(target->attributes().getSkillRank(skill));
}

static Variable GetAttackTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    auto target = creature->getAttackTarget();
    return Variable::ofObject(getObjectIdOrInvalid(target));
}

static Variable GetLastAttackType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackType");
}

static Variable GetLastAttackMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackMode");
}

static Variable GetDistanceBetween2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectA = getObject(args, 0, ctx);
    auto oObjectB = getObject(args, 1, ctx);

    // Transform

    // Execute
    float distance = oObjectA->getDistanceTo(glm::vec2(oObjectB->position()));
    return Variable::ofFloat(distance);
}

static Variable GetIsInCombat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);
    auto bOnlyCountReal = getIntOrElse(args, 1, 0);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->isInCombat()));
}

static Variable GetLastAssociateCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAssociate = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAssociateCommand");
}

static Variable GiveGoldToCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nGP = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    creature->giveGold(nGP);
    return Variable::ofNull();
}

static Variable SetIsDestroyable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bDestroyable = getInt(args, 0);
    auto bRaiseable = getIntOrElse(args, 1, 1);
    auto bSelectableWhenDead = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetIsDestroyable");
}

static Variable SetLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto bLocked = getInt(args, 1);

    // Transform
    auto target = checkDoor(oTarget);
    bool locked = static_cast<bool>(bLocked);

    // Execute
    target->setLocked(locked);
    return Variable::ofNull();
}

static Variable GetLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform
    auto target = checkDoor(oTarget);

    // Execute
    return Variable::ofInt(static_cast<int>(target->isLocked()));
}

static Variable GetClickingObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetClickingObject");
}

static Variable SetAssociateListenPatterns(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAssociateListenPatterns");
}

static Variable GetLastWeaponUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastWeaponUsed");
}

static Variable GetLastUsedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastUsedBy");
}

static Variable GetAbilityModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAbility = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAbilityModifier");
}

static Variable GetIdentified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIdentified");
}

static Variable SetIdentified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto bIdentified = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetIdentified");
}

static Variable GetDistanceBetweenLocations2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocationA = getLocationArgument(args, 0);
    auto lLocationB = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetDistanceBetweenLocations2D");
}

static Variable GetDistanceToObject2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    float result = caller->getDistanceTo(glm::vec2(oObject->position()));
    return Variable::ofFloat(result);
}

static Variable GetBlockingDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetBlockingDoor");
}

static Variable GetIsDoorActionPossible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTargetDoor = getObject(args, 0, ctx);
    auto nDoorAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsDoorActionPossible");
}

static Variable DoDoorAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTargetDoor = getObject(args, 0, ctx);
    auto nDoorAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DoDoorAction");
}

static Variable GetFirstItemInInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    auto item = oTarget->getFirstItem();
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable GetNextItemInInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    auto item = oTarget->getNextItem();
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable GetClassByPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassPosition = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    auto clazz = creature->attributes().getClassByPosition(nClassPosition);
    return Variable::ofInt(static_cast<int>(clazz));
}

static Variable GetLevelByPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassPosition = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    int level = creature->attributes().getLevelByPosition(nClassPosition);
    return Variable::ofInt(level);
}

static Variable GetLevelByClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassType = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);
    auto classType = static_cast<ClassType>(nClassType);

    // Execute
    int level = creature->attributes().getClassLevel(classType);
    return Variable::ofInt(level);
}

static Variable GetDamageDealtByType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageType = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetDamageDealtByType");
}

static Variable GetTotalDamageDealt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTotalDamageDealt");
}

static Variable GetLastDamager(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastDamager");
}

static Variable GetLastDisarmed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastDisarmed");
}

static Variable GetLastDisturbed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastDisturbed");
}

static Variable GetLastLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastLocked");
}

static Variable GetLastUnlocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastUnlocked");
}

static Variable GetInventoryDisturbType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetInventoryDisturbType");
}

static Variable GetInventoryDisturbItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetInventoryDisturbItem");
}

static Variable ShowUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObjectOrNull(args, 0, ctx);
    auto oCharacter = getObjectOrNull(args, 1, ctx);
    auto nDisableItemCreation = getIntOrElse(args, 2, 0);
    auto nDisableUpgrade = getIntOrElse(args, 3, 0);
    auto sOverride2DA = getStringOrElse(args, 4, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowUpgradeScreen");
}

static Variable VersusAlignmentEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);
    auto nLawChaos = getIntOrElse(args, 1, 0);
    auto nGoodEvil = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VersusAlignmentEffect");
}

static Variable VersusRacialTypeEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);
    auto nRacialType = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VersusRacialTypeEffect");
}

static Variable VersusTrapEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VersusTrapEffect");
}

static Variable GetGender(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->gender()));
}

static Variable GetIsTalentValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool valid;
    try {
        auto tTalent = getTalent(args, 0);
        valid = static_cast<bool>(tTalent);
    } catch (const RoutineArgumentException &ignored) {
        valid = false;
    }
    return Variable::ofInt(static_cast<int>(valid));
}

static Variable GetAttemptedAttackTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    auto target = caller->getAttemptedAttackTarget();
    return Variable::ofObject(getObjectIdOrInvalid(target));
}

static Variable GetTypeFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(tTalent->type()));
}

static Variable GetIdFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIdFromTalent");
}

static Variable PlayPazaak(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nOpponentPazaakDeck = getInt(args, 0);
    auto sEndScript = getString(args, 1);
    auto nMaxWager = getInt(args, 2);
    auto bShowTutorial = getIntOrElse(args, 3, 0);
    auto oOpponent = getObjectOrNull(args, 4, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayPazaak");
}

static Variable GetLastPazaakResult(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastPazaakResult");
}

static Variable DisplayFeedBackText(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nTextConstant = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisplayFeedBackText");
}

static Variable AddJournalQuestEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);
    auto nState = getInt(args, 1);
    auto bAllowOverrideHigher = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddJournalQuestEntry");
}

static Variable RemoveJournalQuestEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveJournalQuestEntry");
}

static Variable GetJournalEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetJournalEntry");
}

static Variable PlayRumblePattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPattern = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayRumblePattern");
}

static Variable StopRumblePattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPattern = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("StopRumblePattern");
}

static Variable SendMessageToPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlayer = getObject(args, 0, ctx);
    auto szMessage = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SendMessageToPC");
}

static Variable GetAttemptedSpellTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetAttemptedSpellTarget");
}

static Variable GetLastOpenedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetHasSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasSpell");
}

static Variable OpenStore(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oStore = getObject(args, 0, ctx);
    auto oPC = getObject(args, 1, ctx);
    auto nBonusMarkUp = getIntOrElse(args, 2, 0);
    auto nBonusMarkDown = getIntOrElse(args, 3, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("OpenStore");
}

static Variable GetFirstFactionMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMemberOfFaction = getObject(args, 0, ctx);
    auto bPCOnly = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstFactionMember");
}

static Variable GetNextFactionMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMemberOfFaction = getObject(args, 0, ctx);
    auto bPCOnly = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextFactionMember");
}

static Variable GetJournalQuestExperience(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetJournalQuestExperience");
}

static Variable JumpToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oToJumpTo = getObject(args, 0, ctx);
    auto nWalkStraightLineToPoint = getIntOrElse(args, 1, 1);

    // Transform
    auto walkStraightLineToPoint = static_cast<bool>(nWalkStraightLineToPoint);

    // Execute
    auto action = ctx.game.actionFactory().newJumpToObject(std::move(oToJumpTo), walkStraightLineToPoint);
    getCaller(ctx)->addActionOnTop(std::move(action));
    return Variable::ofNull();
}

static Variable SetMapPinEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMapPin = getObject(args, 0, ctx);
    auto nEnabled = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetMapPinEnabled");
}

static Variable PopUpGUIPanel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);
    auto nGUIPanel = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PopUpGUIPanel");
}

static Variable AddMultiClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassType = getInt(args, 0);
    auto oSource = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddMultiClass");
}

static Variable GetIsLinkImmune(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto eEffect = getEffect(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsLinkImmune");
}

static Variable GiveXPToCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nXpAmount = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    creature->giveXP(nXpAmount);
    return Variable::ofNull();
}

static Variable SetXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nXpAmount = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    creature->setXP(nXpAmount);
    return Variable::ofNull();
}

static Variable GetXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(creature->xp());
}

static Variable IntToHexString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IntToHexString");
}

static Variable GetBaseItemType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    return Variable::ofInt(item->baseItemType());
}

static Variable GetItemHasItemProperty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto nProperty = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetItemHasItemProperty");
}

static Variable GetItemACValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetItemACValue");
}

static Variable ExploreAreaForPlayer(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto oPlayer = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ExploreAreaForPlayer");
}

static Variable GetIsDay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsDay");
}

static Variable GetIsNight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsNight");
}

static Variable GetIsDawn(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsDawn");
}

static Variable GetIsDusk(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsDusk");
}

static Variable GetIsEncounterCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsEncounterCreature");
}

static Variable GetLastPlayerDying(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastPlayerDying");
}

static Variable GetStartingLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetStartingLocation");
}

static Variable ChangeToStandardFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreatureToChange = getObject(args, 0, ctx);
    auto nStandardFaction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeToStandardFaction");
}

static Variable SoundObjectPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform
    auto sound = checkSound(oSound);

    // Execute
    sound->setActive(true);
    return Variable::ofNull();
}

static Variable SoundObjectStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform
    auto sound = checkSound(oSound);

    // Execute
    sound->setActive(false);
    return Variable::ofNull();
}

static Variable SoundObjectSetVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto nVolume = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetVolume");
}

static Variable SoundObjectSetPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto vPosition = getVector(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetPosition");
}

static Variable SpeakOneLinerConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sDialogResRef = getStringOrElse(args, 0, "");
    auto oTokenTarget = getObjectOrNull(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpeakOneLinerConversation");
}

static Variable GetGold(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oTarget);

    // Execute
    return Variable::ofInt(creature->gold());
}

static Variable GetLastRespawnButtonPresser(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastRespawnButtonPresser");
}

static Variable SetLightsaberPowered(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto bOverride = getInt(args, 1);
    auto bPowered = getIntOrElse(args, 2, 1);
    auto bShowTransition = getIntOrElse(args, 3, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetLightsaberPowered");
}

static Variable GetIsWeaponEffective(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oVersus = getObjectOrNull(args, 0, ctx);
    auto bOffHand = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsWeaponEffective");
}

static Variable GetLastSpellHarmful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpellHarmful");
}

static Variable EventActivateItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto lTarget = getLocationArgument(args, 1);
    auto oTarget = getObjectOrNull(args, 2, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EventActivateItem");
}

static Variable MusicBackgroundPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundPlay");
}

static Variable MusicBackgroundStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundStop");
}

static Variable MusicBackgroundSetDelay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nDelay = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundSetDelay");
}

static Variable MusicBackgroundChangeDay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);
    auto nStreamingMusic = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundChangeDay");
}

static Variable MusicBackgroundChangeNight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);
    auto nStreamingMusic = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundChangeNight");
}

static Variable MusicBattlePlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBattlePlay");
}

static Variable MusicBattleStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBattleStop");
}

static Variable MusicBattleChange(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBattleChange");
}

static Variable AmbientSoundPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundPlay");
}

static Variable AmbientSoundStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundStop");
}

static Variable AmbientSoundChangeDay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundChangeDay");
}

static Variable AmbientSoundChangeNight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundChangeNight");
}

static Variable GetLastKiller(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastKiller");
}

static Variable GetSpellCastItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellCastItem");
}

static Variable GetItemActivated(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivated");
}

static Variable GetItemActivator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivator");
}

static Variable GetItemActivatedTargetLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivatedTargetLocation");
}

static Variable GetItemActivatedTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivatedTarget");
}

static Variable GetIsOpen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oObject->isOpen()));
}

static Variable TakeGoldFromCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAmount = getInt(args, 0);
    auto oCreatureToTakeFrom = getObject(args, 1, ctx);
    auto bDestroy = getIntOrElse(args, 2, 0);

    // Transform
    auto creatureToTakeFrom = checkCreature(oCreatureToTakeFrom);
    bool destroy = static_cast<bool>(bDestroy);

    // Execute
    if (creatureToTakeFrom) {
        creatureToTakeFrom->takeGold(nAmount);
    }
    if (!destroy) {
        auto caller = checkCreature(getCaller(ctx));
        caller->giveGold(nAmount);
    }
    return Variable::ofNull();
}

static Variable GetIsInConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsInConversation");
}

static Variable GetPlotFlag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    bool plotFlag = oTarget->plotFlag();
    return Variable::ofInt(static_cast<int>(plotFlag));
}

static Variable SetPlotFlag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto nPlotFlag = getInt(args, 1);

    // Transform
    bool plotFlag = static_cast<bool>(nPlotFlag);

    // Execute
    oTarget->setPlotFlag(plotFlag);
    return Variable::ofNull();
}

static Variable SetDialogPlaceableCamera(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCameraId = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetDialogPlaceableCamera");
}

static Variable GetSoloMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    bool solo = ctx.game.party().isSoloMode();
    return Variable::ofInt(static_cast<int>(solo));
}

static Variable GetMaxStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetMaxStealthXP");
}

static Variable SetMaxStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nMax = getInt(args, 0);

    // Transform

    // Execute
    ctx.game.module()->area()->setMaxStealthXP(nMax);
    return Variable::ofNull();
}

static Variable GetCurrentStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    int xp = ctx.game.module()->area()->currentStealthXP();
    return Variable::ofInt(xp);
}

static Variable GetNumStackedItems(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNumStackedItems");
}

static Variable SurrenderToEnemies(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("SurrenderToEnemies");
}

static Variable SetCurrentStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCurrent = getInt(args, 0);

    // Transform

    // Execute
    ctx.game.module()->area()->setCurrentStealthXP(nCurrent);
    return Variable::ofNull();
}

static Variable GetCreatureSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureSize");
}

static Variable AwardStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AwardStealthXP");
}

static Variable GetStealthXPEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetStealthXPEnabled");
}

static Variable SetStealthXPEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bEnabled = getInt(args, 0);

    // Transform
    bool enabled = static_cast<bool>(bEnabled);

    // Execute
    ctx.game.module()->area()->setStealthXPEnabled(enabled);
    return Variable::ofNull();
}

static Variable GetLastTrapDetected(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastTrapDetected");
}

static Variable GetNearestTrapToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);
    auto nTrapDetected = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNearestTrapToObject");
}

static Variable GetAttemptedMovementTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetAttemptedMovementTarget");
}

static Variable GetBlockingCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetBlockingCreature");
}

static Variable GetFortitudeSavingThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFortitudeSavingThrow");
}

static Variable GetWillSavingThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetWillSavingThrow");
}

static Variable GetReflexSavingThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetReflexSavingThrow");
}

static Variable GetChallengeRating(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetChallengeRating");
}

static Variable GetFoundEnemyCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFoundEnemyCreature");
}

static Variable GetMovementRate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetMovementRate");
}

static Variable GetSubRace(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->subrace()));
}

static Variable GetStealthXPDecrement(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetStealthXPDecrement");
}

static Variable SetStealthXPDecrement(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDecrement = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetStealthXPDecrement");
}

static Variable DuplicateHeadAppearance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCreatureToChange = getObject(args, 0, ctx);
    auto oidCreatureToMatch = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DuplicateHeadAppearance");
}

static Variable CutsceneAttack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto nAnimation = getInt(args, 1);
    auto nAttackResult = getInt(args, 2);
    auto nDamage = getInt(args, 3);

    // Transform
    auto attackResult = static_cast<AttackResultType>(nAttackResult);

    // Execute
    auto caller = checkCreature(getCaller(ctx));
    ctx.game.combat().addAttack(caller, oTarget, nullptr, attackResult, nDamage);
    return Variable::ofNull();
}

static Variable SetCameraMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlayer = getObject(args, 0, ctx);
    auto nCameraMode = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCameraMode");
}

static Variable SetLockOrientationInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetLockOrientationInDialog");
}

static Variable SetLockHeadFollowInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetLockHeadFollowInDialog");
}

static Variable CutsceneMove(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto vPosition = getVector(args, 1);
    auto nRun = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CutsceneMove");
}

static Variable EnableVideoEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nEffectType = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EnableVideoEffect");
}

static Variable StartNewModule(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sModuleName = getString(args, 0);
    auto sWayPoint = getStringOrElse(args, 1, "");
    auto sMovie1 = getStringOrElse(args, 2, "");
    auto sMovie2 = getStringOrElse(args, 3, "");
    auto sMovie3 = getStringOrElse(args, 4, "");
    auto sMovie4 = getStringOrElse(args, 5, "");
    auto sMovie5 = getStringOrElse(args, 6, "");
    auto sMovie6 = getStringOrElse(args, 7, "");

    // Transform
    auto moduleName = boost::to_lower_copy(sModuleName);
    auto waypoint = boost::to_lower_copy(sWayPoint);

    // Execute
    ctx.game.scheduleModuleTransition(moduleName, waypoint);
    return Variable::ofNull();
}

static Variable DisableVideoEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("DisableVideoEffect");
}

static Variable GetWeaponRanged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetWeaponRanged");
}

static Variable DoSinglePlayerAutoSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("DoSinglePlayerAutoSave");
}

static Variable GetGameDifficulty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetGameDifficulty");
}

static Variable GetUserActionsPending(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    return Variable::ofInt(static_cast<int>(caller->hasUserActionsPending()));
}

static Variable RevealMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vPoint = getVectorOrElse(args, 0, glm::vec3(0.0f, 0.0f, 0.0f));
    auto nRadius = getIntOrElse(args, 1, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RevealMap");
}

static Variable SetTutorialWindowsEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bEnabled = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTutorialWindowsEnabled");
}

static Variable ShowTutorialWindow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nWindow = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowTutorialWindow");
}

static Variable StartCreditSequence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bTransparentBackground = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("StartCreditSequence");
}

static Variable IsCreditSequenceInProgress(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("IsCreditSequenceInProgress");
}

static Variable GetCurrentAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    auto action = oObject->getCurrentAction();
    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

static Variable GetDifficultyModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetDifficultyModifier");
}

static Variable GetAppearanceType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAppearanceType");
}

static Variable FloatingTextStrRefOnCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRefToDisplay = getInt(args, 0);
    auto oCreatureToFloatAbove = getObject(args, 1, ctx);
    auto bBroadcastToFaction = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FloatingTextStrRefOnCreature");
}

static Variable FloatingTextStringOnCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sStringToDisplay = getString(args, 0);
    auto oCreatureToFloatAbove = getObject(args, 1, ctx);
    auto bBroadcastToFaction = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FloatingTextStringOnCreature");
}

static Variable GetTrapDisarmable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDisarmable");
}

static Variable GetTrapDetectable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDetectable");
}

static Variable GetTrapDetectedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDetectedBy");
}

static Variable GetTrapFlagged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapFlagged");
}

static Variable GetTrapBaseType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapBaseType");
}

static Variable GetTrapOneShot(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapOneShot");
}

static Variable GetTrapCreator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapCreator");
}

static Variable GetTrapKeyTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapKeyTag");
}

static Variable GetTrapDisarmDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDisarmDC");
}

static Variable GetTrapDetectDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDetectDC");
}

static Variable GetLockKeyRequired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockKeyRequired");
}

static Variable GetLockKeyTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockKeyTag");
}

static Variable GetLockLockable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockLockable");
}

static Variable GetLockUnlockDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockUnlockDC");
}

static Variable GetLockLockDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockLockDC");
}

static Variable GetPCLevellingUp(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetPCLevellingUp");
}

static Variable GetHasFeatEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oObject = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasFeatEffect");
}

static Variable SetPlaceableIllumination(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObjectOrCaller(args, 0, ctx);
    auto bIlluminate = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPlaceableIllumination");
}

static Variable GetPlaceableIllumination(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPlaceableIllumination");
}

static Variable GetIsPlaceableObjectActionPossible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObject(args, 0, ctx);
    auto nPlaceableAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPlaceableObjectActionPossible");
}

static Variable DoPlaceableObjectAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObject(args, 0, ctx);
    auto nPlaceableAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DoPlaceableObjectAction");
}

static Variable GetFirstPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

static Variable GetNextPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofObject(kObjectInvalid);
}

static Variable SetTrapDetectedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrap = getObject(args, 0, ctx);
    auto oDetector = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTrapDetectedBy");
}

static Variable GetIsTrapped(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsTrapped");
}

static Variable SetEffectIcon(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);
    auto nIcon = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEffectIcon");
}

static Variable FaceObjectAwayFromObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFacer = getObject(args, 0, ctx);
    auto oObjectToFaceAwayFrom = getObject(args, 1, ctx);

    // Transform

    // Execute
    oFacer->faceAwayFrom(*oObjectToFaceAwayFrom);
    return Variable::ofNull();
}

static Variable PopUpDeathGUIPanel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);
    auto bRespawnButtonEnabled = getIntOrElse(args, 1, 1);
    auto bWaitForHelpButtonEnabled = getIntOrElse(args, 2, 1);
    auto nHelpStringReference = getIntOrElse(args, 3, 0);
    auto sHelpString = getStringOrElse(args, 4, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("PopUpDeathGUIPanel");
}

static Variable SetTrapDisabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrap = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTrapDisabled");
}

static Variable GetLastHostileActor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oVictim = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastHostileActor");
}

static Variable ExportAllCharacters(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ExportAllCharacters");
}

static Variable MusicBackgroundGetDayTrack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundGetDayTrack");
}

static Variable MusicBackgroundGetNightTrack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundGetNightTrack");
}

static Variable WriteTimestampedLogEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sLogEntry = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("WriteTimestampedLogEntry");
}

static Variable GetModuleName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleName");
}

static Variable GetFactionLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMemberOfFaction = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionLeader");
}

static Variable EndGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nShowEndGameGui = getIntOrElse(args, 0, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EndGame");
}

static Variable GetRunScriptVar(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(ctx.execution.scriptVar);
}

static Variable GetCreatureMovmentType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureMovmentType");
}

static Variable AmbientSoundSetDayVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nVolume = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundSetDayVolume");
}

static Variable AmbientSoundSetNightVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nVolume = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundSetNightVolume");
}

static Variable MusicBackgroundGetBattleTrack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundGetBattleTrack");
}

static Variable GetHasInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasInventory");
}

static Variable GetStrRefSoundDuration(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRef = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetStrRefSoundDuration");
}

static Variable AddToParty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);
    auto oPartyLeader = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddToParty");
}

static Variable RemoveFromParty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveFromParty");
}

static Variable AddPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    bool added = ctx.game.party().addAvailableMember(nNPC, creature->blueprintResRef());
    return Variable::ofInt(static_cast<int>(added));
}

static Variable RemovePartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool removed = false;
    if (ctx.game.party().isMember(nNPC)) {
        ctx.game.party().removeMember(nNPC);
        auto area = ctx.game.module()->area();
        area->unloadParty();
        area->reloadParty();
        removed = true;
    }
    return Variable::ofInt(static_cast<int>(removed));
}

static Variable IsObjectPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    bool member = ctx.game.party().isMember(*creature);
    return Variable::ofInt(static_cast<int>(member));
}

static Variable GetPartyMemberByIndex(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);

    // Transform

    // Execute
    auto member = ctx.game.party().getMember(nIndex);
    return Variable::ofObject(getObjectIdOrInvalid(member));
}

static Variable GetGlobalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    bool value = ctx.game.getGlobalBoolean(sIdentifier);
    return Variable::ofInt(static_cast<int>(value));
}

static Variable SetGlobalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalBoolean(sIdentifier, nValue);
    return Variable::ofNull();
}

static Variable GetGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    int value = ctx.game.getGlobalNumber(sIdentifier);
    return Variable::ofInt(value);
}

static Variable SetGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalNumber(sIdentifier, nValue);
    return Variable::ofNull();
}

static Variable AurPostString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nX = getInt(args, 1);
    auto nY = getInt(args, 2);
    auto fLife = getFloat(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AurPostString");
}

static Variable AddJournalWorldEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);
    auto szEntry = getString(args, 1);
    auto szTitle = getStringOrElse(args, 2, "World Entry");

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddJournalWorldEntry");
}

static Variable AddJournalWorldEntryStrref(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto strref = getInt(args, 0);
    auto strrefTitle = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddJournalWorldEntryStrref");
}

static Variable BarkString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto strRef = getInt(args, 1);
    auto nBarkX = getIntOrElse(args, 2, -1);
    auto nBarkY = getIntOrElse(args, 3, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("BarkString");
}

static Variable DeleteJournalWorldAllEntries(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("DeleteJournalWorldAllEntries");
}

static Variable DeleteJournalWorldEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DeleteJournalWorldEntry");
}

static Variable DeleteJournalWorldEntryStrref(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto strref = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DeleteJournalWorldEntryStrref");
}

static Variable PlayVisualAreaEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nEffectID = getInt(args, 0);
    auto lTarget = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayVisualAreaEffect");
}

static Variable SetJournalQuestEntryPicture(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);
    auto oObject = getObject(args, 1, ctx);
    auto nPictureIndex = getInt(args, 2);
    auto bAllPartyMemebers = getIntOrElse(args, 3, 1);
    auto bAllPlayers = getIntOrElse(args, 4, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetJournalQuestEntryPicture");
}

static Variable GetLocalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);

    // Transform

    // Execute
    bool value = oObject->getLocalBoolean(nIndex);
    return Variable::ofInt(static_cast<int>(value));
}

static Variable SetLocalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);
    auto nValue = getInt(args, 2);

    // Transform

    // Execute
    oObject->setLocalBoolean(nIndex, nValue);
    return Variable::ofNull();
}

static Variable GetLocalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);

    // Transform

    // Execute
    return Variable::ofInt(oObject->getLocalNumber(nIndex));
}

static Variable SetLocalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);
    auto nValue = getInt(args, 2);

    // Transform

    // Execute
    oObject->setLocalNumber(nIndex, nValue);
    return Variable::ofNull();
}

static Variable SoundObjectGetPitchVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectGetPitchVariance");
}

static Variable SoundObjectSetPitchVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto fVariance = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetPitchVariance");
}

static Variable SoundObjectGetVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectGetVolume");
}

static Variable GetGlobalLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    return Variable::ofLocation(ctx.game.getGlobalLocation(sIdentifier));
}

static Variable SetGlobalLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto lValue = getLocationArgument(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalLocation(sIdentifier, lValue);
    return Variable::ofNull();
}

static Variable AddAvailableNPCByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddAvailableNPCByObject");
}

static Variable RemoveAvailableNPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool removed = ctx.game.party().removeAvailableMember(nNPC);
    return Variable::ofInt(static_cast<int>(removed));
}

static Variable IsAvailableCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool available = ctx.game.party().isMemberAvailable(nNPC);
    return Variable::ofInt(static_cast<int>(available));
}

static Variable AddAvailableNPCByTemplate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto sTemplate = getString(args, 1);

    // Transform
    auto tmplt = boost::to_lower_copy(sTemplate);

    // Execute
    bool added = ctx.game.party().addAvailableMember(nNPC, tmplt);
    return Variable::ofInt(static_cast<int>(added));
}

static Variable SpawnAvailableNPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto lPosition = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpawnAvailableNPC");
}

static Variable IsNPCPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool member = ctx.game.party().isMember(nNPC);
    return Variable::ofInt(static_cast<int>(member));
}

static Variable GetIsConversationActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsConversationActive");
}

static Variable GetPartyAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetPartyAIStyle");
}

static Variable GetNPCAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->aiStyle()));
}

static Variable SetPartyAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStyle = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPartyAIStyle");
}

static Variable SetNPCAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nStyle = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);
    auto style = static_cast<NPCAIStyle>(nStyle);

    // Execute
    creature->setAIStyle(style);
    return Variable::ofNull();
}

static Variable SetNPCSelectability(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto nSelectability = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetNPCSelectability");
}

static Variable GetNPCSelectability(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNPCSelectability");
}

static Variable ClearAllEffects(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = getCaller(ctx);
    caller->clearAllEffects();
    return Variable::ofNull();
}

static Variable GetLastConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastConversation");
}

static Variable ShowPartySelectionGUI(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sExitScript = getStringOrElse(args, 0, "");
    auto nForceNPC1 = getIntOrElse(args, 1, -1);
    auto nForceNPC2 = getIntOrElse(args, 2, -1);
    auto nAllowCancel = getIntOrElse(args, 3, 0);

    // Transform
    auto exitScript = boost::to_lower_copy(sExitScript);
    auto forceNPC1 = static_cast<bool>(nForceNPC1);
    auto forceNPC2 = static_cast<bool>(nForceNPC2);

    // Execute
    PartySelectionContext partyCtx;
    partyCtx.exitScript = exitScript;
    partyCtx.forceNpc1 = forceNPC1;
    partyCtx.forceNpc2 = forceNPC2;
    ctx.game.openPartySelection(partyCtx);
    return Variable::ofNull();
}

static Variable GetStandardFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oObject);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->faction()));
}

static Variable GivePlotXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sPlotName = getString(args, 0);
    auto nPercentage = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GivePlotXP");
}

static Variable GetMinOneHP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oObject->isMinOneHP()));
}

static Variable SetMinOneHP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nMinOneHP = getInt(args, 1);

    // Transform
    auto minOneHP = static_cast<bool>(nMinOneHP);

    // Execute
    oObject->setMinOneHP(minOneHP);
    return Variable::ofNull();
}

static Variable SetGlobalFadeIn(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fWait = getFloatOrElse(args, 0, 0.0f);
    auto fLength = getFloatOrElse(args, 1, 0.0f);
    auto fR = getFloatOrElse(args, 2, 0.0f);
    auto fG = getFloatOrElse(args, 3, 0.0f);
    auto fB = getFloatOrElse(args, 4, 0.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetGlobalFadeIn");
}

static Variable SetGlobalFadeOut(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fWait = getFloatOrElse(args, 0, 0.0f);
    auto fLength = getFloatOrElse(args, 1, 0.0f);
    auto fR = getFloatOrElse(args, 2, 0.0f);
    auto fG = getFloatOrElse(args, 3, 0.0f);
    auto fB = getFloatOrElse(args, 4, 0.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetGlobalFadeOut");
}

static Variable GetLastHostileTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastHostileTarget");
}

static Variable GetLastAttackAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackAction");
}

static Variable GetLastForcePowerUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastForcePowerUsed");
}

static Variable GetLastCombatFeatUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastCombatFeatUsed");
}

static Variable GetLastAttackResult(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackResult");
}

static Variable GetWasForcePowerSuccessful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetWasForcePowerSuccessful");
}

static Variable GetFirstAttacker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstAttacker");
}

static Variable GetNextAttacker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextAttacker");
}

static Variable SetFormation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAnchor = getObject(args, 0, ctx);
    auto oCreature = getObject(args, 1, ctx);
    auto nFormationPattern = getInt(args, 2);
    auto nPosition = getInt(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetFormation");
}

static Variable SetForcePowerUnsuccessful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nResult = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetForcePowerUnsuccessful");
}

static Variable GetIsDebilitated(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->isDebilitated()));
}

static Variable PlayMovie(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sMovie = getString(args, 0);
    auto nStreamingMusic = getIntOrElse(args, 1, 0);

    // Transform
    auto movie = boost::to_lower_copy(sMovie);

    // Execute
    ctx.game.playVideo(movie);
    return Variable::ofNull();
}

static Variable SaveNPCState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SaveNPCState");
}

static Variable GetCategoryFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCategoryFromTalent");
}

static Variable SurrenderByFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFactionFrom = getInt(args, 0);
    auto nFactionTo = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SurrenderByFaction");
}

static Variable ChangeFactionByFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFactionFrom = getInt(args, 0);
    auto nFactionTo = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeFactionByFaction");
}

static Variable PlayRoomAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sRoom = getString(args, 0);
    auto nAnimation = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayRoomAnimation");
}

static Variable ShowGalaxyMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowGalaxyMap");
}

static Variable SetPlanetSelectable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);
    auto bSelectable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPlanetSelectable");
}

static Variable GetPlanetSelectable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPlanetSelectable");
}

static Variable SetPlanetAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);
    auto bAvailable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPlanetAvailable");
}

static Variable GetPlanetAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPlanetAvailable");
}

static Variable GetSelectedPlanet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSelectedPlanet");
}

static Variable SoundObjectFadeAndStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto fSeconds = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectFadeAndStop");
}

static Variable SetAreaFogColor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto fRed = getFloat(args, 1);
    auto fGreen = getFloat(args, 2);
    auto fBlue = getFloat(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAreaFogColor");
}

static Variable ChangeItemCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sItem = getString(args, 0);
    auto fCostMultiplier = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeItemCost");
}

static Variable GetIsLiveContentAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPkg = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsLiveContentAvailable");
}

static Variable ResetDialogState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ResetDialogState");
}

static Variable SetGoodEvilValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nAlignment = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetGoodEvilValue");
}

static Variable GetIsPoisoned(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPoisoned");
}

static Variable GetSpellTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellTarget");
}

static Variable SetSoloMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bActivate = getInt(args, 0);

    // Transform
    auto activate = static_cast<bool>(bActivate);

    // Execute
    ctx.game.party().setSoloMode(activate);
    return Variable::ofNull();
}

static Variable CancelPostDialogCharacterSwitch(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("CancelPostDialogCharacterSwitch");
}

static Variable SetMaxHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nMaxHP = getInt(args, 1);

    // Transform

    // Execute
    oObject->setMaxHitPoints(nMaxHP);
    return Variable::ofNull();
}

static Variable NoClicksFor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fDuration = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("NoClicksFor");
}

static Variable HoldWorldFadeInForDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("HoldWorldFadeInForDialog");
}

static Variable ShipBuild(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(static_cast<int>(kShipBuild));
}

static Variable SurrenderRetainBuffs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("SurrenderRetainBuffs");
}

static Variable SuppressStatusSummaryEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumEntries = getIntOrElse(args, 0, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SuppressStatusSummaryEntry");
}

static Variable GetCheatCode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCode = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(0);
}

static Variable SetMusicVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fVolume = getFloatOrElse(args, 0, 1.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetMusicVolume");
}

static Variable CreateItemOnFloor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTemplate = getString(args, 0);
    auto lLocation = getLocationArgument(args, 1);
    auto bUseAppearAnimation = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CreateItemOnFloor");
}

static Variable SetAvailableNPCId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oidNPC = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAvailableNPCId");
}

static Variable GetScriptParameter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetScriptParameter");
}

static Variable SetFadeUntilScript(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("SetFadeUntilScript");
}

static Variable GetItemComponent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemComponent");
}

static Variable GetItemComponentPieceValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemComponentPieceValue");
}

static Variable ShowChemicalUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCharacter = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowChemicalUpgradeScreen");
}

static Variable GetChemicals(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetChemicals");
}

static Variable GetChemicalPieceValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetChemicalPieceValue");
}

static Variable GetSpellForcePointCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellForcePointCost");
}

static Variable GetFeatAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFeatAcquired");
}

static Variable GetSpellAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellAcquired");
}

static Variable ShowSwoopUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ShowSwoopUpgradeScreen");
}

static Variable GrantFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GrantFeat");
}

static Variable GrantSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GrantSpell");
}

static Variable SpawnMine(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nMineType = getInt(args, 0);
    auto lPoint = getLocationArgument(args, 1);
    auto nDetectDCBase = getInt(args, 2);
    auto nDisarmDCBase = getInt(args, 3);
    auto oCreator = getObject(args, 4, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpawnMine");
}

static Variable SetFakeCombatState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nEnable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetFakeCombatState");
}

static Variable GetOwnerDemolitionsSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetOwnerDemolitionsSkill");
}

static Variable SetOrientOnClick(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);
    auto nState = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetOrientOnClick");
}

static Variable GetInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetInfluence");
}

static Variable SetInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto nInfluence = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetInfluence");
}

static Variable ModifyInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto nModifier = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyInfluence");
}

static Variable GetRacialSubType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetRacialSubType");
}

static Variable IncrementGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nAmount = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IncrementGlobalNumber");
}

static Variable DecrementGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nAmount = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DecrementGlobalNumber");
}

static Variable SetBonusForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nBonusFP = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetBonusForcePoints");
}

static Variable AddBonusForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nBonusFP = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddBonusForcePoints");
}

static Variable GetBonusForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetBonusForcePoints");
}

static Variable IsMoviePlaying(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("IsMoviePlaying");
}

static Variable QueueMovie(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sMovie = getString(args, 0);
    auto nSkippable = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("QueueMovie");
}

static Variable PlayMovieQueue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAllowSkips = getIntOrElse(args, 0, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayMovieQueue");
}

static Variable YavinHackDoorClose(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("YavinHackDoorClose");
}

static Variable IsStealthed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsStealthed");
}

static Variable IsMeditating(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsMeditating");
}

static Variable IsInTotalDefense(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsInTotalDefense");
}

static Variable SetHealTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidHealer = getObject(args, 0, ctx);
    auto oidTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetHealTarget");
}

static Variable GetHealTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidHealer = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHealTarget");
}

static Variable GetRandomDestination(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto rangeLimit = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetRandomDestination");
}

static Variable IsFormActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nFormID = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsFormActive");
}

static Variable GetSpellFormMask(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpellID = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellFormMask");
}

static Variable GetSpellBaseForcePointCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpellID = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellBaseForcePointCost");
}

static Variable SetKeepStealthInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStealthState = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetKeepStealthInDialog");
}

static Variable HasLineOfSight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vSource = getVector(args, 0);
    auto vTarget = getVector(args, 1);
    auto oSource = getObjectOrNull(args, 2, ctx);
    auto oTarget = getObjectOrNull(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("HasLineOfSight");
}

static Variable ShowDemoScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTexture = getString(args, 0);
    auto nTimeout = getInt(args, 1);
    auto nDisplayString = getInt(args, 2);
    auto nDisplayX = getInt(args, 3);
    auto nDisplayY = getInt(args, 4);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowDemoScreen");
}

static Variable ForceHeartbeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ForceHeartbeat");
}

static Variable IsRunning(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsRunning");
}

static Variable SetForfeitConditions(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nForfeitFlags = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetForfeitConditions");
}

static Variable GetLastForfeitViolation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastForfeitViolation");
}

static Variable ModifyReflexSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);
    auto aModValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyReflexSavingThrowBase");
}

static Variable ModifyFortitudeSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);
    auto aModValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyFortitudeSavingThrowBase");
}

static Variable ModifyWillSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);
    auto aModValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyWillSavingThrowBase");
}

static Variable GetScriptStringParameter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetScriptStringParameter");
}

static Variable GetObjectPersonalSpace(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetObjectPersonalSpace");
}

static Variable AdjustCreatureAttributes(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nAttribute = getInt(args, 1);
    auto nAmount = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustCreatureAttributes");
}

static Variable SetCreatureAILevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nPriority = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCreatureAILevel");
}

static Variable ResetCreatureAILevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ResetCreatureAILevel");
}

static Variable AddAvailablePUPByTemplate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto sTemplate = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddAvailablePUPByTemplate");
}

static Variable AddAvailablePUPByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto oPuppet = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddAvailablePUPByObject");
}

static Variable AssignPUP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto nNPC = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AssignPUP");
}

static Variable SpawnAvailablePUP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto lLocation = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpawnAvailablePUP");
}

static Variable AddPartyPuppet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto oidCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddPartyPuppet");
}

static Variable GetPUPOwner(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPUP = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPUPOwner");
}

static Variable GetIsPuppet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPUP = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPuppet");
}

static Variable GetIsPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCharacter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPartyLeader");
}

static Variable GetPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetPartyLeader");
}

static Variable RemoveNPCFromPartyToBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveNPCFromPartyToBase");
}

static Variable CreatureFlourishWeapon(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CreatureFlourishWeapon");
}

static Variable ChangeObjectAppearance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectToChange = getObject(args, 0, ctx);
    auto nAppearance = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeObjectAppearance");
}

static Variable GetIsXBox(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsXBox");
}

static Variable PlayOverlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto nAnimation = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayOverlayAnimation");
}

static Variable UnlockAllSongs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("UnlockAllSongs");
}

static Variable DisableMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisableMap");
}

static Variable DetonateMine(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMine = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DetonateMine");
}

static Variable DisableHealthRegen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisableHealthRegen");
}

static Variable SetCurrentForm(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nFormID = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCurrentForm");
}

static Variable SetDisableTransit(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetDisableTransit");
}

static Variable SetInputClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClass = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetInputClass");
}

static Variable SetForceAlwaysUpdate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nFlag = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetForceAlwaysUpdate");
}

static Variable EnableRain(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EnableRain");
}

static Variable DisplayMessageBox(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRef = getInt(args, 0);
    auto sIcon = getStringOrElse(args, 1, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisplayMessageBox");
}

static Variable DisplayDatapad(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oDatapad = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisplayDatapad");
}

static Variable RemoveHeartbeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveHeartbeat");
}

static Variable RemoveEffectByID(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nEffectID = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveEffectByID");
}

static Variable RemoveEffectByExactMatch(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto eEffect = getEffect(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveEffectByExactMatch");
}

static Variable AdjustCreatureSkills(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nSkill = getInt(args, 1);
    auto nAmount = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustCreatureSkills");
}

static Variable GetSkillRankBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto oObject = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSkillRankBase");
}

static Variable EnableRendering(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto bEnable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EnableRendering");
}

static Variable GetCombatActionsPending(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCombatActionsPending");
}

static Variable SaveNPCByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oidCharacter = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SaveNPCByObject");
}

static Variable SavePUPByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto oidPuppet = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SavePUPByObject");
}

static Variable GetIsPlayerMadeCharacter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCharacter = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPlayerMadeCharacter");
}

static Variable RebuildPartyTable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("RebuildPartyTable");
}

void Routines::registerMainKotorRoutines() {
    insert(0, "Random", R_INT, {R_INT}, &Random);
    insert(1, "PrintString", R_VOID, {R_STRING}, &PrintString);
    insert(2, "PrintFloat", R_VOID, {R_FLOAT, R_INT, R_INT}, &PrintFloat);
    insert(3, "FloatToString", R_STRING, {R_FLOAT, R_INT, R_INT}, &FloatToString);
    insert(4, "PrintInteger", R_VOID, {R_INT}, &PrintInteger);
    insert(5, "PrintObject", R_VOID, {R_OBJECT}, &PrintObject);
    insert(6, "AssignCommand", R_VOID, {R_OBJECT, R_ACTION}, &AssignCommand);
    insert(7, "DelayCommand", R_VOID, {R_FLOAT, R_ACTION}, &DelayCommand);
    insert(8, "ExecuteScript", R_VOID, {R_STRING, R_OBJECT, R_INT}, &ExecuteScript);
    insert(9, "ClearAllActions", R_VOID, {}, &ClearAllActions);
    insert(10, "SetFacing", R_VOID, {R_FLOAT}, &SetFacing);
    insert(11, "SwitchPlayerCharacter", R_INT, {R_INT}, &SwitchPlayerCharacter);
    insert(12, "SetTime", R_VOID, {R_INT, R_INT, R_INT, R_INT}, &SetTime);
    insert(13, "SetPartyLeader", R_INT, {R_INT}, &SetPartyLeader);
    insert(14, "SetAreaUnescapable", R_VOID, {R_INT}, &SetAreaUnescapable);
    insert(15, "GetAreaUnescapable", R_INT, {}, &GetAreaUnescapable);
    insert(16, "GetTimeHour", R_INT, {}, &GetTimeHour);
    insert(17, "GetTimeMinute", R_INT, {}, &GetTimeMinute);
    insert(18, "GetTimeSecond", R_INT, {}, &GetTimeSecond);
    insert(19, "GetTimeMillisecond", R_INT, {}, &GetTimeMillisecond);
    insert(24, "GetArea", R_OBJECT, {R_OBJECT}, &GetArea);
    insert(25, "GetEnteringObject", R_OBJECT, {}, &GetEnteringObject);
    insert(26, "GetExitingObject", R_OBJECT, {}, &GetExitingObject);
    insert(27, "GetPosition", R_VECTOR, {R_OBJECT}, &GetPosition);
    insert(28, "GetFacing", R_FLOAT, {R_OBJECT}, &GetFacing);
    insert(29, "GetItemPossessor", R_OBJECT, {R_OBJECT}, &GetItemPossessor);
    insert(30, "GetItemPossessedBy", R_OBJECT, {R_OBJECT, R_STRING}, &GetItemPossessedBy);
    insert(31, "CreateItemOnObject", R_OBJECT, {R_STRING, R_OBJECT, R_INT}, &CreateItemOnObject);
    insert(36, "GetLastAttacker", R_OBJECT, {R_OBJECT}, &GetLastAttacker);
    insert(38, "GetNearestCreature", R_OBJECT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &GetNearestCreature);
    insert(41, "GetDistanceToObject", R_FLOAT, {R_OBJECT}, &GetDistanceToObject);
    insert(42, "GetIsObjectValid", R_INT, {R_OBJECT}, &GetIsObjectValid);
    insert(45, "SetCameraFacing", R_VOID, {R_FLOAT}, &SetCameraFacing);
    insert(46, "PlaySound", R_VOID, {R_STRING}, &PlaySound);
    insert(47, "GetSpellTargetObject", R_OBJECT, {}, &GetSpellTargetObject);
    insert(49, "GetCurrentHitPoints", R_INT, {R_OBJECT}, &GetCurrentHitPoints);
    insert(50, "GetMaxHitPoints", R_INT, {R_OBJECT}, &GetMaxHitPoints);
    insert(52, "GetLastItemEquipped", R_OBJECT, {}, &GetLastItemEquipped);
    insert(53, "GetSubScreenID", R_INT, {}, &GetSubScreenID);
    insert(54, "CancelCombat", R_VOID, {R_OBJECT}, &CancelCombat);
    insert(55, "GetCurrentForcePoints", R_INT, {R_OBJECT}, &GetCurrentForcePoints);
    insert(56, "GetMaxForcePoints", R_INT, {R_OBJECT}, &GetMaxForcePoints);
    insert(57, "PauseGame", R_VOID, {R_INT}, &PauseGame);
    insert(58, "SetPlayerRestrictMode", R_VOID, {R_INT}, &SetPlayerRestrictMode);
    insert(59, "GetStringLength", R_INT, {R_STRING}, &GetStringLength);
    insert(60, "GetStringUpperCase", R_STRING, {R_STRING}, &GetStringUpperCase);
    insert(61, "GetStringLowerCase", R_STRING, {R_STRING}, &GetStringLowerCase);
    insert(62, "GetStringRight", R_STRING, {R_STRING, R_INT}, &GetStringRight);
    insert(63, "GetStringLeft", R_STRING, {R_STRING, R_INT}, &GetStringLeft);
    insert(64, "InsertString", R_STRING, {R_STRING, R_STRING, R_INT}, &InsertString);
    insert(65, "GetSubString", R_STRING, {R_STRING, R_INT, R_INT}, &GetSubString);
    insert(66, "FindSubString", R_INT, {R_STRING, R_STRING}, &FindSubString);
    insert(67, "fabs", R_FLOAT, {R_FLOAT}, &fabs);
    insert(68, "cos", R_FLOAT, {R_FLOAT}, &cos);
    insert(69, "sin", R_FLOAT, {R_FLOAT}, &sin);
    insert(70, "tan", R_FLOAT, {R_FLOAT}, &tan);
    insert(71, "acos", R_FLOAT, {R_FLOAT}, &acos);
    insert(72, "asin", R_FLOAT, {R_FLOAT}, &asin);
    insert(73, "atan", R_FLOAT, {R_FLOAT}, &atan);
    insert(74, "log", R_FLOAT, {R_FLOAT}, &log);
    insert(75, "pow", R_FLOAT, {R_FLOAT, R_FLOAT}, &pow);
    insert(76, "sqrt", R_FLOAT, {R_FLOAT}, &sqrt);
    insert(77, "abs", R_INT, {R_INT}, &abs);
    insert(83, "GetPlayerRestrictMode", R_INT, {R_OBJECT}, &GetPlayerRestrictMode);
    insert(84, "GetCasterLevel", R_INT, {R_OBJECT}, &GetCasterLevel);
    insert(85, "GetFirstEffect", R_EFFECT, {R_OBJECT}, &GetFirstEffect);
    insert(86, "GetNextEffect", R_EFFECT, {R_OBJECT}, &GetNextEffect);
    insert(87, "RemoveEffect", R_VOID, {R_OBJECT, R_EFFECT}, &RemoveEffect);
    insert(88, "GetIsEffectValid", R_INT, {R_EFFECT}, &GetIsEffectValid);
    insert(89, "GetEffectDurationType", R_INT, {R_EFFECT}, &GetEffectDurationType);
    insert(90, "GetEffectSubType", R_INT, {R_EFFECT}, &GetEffectSubType);
    insert(91, "GetEffectCreator", R_OBJECT, {R_EFFECT}, &GetEffectCreator);
    insert(92, "IntToString", R_STRING, {R_INT}, &IntToString);
    insert(93, "GetFirstObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &GetFirstObjectInArea);
    insert(94, "GetNextObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &GetNextObjectInArea);
    insert(95, "d2", R_INT, {R_INT}, &d2);
    insert(96, "d3", R_INT, {R_INT}, &d3);
    insert(97, "d4", R_INT, {R_INT}, &d4);
    insert(98, "d6", R_INT, {R_INT}, &d6);
    insert(99, "d8", R_INT, {R_INT}, &d8);
    insert(100, "d10", R_INT, {R_INT}, &d10);
    insert(101, "d12", R_INT, {R_INT}, &d12);
    insert(102, "d20", R_INT, {R_INT}, &d20);
    insert(103, "d100", R_INT, {R_INT}, &d100);
    insert(104, "VectorMagnitude", R_FLOAT, {R_VECTOR}, &VectorMagnitude);
    insert(105, "GetMetaMagicFeat", R_INT, {}, &GetMetaMagicFeat);
    insert(106, "GetObjectType", R_INT, {R_OBJECT}, &GetObjectType);
    insert(107, "GetRacialType", R_INT, {R_OBJECT}, &GetRacialType);
    insert(108, "FortitudeSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &FortitudeSave);
    insert(109, "ReflexSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &ReflexSave);
    insert(110, "WillSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &WillSave);
    insert(111, "GetSpellSaveDC", R_INT, {}, &GetSpellSaveDC);
    insert(112, "MagicalEffect", R_EFFECT, {R_EFFECT}, &MagicalEffect);
    insert(113, "SupernaturalEffect", R_EFFECT, {R_EFFECT}, &SupernaturalEffect);
    insert(114, "ExtraordinaryEffect", R_EFFECT, {R_EFFECT}, &ExtraordinaryEffect);
    insert(116, "GetAC", R_INT, {R_OBJECT, R_INT}, &GetAC);
    insert(121, "RoundsToSeconds", R_FLOAT, {R_INT}, &RoundsToSeconds);
    insert(122, "HoursToSeconds", R_FLOAT, {R_INT}, &HoursToSeconds);
    insert(123, "TurnsToSeconds", R_FLOAT, {R_INT}, &TurnsToSeconds);
    insert(124, "SoundObjectSetFixedVariance", R_VOID, {R_OBJECT, R_FLOAT}, &SoundObjectSetFixedVariance);
    insert(125, "GetGoodEvilValue", R_INT, {R_OBJECT}, &GetGoodEvilValue);
    insert(126, "GetPartyMemberCount", R_INT, {}, &GetPartyMemberCount);
    insert(127, "GetAlignmentGoodEvil", R_INT, {R_OBJECT}, &GetAlignmentGoodEvil);
    insert(128, "GetFirstObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &GetFirstObjectInShape);
    insert(129, "GetNextObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &GetNextObjectInShape);
    insert(131, "SignalEvent", R_VOID, {R_OBJECT, R_EVENT}, &SignalEvent);
    insert(132, "EventUserDefined", R_EVENT, {R_INT}, &EventUserDefined);
    insert(137, "VectorNormalize", R_VECTOR, {R_VECTOR}, &VectorNormalize);
    insert(138, "GetItemStackSize", R_INT, {R_OBJECT}, &GetItemStackSize);
    insert(139, "GetAbilityScore", R_INT, {R_OBJECT, R_INT}, &GetAbilityScore);
    insert(140, "GetIsDead", R_INT, {R_OBJECT}, &GetIsDead);
    insert(141, "PrintVector", R_VOID, {R_VECTOR, R_INT}, &PrintVector);
    insert(142, "Vector", R_VECTOR, {R_FLOAT, R_FLOAT, R_FLOAT}, &Vector);
    insert(143, "SetFacingPoint", R_VOID, {R_VECTOR}, &SetFacingPoint);
    insert(144, "AngleToVector", R_VECTOR, {R_FLOAT}, &AngleToVector);
    insert(145, "VectorToAngle", R_FLOAT, {R_VECTOR}, &VectorToAngle);
    insert(146, "TouchAttackMelee", R_INT, {R_OBJECT, R_INT}, &TouchAttackMelee);
    insert(147, "TouchAttackRanged", R_INT, {R_OBJECT, R_INT}, &TouchAttackRanged);
    insert(150, "SetItemStackSize", R_VOID, {R_OBJECT, R_INT}, &SetItemStackSize);
    insert(151, "GetDistanceBetween", R_FLOAT, {R_OBJECT, R_OBJECT}, &GetDistanceBetween);
    insert(152, "SetReturnStrref", R_VOID, {R_INT, R_INT, R_INT}, &SetReturnStrref);
    insert(155, "GetItemInSlot", R_OBJECT, {R_INT, R_OBJECT}, &GetItemInSlot);
    insert(160, "SetGlobalString", R_VOID, {R_STRING, R_STRING}, &SetGlobalString);
    insert(162, "SetCommandable", R_VOID, {R_INT, R_OBJECT}, &SetCommandable);
    insert(163, "GetCommandable", R_INT, {R_OBJECT}, &GetCommandable);
    insert(166, "GetHitDice", R_INT, {R_OBJECT}, &GetHitDice);
    insert(168, "GetTag", R_STRING, {R_OBJECT}, &GetTag);
    insert(169, "ResistForce", R_INT, {R_OBJECT, R_OBJECT}, &ResistForce);
    insert(170, "GetEffectType", R_INT, {R_EFFECT}, &GetEffectType);
    insert(172, "GetFactionEqual", R_INT, {R_OBJECT, R_OBJECT}, &GetFactionEqual);
    insert(173, "ChangeFaction", R_VOID, {R_OBJECT, R_OBJECT}, &ChangeFaction);
    insert(174, "GetIsListening", R_INT, {R_OBJECT}, &GetIsListening);
    insert(175, "SetListening", R_VOID, {R_OBJECT, R_INT}, &SetListening);
    insert(176, "SetListenPattern", R_VOID, {R_OBJECT, R_STRING, R_INT}, &SetListenPattern);
    insert(177, "TestStringAgainstPattern", R_INT, {R_STRING, R_STRING}, &TestStringAgainstPattern);
    insert(178, "GetMatchedSubstring", R_STRING, {R_INT}, &GetMatchedSubstring);
    insert(179, "GetMatchedSubstringsCount", R_INT, {}, &GetMatchedSubstringsCount);
    insert(181, "GetFactionWeakestMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionWeakestMember);
    insert(182, "GetFactionStrongestMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionStrongestMember);
    insert(183, "GetFactionMostDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionMostDamagedMember);
    insert(184, "GetFactionLeastDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionLeastDamagedMember);
    insert(185, "GetFactionGold", R_INT, {R_OBJECT}, &GetFactionGold);
    insert(186, "GetFactionAverageReputation", R_INT, {R_OBJECT, R_OBJECT}, &GetFactionAverageReputation);
    insert(187, "GetFactionAverageGoodEvilAlignment", R_INT, {R_OBJECT}, &GetFactionAverageGoodEvilAlignment);
    insert(188, "SoundObjectGetFixedVariance", R_FLOAT, {R_OBJECT}, &SoundObjectGetFixedVariance);
    insert(189, "GetFactionAverageLevel", R_INT, {R_OBJECT}, &GetFactionAverageLevel);
    insert(190, "GetFactionAverageXP", R_INT, {R_OBJECT}, &GetFactionAverageXP);
    insert(191, "GetFactionMostFrequentClass", R_INT, {R_OBJECT}, &GetFactionMostFrequentClass);
    insert(192, "GetFactionWorstAC", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionWorstAC);
    insert(193, "GetFactionBestAC", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionBestAC);
    insert(194, "GetGlobalString", R_STRING, {R_STRING}, &GetGlobalString);
    insert(195, "GetListenPatternNumber", R_INT, {}, &GetListenPatternNumber);
    insert(197, "GetWaypointByTag", R_OBJECT, {R_STRING}, &GetWaypointByTag);
    insert(198, "GetTransitionTarget", R_OBJECT, {R_OBJECT}, &GetTransitionTarget);
    insert(200, "GetObjectByTag", R_OBJECT, {R_STRING, R_INT}, &GetObjectByTag);
    insert(201, "AdjustAlignment", R_VOID, {R_OBJECT, R_INT, R_INT}, &AdjustAlignment);
    insert(203, "SetAreaTransitionBMP", R_VOID, {R_INT, R_STRING}, &SetAreaTransitionBMP);
    insert(208, "GetReputation", R_INT, {R_OBJECT, R_OBJECT}, &GetReputation);
    insert(209, "AdjustReputation", R_VOID, {R_OBJECT, R_OBJECT, R_INT}, &AdjustReputation);
    insert(210, "GetModuleFileName", R_STRING, {}, &GetModuleFileName);
    insert(211, "GetGoingToBeAttackedBy", R_OBJECT, {R_OBJECT}, &GetGoingToBeAttackedBy);
    insert(213, "GetLocation", R_LOCATION, {R_OBJECT}, &GetLocation);
    insert(215, "Location", R_LOCATION, {R_VECTOR, R_FLOAT}, &CreateLocation);
    insert(216, "ApplyEffectAtLocation", R_VOID, {R_INT, R_EFFECT, R_LOCATION, R_FLOAT}, &ApplyEffectAtLocation);
    insert(217, "GetIsPC", R_INT, {R_OBJECT}, &GetIsPC);
    insert(218, "FeetToMeters", R_FLOAT, {R_FLOAT}, &FeetToMeters);
    insert(219, "YardsToMeters", R_FLOAT, {R_FLOAT}, &YardsToMeters);
    insert(220, "ApplyEffectToObject", R_VOID, {R_INT, R_EFFECT, R_OBJECT, R_FLOAT}, &ApplyEffectToObject);
    insert(221, "SpeakString", R_VOID, {R_STRING, R_INT}, &SpeakString);
    insert(222, "GetSpellTargetLocation", R_LOCATION, {}, &GetSpellTargetLocation);
    insert(223, "GetPositionFromLocation", R_VECTOR, {R_LOCATION}, &GetPositionFromLocation);
    insert(225, "GetFacingFromLocation", R_FLOAT, {R_LOCATION}, &GetFacingFromLocation);
    insert(226, "GetNearestCreatureToLocation", R_OBJECT, {R_INT, R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT, R_INT}, &GetNearestCreatureToLocation);
    insert(227, "GetNearestObject", R_OBJECT, {R_INT, R_OBJECT, R_INT}, &GetNearestObject);
    insert(228, "GetNearestObjectToLocation", R_OBJECT, {R_INT, R_LOCATION, R_INT}, &GetNearestObjectToLocation);
    insert(229, "GetNearestObjectByTag", R_OBJECT, {R_STRING, R_OBJECT, R_INT}, &GetNearestObjectByTag);
    insert(230, "IntToFloat", R_FLOAT, {R_INT}, &IntToFloat);
    insert(231, "FloatToInt", R_INT, {R_FLOAT}, &FloatToInt);
    insert(232, "StringToInt", R_INT, {R_STRING}, &StringToInt);
    insert(233, "StringToFloat", R_FLOAT, {R_STRING}, &StringToFloat);
    insert(235, "GetIsEnemy", R_INT, {R_OBJECT, R_OBJECT}, &GetIsEnemy);
    insert(236, "GetIsFriend", R_INT, {R_OBJECT, R_OBJECT}, &GetIsFriend);
    insert(237, "GetIsNeutral", R_INT, {R_OBJECT, R_OBJECT}, &GetIsNeutral);
    insert(238, "GetPCSpeaker", R_OBJECT, {}, &GetPCSpeaker);
    insert(239, "GetStringByStrRef", R_STRING, {R_INT}, &GetStringByStrRef);
    insert(241, "DestroyObject", R_VOID, {R_OBJECT, R_FLOAT, R_INT, R_FLOAT}, &DestroyObject);
    insert(242, "GetModule", R_OBJECT, {}, &GetModule);
    insert(243, "CreateObject", R_OBJECT, {R_INT, R_STRING, R_LOCATION, R_INT}, &CreateObject);
    insert(244, "EventSpellCastAt", R_EVENT, {R_OBJECT, R_INT, R_INT}, &EventSpellCastAt);
    insert(245, "GetLastSpellCaster", R_OBJECT, {}, &GetLastSpellCaster);
    insert(246, "GetLastSpell", R_INT, {}, &GetLastSpell);
    insert(247, "GetUserDefinedEventNumber", R_INT, {}, &GetUserDefinedEventNumber);
    insert(248, "GetSpellId", R_INT, {}, &GetSpellId);
    insert(249, "RandomName", R_STRING, {}, &RandomName);
    insert(251, "GetLoadFromSaveGame", R_INT, {}, &GetLoadFromSaveGame);
    insert(253, "GetName", R_STRING, {R_OBJECT}, &GetName);
    insert(254, "GetLastSpeaker", R_OBJECT, {}, &GetLastSpeaker);
    insert(255, "BeginConversation", R_INT, {R_STRING, R_OBJECT}, &BeginConversation);
    insert(256, "GetLastPerceived", R_OBJECT, {}, &GetLastPerceived);
    insert(257, "GetLastPerceptionHeard", R_INT, {}, &GetLastPerceptionHeard);
    insert(258, "GetLastPerceptionInaudible", R_INT, {}, &GetLastPerceptionInaudible);
    insert(259, "GetLastPerceptionSeen", R_INT, {}, &GetLastPerceptionSeen);
    insert(260, "GetLastClosedBy", R_OBJECT, {}, &GetLastClosedBy);
    insert(261, "GetLastPerceptionVanished", R_INT, {}, &GetLastPerceptionVanished);
    insert(262, "GetFirstInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &GetFirstInPersistentObject);
    insert(263, "GetNextInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &GetNextInPersistentObject);
    insert(264, "GetAreaOfEffectCreator", R_OBJECT, {R_OBJECT}, &GetAreaOfEffectCreator);
    insert(265, "ShowLevelUpGUI", R_INT, {}, &ShowLevelUpGUI);
    insert(266, "SetItemNonEquippable", R_VOID, {R_OBJECT, R_INT}, &SetItemNonEquippable);
    insert(267, "GetButtonMashCheck", R_INT, {}, &GetButtonMashCheck);
    insert(268, "SetButtonMashCheck", R_VOID, {R_INT}, &SetButtonMashCheck);
    insert(271, "GiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &GiveItem);
    insert(272, "ObjectToString", R_STRING, {R_OBJECT}, &ObjectToString);
    insert(274, "GetIsImmune", R_INT, {R_OBJECT, R_INT, R_OBJECT}, &GetIsImmune);
    insert(276, "GetEncounterActive", R_INT, {R_OBJECT}, &GetEncounterActive);
    insert(277, "SetEncounterActive", R_VOID, {R_INT, R_OBJECT}, &SetEncounterActive);
    insert(278, "GetEncounterSpawnsMax", R_INT, {R_OBJECT}, &GetEncounterSpawnsMax);
    insert(279, "SetEncounterSpawnsMax", R_VOID, {R_INT, R_OBJECT}, &SetEncounterSpawnsMax);
    insert(280, "GetEncounterSpawnsCurrent", R_INT, {R_OBJECT}, &GetEncounterSpawnsCurrent);
    insert(281, "SetEncounterSpawnsCurrent", R_VOID, {R_INT, R_OBJECT}, &SetEncounterSpawnsCurrent);
    insert(282, "GetModuleItemAcquired", R_OBJECT, {}, &GetModuleItemAcquired);
    insert(283, "GetModuleItemAcquiredFrom", R_OBJECT, {}, &GetModuleItemAcquiredFrom);
    insert(284, "SetCustomToken", R_VOID, {R_INT, R_STRING}, &SetCustomToken);
    insert(285, "GetHasFeat", R_INT, {R_INT, R_OBJECT}, &GetHasFeat);
    insert(286, "GetHasSkill", R_INT, {R_INT, R_OBJECT}, &GetHasSkill);
    insert(289, "GetObjectSeen", R_INT, {R_OBJECT, R_OBJECT}, &GetObjectSeen);
    insert(290, "GetObjectHeard", R_INT, {R_OBJECT, R_OBJECT}, &GetObjectHeard);
    insert(291, "GetLastPlayerDied", R_OBJECT, {}, &GetLastPlayerDied);
    insert(292, "GetModuleItemLost", R_OBJECT, {}, &GetModuleItemLost);
    insert(293, "GetModuleItemLostBy", R_OBJECT, {}, &GetModuleItemLostBy);
    insert(295, "EventConversation", R_EVENT, {}, &EventConversation);
    insert(296, "SetEncounterDifficulty", R_VOID, {R_INT, R_OBJECT}, &SetEncounterDifficulty);
    insert(297, "GetEncounterDifficulty", R_INT, {R_OBJECT}, &GetEncounterDifficulty);
    insert(298, "GetDistanceBetweenLocations", R_FLOAT, {R_LOCATION, R_LOCATION}, &GetDistanceBetweenLocations);
    insert(299, "GetReflexAdjustedDamage", R_INT, {R_INT, R_OBJECT, R_INT, R_INT, R_OBJECT}, &GetReflexAdjustedDamage);
    insert(300, "PlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &PlayAnimation);
    insert(301, "TalentSpell", R_TALENT, {R_INT}, &TalentSpell);
    insert(302, "TalentFeat", R_TALENT, {R_INT}, &TalentFeat);
    insert(303, "TalentSkill", R_TALENT, {R_INT}, &TalentSkill);
    insert(304, "GetHasSpellEffect", R_INT, {R_INT, R_OBJECT}, &GetHasSpellEffect);
    insert(305, "GetEffectSpellId", R_INT, {R_EFFECT}, &GetEffectSpellId);
    insert(306, "GetCreatureHasTalent", R_INT, {R_TALENT, R_OBJECT}, &GetCreatureHasTalent);
    insert(307, "GetCreatureTalentRandom", R_TALENT, {R_INT, R_OBJECT, R_INT}, &GetCreatureTalentRandom);
    insert(308, "GetCreatureTalentBest", R_TALENT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT}, &GetCreatureTalentBest);
    insert(311, "GetGoldPieceValue", R_INT, {R_OBJECT}, &GetGoldPieceValue);
    insert(312, "GetIsPlayableRacialType", R_INT, {R_OBJECT}, &GetIsPlayableRacialType);
    insert(313, "JumpToLocation", R_VOID, {R_LOCATION}, &JumpToLocation);
    insert(315, "GetSkillRank", R_INT, {R_INT, R_OBJECT}, &GetSkillRank);
    insert(316, "GetAttackTarget", R_OBJECT, {R_OBJECT}, &GetAttackTarget);
    insert(317, "GetLastAttackType", R_INT, {R_OBJECT}, &GetLastAttackType);
    insert(318, "GetLastAttackMode", R_INT, {R_OBJECT}, &GetLastAttackMode);
    insert(319, "GetDistanceBetween2D", R_FLOAT, {R_OBJECT, R_OBJECT}, &GetDistanceBetween2D);
    insert(320, "GetIsInCombat", R_INT, {R_OBJECT}, &GetIsInCombat);
    insert(321, "GetLastAssociateCommand", R_INT, {R_OBJECT}, &GetLastAssociateCommand);
    insert(322, "GiveGoldToCreature", R_VOID, {R_OBJECT, R_INT}, &GiveGoldToCreature);
    insert(323, "SetIsDestroyable", R_VOID, {R_INT, R_INT, R_INT}, &SetIsDestroyable);
    insert(324, "SetLocked", R_VOID, {R_OBJECT, R_INT}, &SetLocked);
    insert(325, "GetLocked", R_INT, {R_OBJECT}, &GetLocked);
    insert(326, "GetClickingObject", R_OBJECT, {}, &GetClickingObject);
    insert(327, "SetAssociateListenPatterns", R_VOID, {R_OBJECT}, &SetAssociateListenPatterns);
    insert(328, "GetLastWeaponUsed", R_OBJECT, {R_OBJECT}, &GetLastWeaponUsed);
    insert(330, "GetLastUsedBy", R_OBJECT, {}, &GetLastUsedBy);
    insert(331, "GetAbilityModifier", R_INT, {R_INT, R_OBJECT}, &GetAbilityModifier);
    insert(332, "GetIdentified", R_INT, {R_OBJECT}, &GetIdentified);
    insert(333, "SetIdentified", R_VOID, {R_OBJECT, R_INT}, &SetIdentified);
    insert(334, "GetDistanceBetweenLocations2D", R_FLOAT, {R_LOCATION, R_LOCATION}, &GetDistanceBetweenLocations2D);
    insert(335, "GetDistanceToObject2D", R_FLOAT, {R_OBJECT}, &GetDistanceToObject2D);
    insert(336, "GetBlockingDoor", R_OBJECT, {}, &GetBlockingDoor);
    insert(337, "GetIsDoorActionPossible", R_INT, {R_OBJECT, R_INT}, &GetIsDoorActionPossible);
    insert(338, "DoDoorAction", R_VOID, {R_OBJECT, R_INT}, &DoDoorAction);
    insert(339, "GetFirstItemInInventory", R_OBJECT, {R_OBJECT}, &GetFirstItemInInventory);
    insert(340, "GetNextItemInInventory", R_OBJECT, {R_OBJECT}, &GetNextItemInInventory);
    insert(341, "GetClassByPosition", R_INT, {R_INT, R_OBJECT}, &GetClassByPosition);
    insert(342, "GetLevelByPosition", R_INT, {R_INT, R_OBJECT}, &GetLevelByPosition);
    insert(343, "GetLevelByClass", R_INT, {R_INT, R_OBJECT}, &GetLevelByClass);
    insert(344, "GetDamageDealtByType", R_INT, {R_INT}, &GetDamageDealtByType);
    insert(345, "GetTotalDamageDealt", R_INT, {}, &GetTotalDamageDealt);
    insert(346, "GetLastDamager", R_OBJECT, {}, &GetLastDamager);
    insert(347, "GetLastDisarmed", R_OBJECT, {}, &GetLastDisarmed);
    insert(348, "GetLastDisturbed", R_OBJECT, {}, &GetLastDisturbed);
    insert(349, "GetLastLocked", R_OBJECT, {}, &GetLastLocked);
    insert(350, "GetLastUnlocked", R_OBJECT, {}, &GetLastUnlocked);
    insert(352, "GetInventoryDisturbType", R_INT, {}, &GetInventoryDisturbType);
    insert(353, "GetInventoryDisturbItem", R_OBJECT, {}, &GetInventoryDisturbItem);
    insert(354, "ShowUpgradeScreen", R_VOID, {R_OBJECT}, &ShowUpgradeScreen);
    insert(355, "VersusAlignmentEffect", R_EFFECT, {R_EFFECT, R_INT, R_INT}, &VersusAlignmentEffect);
    insert(356, "VersusRacialTypeEffect", R_EFFECT, {R_EFFECT, R_INT}, &VersusRacialTypeEffect);
    insert(357, "VersusTrapEffect", R_EFFECT, {R_EFFECT}, &VersusTrapEffect);
    insert(358, "GetGender", R_INT, {R_OBJECT}, &GetGender);
    insert(359, "GetIsTalentValid", R_INT, {R_TALENT}, &GetIsTalentValid);
    insert(361, "GetAttemptedAttackTarget", R_OBJECT, {}, &GetAttemptedAttackTarget);
    insert(362, "GetTypeFromTalent", R_INT, {R_TALENT}, &GetTypeFromTalent);
    insert(363, "GetIdFromTalent", R_INT, {R_TALENT}, &GetIdFromTalent);
    insert(364, "PlayPazaak", R_VOID, {R_INT, R_STRING, R_INT, R_INT, R_OBJECT}, &PlayPazaak);
    insert(365, "GetLastPazaakResult", R_INT, {}, &GetLastPazaakResult);
    insert(366, "DisplayFeedBackText", R_VOID, {R_OBJECT, R_INT}, &DisplayFeedBackText);
    insert(367, "AddJournalQuestEntry", R_VOID, {R_STRING, R_INT, R_INT}, &AddJournalQuestEntry);
    insert(368, "RemoveJournalQuestEntry", R_VOID, {R_STRING}, &RemoveJournalQuestEntry);
    insert(369, "GetJournalEntry", R_INT, {R_STRING}, &GetJournalEntry);
    insert(370, "PlayRumblePattern", R_INT, {R_INT}, &PlayRumblePattern);
    insert(371, "StopRumblePattern", R_INT, {R_INT}, &StopRumblePattern);
    insert(374, "SendMessageToPC", R_VOID, {R_OBJECT, R_STRING}, &SendMessageToPC);
    insert(375, "GetAttemptedSpellTarget", R_OBJECT, {}, &GetAttemptedSpellTarget);
    insert(376, "GetLastOpenedBy", R_OBJECT, {}, &GetLastOpenedBy);
    insert(377, "GetHasSpell", R_INT, {R_INT, R_OBJECT}, &GetHasSpell);
    insert(378, "OpenStore", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &OpenStore);
    insert(380, "GetFirstFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFirstFactionMember);
    insert(381, "GetNextFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &GetNextFactionMember);
    insert(384, "GetJournalQuestExperience", R_INT, {R_STRING}, &GetJournalQuestExperience);
    insert(385, "JumpToObject", R_VOID, {R_OBJECT, R_INT}, &JumpToObject);
    insert(386, "SetMapPinEnabled", R_VOID, {R_OBJECT, R_INT}, &SetMapPinEnabled);
    insert(388, "PopUpGUIPanel", R_VOID, {R_OBJECT, R_INT}, &PopUpGUIPanel);
    insert(389, "AddMultiClass", R_VOID, {R_INT, R_OBJECT}, &AddMultiClass);
    insert(390, "GetIsLinkImmune", R_INT, {R_OBJECT, R_EFFECT}, &GetIsLinkImmune);
    insert(393, "GiveXPToCreature", R_VOID, {R_OBJECT, R_INT}, &GiveXPToCreature);
    insert(394, "SetXP", R_VOID, {R_OBJECT, R_INT}, &SetXP);
    insert(395, "GetXP", R_INT, {R_OBJECT}, &GetXP);
    insert(396, "IntToHexString", R_STRING, {R_INT}, &IntToHexString);
    insert(397, "GetBaseItemType", R_INT, {R_OBJECT}, &GetBaseItemType);
    insert(398, "GetItemHasItemProperty", R_INT, {R_OBJECT, R_INT}, &GetItemHasItemProperty);
    insert(401, "GetItemACValue", R_INT, {R_OBJECT}, &GetItemACValue);
    insert(403, "ExploreAreaForPlayer", R_VOID, {R_OBJECT, R_OBJECT}, &ExploreAreaForPlayer);
    insert(405, "GetIsDay", R_INT, {}, &GetIsDay);
    insert(406, "GetIsNight", R_INT, {}, &GetIsNight);
    insert(407, "GetIsDawn", R_INT, {}, &GetIsDawn);
    insert(408, "GetIsDusk", R_INT, {}, &GetIsDusk);
    insert(409, "GetIsEncounterCreature", R_INT, {R_OBJECT}, &GetIsEncounterCreature);
    insert(410, "GetLastPlayerDying", R_OBJECT, {}, &GetLastPlayerDying);
    insert(411, "GetStartingLocation", R_LOCATION, {}, &GetStartingLocation);
    insert(412, "ChangeToStandardFaction", R_VOID, {R_OBJECT, R_INT}, &ChangeToStandardFaction);
    insert(413, "SoundObjectPlay", R_VOID, {R_OBJECT}, &SoundObjectPlay);
    insert(414, "SoundObjectStop", R_VOID, {R_OBJECT}, &SoundObjectStop);
    insert(415, "SoundObjectSetVolume", R_VOID, {R_OBJECT, R_INT}, &SoundObjectSetVolume);
    insert(416, "SoundObjectSetPosition", R_VOID, {R_OBJECT, R_VECTOR}, &SoundObjectSetPosition);
    insert(417, "SpeakOneLinerConversation", R_VOID, {R_STRING, R_OBJECT}, &SpeakOneLinerConversation);
    insert(418, "GetGold", R_INT, {R_OBJECT}, &GetGold);
    insert(419, "GetLastRespawnButtonPresser", R_OBJECT, {}, &GetLastRespawnButtonPresser);
    insert(421, "SetLightsaberPowered", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &SetLightsaberPowered);
    insert(422, "GetIsWeaponEffective", R_INT, {R_OBJECT, R_INT}, &GetIsWeaponEffective);
    insert(423, "GetLastSpellHarmful", R_INT, {}, &GetLastSpellHarmful);
    insert(424, "EventActivateItem", R_EVENT, {R_OBJECT, R_LOCATION, R_OBJECT}, &EventActivateItem);
    insert(425, "MusicBackgroundPlay", R_VOID, {R_OBJECT}, &MusicBackgroundPlay);
    insert(426, "MusicBackgroundStop", R_VOID, {R_OBJECT}, &MusicBackgroundStop);
    insert(427, "MusicBackgroundSetDelay", R_VOID, {R_OBJECT, R_INT}, &MusicBackgroundSetDelay);
    insert(428, "MusicBackgroundChangeDay", R_VOID, {R_OBJECT, R_INT}, &MusicBackgroundChangeDay);
    insert(429, "MusicBackgroundChangeNight", R_VOID, {R_OBJECT, R_INT}, &MusicBackgroundChangeNight);
    insert(430, "MusicBattlePlay", R_VOID, {R_OBJECT}, &MusicBattlePlay);
    insert(431, "MusicBattleStop", R_VOID, {R_OBJECT}, &MusicBattleStop);
    insert(432, "MusicBattleChange", R_VOID, {R_OBJECT, R_INT}, &MusicBattleChange);
    insert(433, "AmbientSoundPlay", R_VOID, {R_OBJECT}, &AmbientSoundPlay);
    insert(434, "AmbientSoundStop", R_VOID, {R_OBJECT}, &AmbientSoundStop);
    insert(435, "AmbientSoundChangeDay", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundChangeDay);
    insert(436, "AmbientSoundChangeNight", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundChangeNight);
    insert(437, "GetLastKiller", R_OBJECT, {}, &GetLastKiller);
    insert(438, "GetSpellCastItem", R_OBJECT, {}, &GetSpellCastItem);
    insert(439, "GetItemActivated", R_OBJECT, {}, &GetItemActivated);
    insert(440, "GetItemActivator", R_OBJECT, {}, &GetItemActivator);
    insert(441, "GetItemActivatedTargetLocation", R_LOCATION, {}, &GetItemActivatedTargetLocation);
    insert(442, "GetItemActivatedTarget", R_OBJECT, {}, &GetItemActivatedTarget);
    insert(443, "GetIsOpen", R_INT, {R_OBJECT}, &GetIsOpen);
    insert(444, "TakeGoldFromCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &TakeGoldFromCreature);
    insert(445, "GetIsInConversation", R_INT, {R_OBJECT}, &GetIsInConversation);
    insert(455, "GetPlotFlag", R_INT, {R_OBJECT}, &GetPlotFlag);
    insert(456, "SetPlotFlag", R_VOID, {R_OBJECT, R_INT}, &SetPlotFlag);
    insert(461, "SetDialogPlaceableCamera", R_VOID, {R_INT}, &SetDialogPlaceableCamera);
    insert(462, "GetSoloMode", R_INT, {}, &GetSoloMode);
    insert(464, "GetMaxStealthXP", R_INT, {}, &GetMaxStealthXP);
    insert(468, "SetMaxStealthXP", R_VOID, {R_INT}, &SetMaxStealthXP);
    insert(474, "GetCurrentStealthXP", R_INT, {}, &GetCurrentStealthXP);
    insert(475, "GetNumStackedItems", R_INT, {R_OBJECT}, &GetNumStackedItems);
    insert(476, "SurrenderToEnemies", R_VOID, {}, &SurrenderToEnemies);
    insert(478, "SetCurrentStealthXP", R_VOID, {R_INT}, &SetCurrentStealthXP);
    insert(479, "GetCreatureSize", R_INT, {R_OBJECT}, &GetCreatureSize);
    insert(480, "AwardStealthXP", R_VOID, {R_OBJECT}, &AwardStealthXP);
    insert(481, "GetStealthXPEnabled", R_INT, {}, &GetStealthXPEnabled);
    insert(482, "SetStealthXPEnabled", R_VOID, {R_INT}, &SetStealthXPEnabled);
    insert(486, "GetLastTrapDetected", R_OBJECT, {R_OBJECT}, &GetLastTrapDetected);
    insert(488, "GetNearestTrapToObject", R_OBJECT, {R_OBJECT, R_INT}, &GetNearestTrapToObject);
    insert(489, "GetAttemptedMovementTarget", R_OBJECT, {}, &GetAttemptedMovementTarget);
    insert(490, "GetBlockingCreature", R_OBJECT, {R_OBJECT}, &GetBlockingCreature);
    insert(491, "GetFortitudeSavingThrow", R_INT, {R_OBJECT}, &GetFortitudeSavingThrow);
    insert(492, "GetWillSavingThrow", R_INT, {R_OBJECT}, &GetWillSavingThrow);
    insert(493, "GetReflexSavingThrow", R_INT, {R_OBJECT}, &GetReflexSavingThrow);
    insert(494, "GetChallengeRating", R_FLOAT, {R_OBJECT}, &GetChallengeRating);
    insert(495, "GetFoundEnemyCreature", R_OBJECT, {R_OBJECT}, &GetFoundEnemyCreature);
    insert(496, "GetMovementRate", R_INT, {R_OBJECT}, &GetMovementRate);
    insert(497, "GetSubRace", R_INT, {R_OBJECT}, &GetSubRace);
    insert(498, "GetStealthXPDecrement", R_INT, {}, &GetStealthXPDecrement);
    insert(499, "SetStealthXPDecrement", R_VOID, {R_INT}, &SetStealthXPDecrement);
    insert(500, "DuplicateHeadAppearance", R_VOID, {R_OBJECT, R_OBJECT}, &DuplicateHeadAppearance);
    insert(503, "CutsceneAttack", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &CutsceneAttack);
    insert(504, "SetCameraMode", R_VOID, {R_OBJECT, R_INT}, &SetCameraMode);
    insert(505, "SetLockOrientationInDialog", R_VOID, {R_OBJECT, R_INT}, &SetLockOrientationInDialog);
    insert(506, "SetLockHeadFollowInDialog", R_VOID, {R_OBJECT, R_INT}, &SetLockHeadFollowInDialog);
    insert(507, "CutsceneMove", R_VOID, {R_OBJECT, R_VECTOR, R_INT}, &CutsceneMove);
    insert(508, "EnableVideoEffect", R_VOID, {R_INT}, &EnableVideoEffect);
    insert(509, "StartNewModule", R_VOID, {R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING}, &StartNewModule);
    insert(510, "DisableVideoEffect", R_VOID, {}, &DisableVideoEffect);
    insert(511, "GetWeaponRanged", R_INT, {R_OBJECT}, &GetWeaponRanged);
    insert(512, "DoSinglePlayerAutoSave", R_VOID, {}, &DoSinglePlayerAutoSave);
    insert(513, "GetGameDifficulty", R_INT, {}, &GetGameDifficulty);
    insert(514, "GetUserActionsPending", R_INT, {}, &GetUserActionsPending);
    insert(515, "RevealMap", R_VOID, {R_VECTOR, R_INT}, &RevealMap);
    insert(516, "SetTutorialWindowsEnabled", R_VOID, {R_INT}, &SetTutorialWindowsEnabled);
    insert(517, "ShowTutorialWindow", R_VOID, {R_INT}, &ShowTutorialWindow);
    insert(518, "StartCreditSequence", R_VOID, {R_INT}, &StartCreditSequence);
    insert(519, "IsCreditSequenceInProgress", R_INT, {}, &IsCreditSequenceInProgress);
    insert(522, "GetCurrentAction", R_INT, {R_OBJECT}, &GetCurrentAction);
    insert(523, "GetDifficultyModifier", R_FLOAT, {}, &GetDifficultyModifier);
    insert(524, "GetAppearanceType", R_INT, {R_OBJECT}, &GetAppearanceType);
    insert(525, "FloatingTextStrRefOnCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &FloatingTextStrRefOnCreature);
    insert(526, "FloatingTextStringOnCreature", R_VOID, {R_STRING, R_OBJECT, R_INT}, &FloatingTextStringOnCreature);
    insert(527, "GetTrapDisarmable", R_INT, {R_OBJECT}, &GetTrapDisarmable);
    insert(528, "GetTrapDetectable", R_INT, {R_OBJECT}, &GetTrapDetectable);
    insert(529, "GetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &GetTrapDetectedBy);
    insert(530, "GetTrapFlagged", R_INT, {R_OBJECT}, &GetTrapFlagged);
    insert(531, "GetTrapBaseType", R_INT, {R_OBJECT}, &GetTrapBaseType);
    insert(532, "GetTrapOneShot", R_INT, {R_OBJECT}, &GetTrapOneShot);
    insert(533, "GetTrapCreator", R_OBJECT, {R_OBJECT}, &GetTrapCreator);
    insert(534, "GetTrapKeyTag", R_STRING, {R_OBJECT}, &GetTrapKeyTag);
    insert(535, "GetTrapDisarmDC", R_INT, {R_OBJECT}, &GetTrapDisarmDC);
    insert(536, "GetTrapDetectDC", R_INT, {R_OBJECT}, &GetTrapDetectDC);
    insert(537, "GetLockKeyRequired", R_INT, {R_OBJECT}, &GetLockKeyRequired);
    insert(538, "GetLockKeyTag", R_INT, {R_OBJECT}, &GetLockKeyTag);
    insert(539, "GetLockLockable", R_INT, {R_OBJECT}, &GetLockLockable);
    insert(540, "GetLockUnlockDC", R_INT, {R_OBJECT}, &GetLockUnlockDC);
    insert(541, "GetLockLockDC", R_INT, {R_OBJECT}, &GetLockLockDC);
    insert(542, "GetPCLevellingUp", R_OBJECT, {}, &GetPCLevellingUp);
    insert(543, "GetHasFeatEffect", R_INT, {R_INT, R_OBJECT}, &GetHasFeatEffect);
    insert(544, "SetPlaceableIllumination", R_VOID, {R_OBJECT, R_INT}, &SetPlaceableIllumination);
    insert(545, "GetPlaceableIllumination", R_INT, {R_OBJECT}, &GetPlaceableIllumination);
    insert(546, "GetIsPlaceableObjectActionPossible", R_INT, {R_OBJECT, R_INT}, &GetIsPlaceableObjectActionPossible);
    insert(547, "DoPlaceableObjectAction", R_VOID, {R_OBJECT, R_INT}, &DoPlaceableObjectAction);
    insert(548, "GetFirstPC", R_OBJECT, {}, &GetFirstPC);
    insert(549, "GetNextPC", R_OBJECT, {}, &GetNextPC);
    insert(550, "SetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &SetTrapDetectedBy);
    insert(551, "GetIsTrapped", R_INT, {R_OBJECT}, &GetIsTrapped);
    insert(552, "SetEffectIcon", R_EFFECT, {R_EFFECT, R_INT}, &SetEffectIcon);
    insert(553, "FaceObjectAwayFromObject", R_VOID, {R_OBJECT, R_OBJECT}, &FaceObjectAwayFromObject);
    insert(554, "PopUpDeathGUIPanel", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT, R_STRING}, &PopUpDeathGUIPanel);
    insert(555, "SetTrapDisabled", R_VOID, {R_OBJECT}, &SetTrapDisabled);
    insert(556, "GetLastHostileActor", R_OBJECT, {R_OBJECT}, &GetLastHostileActor);
    insert(557, "ExportAllCharacters", R_VOID, {}, &ExportAllCharacters);
    insert(558, "MusicBackgroundGetDayTrack", R_INT, {R_OBJECT}, &MusicBackgroundGetDayTrack);
    insert(559, "MusicBackgroundGetNightTrack", R_INT, {R_OBJECT}, &MusicBackgroundGetNightTrack);
    insert(560, "WriteTimestampedLogEntry", R_VOID, {R_STRING}, &WriteTimestampedLogEntry);
    insert(561, "GetModuleName", R_STRING, {}, &GetModuleName);
    insert(562, "GetFactionLeader", R_OBJECT, {R_OBJECT}, &GetFactionLeader);
    insert(564, "EndGame", R_VOID, {R_INT}, &EndGame);
    insert(565, "GetRunScriptVar", R_INT, {}, &GetRunScriptVar);
    insert(566, "GetCreatureMovmentType", R_INT, {R_OBJECT}, &GetCreatureMovmentType);
    insert(567, "AmbientSoundSetDayVolume", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundSetDayVolume);
    insert(568, "AmbientSoundSetNightVolume", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundSetNightVolume);
    insert(569, "MusicBackgroundGetBattleTrack", R_INT, {R_OBJECT}, &MusicBackgroundGetBattleTrack);
    insert(570, "GetHasInventory", R_INT, {R_OBJECT}, &GetHasInventory);
    insert(571, "GetStrRefSoundDuration", R_FLOAT, {R_INT}, &GetStrRefSoundDuration);
    insert(572, "AddToParty", R_VOID, {R_OBJECT, R_OBJECT}, &AddToParty);
    insert(573, "RemoveFromParty", R_VOID, {R_OBJECT}, &RemoveFromParty);
    insert(574, "AddPartyMember", R_INT, {R_INT, R_OBJECT}, &AddPartyMember);
    insert(575, "RemovePartyMember", R_INT, {R_INT}, &RemovePartyMember);
    insert(576, "IsObjectPartyMember", R_INT, {R_OBJECT}, &IsObjectPartyMember);
    insert(577, "GetPartyMemberByIndex", R_OBJECT, {R_INT}, &GetPartyMemberByIndex);
    insert(578, "GetGlobalBoolean", R_INT, {R_STRING}, &GetGlobalBoolean);
    insert(579, "SetGlobalBoolean", R_VOID, {R_STRING, R_INT}, &SetGlobalBoolean);
    insert(580, "GetGlobalNumber", R_INT, {R_STRING}, &GetGlobalNumber);
    insert(581, "SetGlobalNumber", R_VOID, {R_STRING, R_INT}, &SetGlobalNumber);
    insert(582, "AurPostString", R_VOID, {R_STRING, R_INT, R_INT, R_FLOAT}, &AurPostString);
    insert(669, "AddJournalWorldEntry", R_VOID, {R_INT, R_STRING, R_STRING}, &AddJournalWorldEntry);
    insert(670, "AddJournalWorldEntryStrref", R_VOID, {R_INT, R_INT}, &AddJournalWorldEntryStrref);
    insert(671, "BarkString", R_VOID, {R_OBJECT, R_INT}, &BarkString);
    insert(672, "DeleteJournalWorldAllEntries", R_VOID, {}, &DeleteJournalWorldAllEntries);
    insert(673, "DeleteJournalWorldEntry", R_VOID, {R_INT}, &DeleteJournalWorldEntry);
    insert(674, "DeleteJournalWorldEntryStrref", R_VOID, {R_INT}, &DeleteJournalWorldEntryStrref);
    insert(677, "PlayVisualAreaEffect", R_VOID, {R_INT, R_LOCATION}, &PlayVisualAreaEffect);
    insert(678, "SetJournalQuestEntryPicture", R_VOID, {R_STRING, R_OBJECT, R_INT, R_INT, R_INT}, &SetJournalQuestEntryPicture);
    insert(679, "GetLocalBoolean", R_INT, {R_OBJECT, R_INT}, &GetLocalBoolean);
    insert(680, "SetLocalBoolean", R_VOID, {R_OBJECT, R_INT, R_INT}, &SetLocalBoolean);
    insert(681, "GetLocalNumber", R_INT, {R_OBJECT, R_INT}, &GetLocalNumber);
    insert(682, "SetLocalNumber", R_VOID, {R_OBJECT, R_INT, R_INT}, &SetLocalNumber);
    insert(689, "SoundObjectGetPitchVariance", R_FLOAT, {R_OBJECT}, &SoundObjectGetPitchVariance);
    insert(690, "SoundObjectSetPitchVariance", R_VOID, {R_OBJECT, R_FLOAT}, &SoundObjectSetPitchVariance);
    insert(691, "SoundObjectGetVolume", R_INT, {R_OBJECT}, &SoundObjectGetVolume);
    insert(692, "GetGlobalLocation", R_LOCATION, {R_STRING}, &GetGlobalLocation);
    insert(693, "SetGlobalLocation", R_VOID, {R_STRING, R_LOCATION}, &SetGlobalLocation);
    insert(694, "AddAvailableNPCByObject", R_INT, {R_INT, R_OBJECT}, &AddAvailableNPCByObject);
    insert(695, "RemoveAvailableNPC", R_INT, {R_INT}, &RemoveAvailableNPC);
    insert(696, "IsAvailableCreature", R_INT, {R_INT}, &IsAvailableCreature);
    insert(697, "AddAvailableNPCByTemplate", R_INT, {R_INT, R_STRING}, &AddAvailableNPCByTemplate);
    insert(698, "SpawnAvailableNPC", R_OBJECT, {R_INT, R_LOCATION}, &SpawnAvailableNPC);
    insert(699, "IsNPCPartyMember", R_INT, {R_INT}, &IsNPCPartyMember);
    insert(701, "GetIsConversationActive", R_INT, {}, &GetIsConversationActive);
    insert(704, "GetPartyAIStyle", R_INT, {}, &GetPartyAIStyle);
    insert(705, "GetNPCAIStyle", R_INT, {R_OBJECT}, &GetNPCAIStyle);
    insert(706, "SetPartyAIStyle", R_VOID, {R_INT}, &SetPartyAIStyle);
    insert(707, "SetNPCAIStyle", R_VOID, {R_OBJECT, R_INT}, &SetNPCAIStyle);
    insert(708, "SetNPCSelectability", R_VOID, {R_INT, R_INT}, &SetNPCSelectability);
    insert(709, "GetNPCSelectability", R_INT, {R_INT}, &GetNPCSelectability);
    insert(710, "ClearAllEffects", R_VOID, {}, &ClearAllEffects);
    insert(711, "GetLastConversation", R_STRING, {}, &GetLastConversation);
    insert(712, "ShowPartySelectionGUI", R_VOID, {R_STRING, R_INT, R_INT}, &ShowPartySelectionGUI);
    insert(713, "GetStandardFaction", R_INT, {R_OBJECT}, &GetStandardFaction);
    insert(714, "GivePlotXP", R_VOID, {R_STRING, R_INT}, &GivePlotXP);
    insert(715, "GetMinOneHP", R_INT, {R_OBJECT}, &GetMinOneHP);
    insert(716, "SetMinOneHP", R_VOID, {R_OBJECT, R_INT}, &SetMinOneHP);
    insert(719, "SetGlobalFadeIn", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &SetGlobalFadeIn);
    insert(720, "SetGlobalFadeOut", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &SetGlobalFadeOut);
    insert(721, "GetLastHostileTarget", R_OBJECT, {R_OBJECT}, &GetLastHostileTarget);
    insert(722, "GetLastAttackAction", R_INT, {R_OBJECT}, &GetLastAttackAction);
    insert(723, "GetLastForcePowerUsed", R_INT, {R_OBJECT}, &GetLastForcePowerUsed);
    insert(724, "GetLastCombatFeatUsed", R_INT, {R_OBJECT}, &GetLastCombatFeatUsed);
    insert(725, "GetLastAttackResult", R_INT, {R_OBJECT}, &GetLastAttackResult);
    insert(726, "GetWasForcePowerSuccessful", R_INT, {R_OBJECT}, &GetWasForcePowerSuccessful);
    insert(727, "GetFirstAttacker", R_OBJECT, {R_OBJECT}, &GetFirstAttacker);
    insert(728, "GetNextAttacker", R_OBJECT, {R_OBJECT}, &GetNextAttacker);
    insert(729, "SetFormation", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &SetFormation);
    insert(731, "SetForcePowerUnsuccessful", R_VOID, {R_INT, R_OBJECT}, &SetForcePowerUnsuccessful);
    insert(732, "GetIsDebilitated", R_INT, {R_OBJECT}, &GetIsDebilitated);
    insert(733, "PlayMovie", R_VOID, {R_STRING}, &PlayMovie);
    insert(734, "SaveNPCState", R_VOID, {R_INT}, &SaveNPCState);
    insert(735, "GetCategoryFromTalent", R_INT, {R_TALENT}, &GetCategoryFromTalent);
    insert(736, "SurrenderByFaction", R_VOID, {R_INT, R_INT}, &SurrenderByFaction);
    insert(737, "ChangeFactionByFaction", R_VOID, {R_INT, R_INT}, &ChangeFactionByFaction);
    insert(738, "PlayRoomAnimation", R_VOID, {R_STRING, R_INT}, &PlayRoomAnimation);
    insert(739, "ShowGalaxyMap", R_VOID, {R_INT}, &ShowGalaxyMap);
    insert(740, "SetPlanetSelectable", R_VOID, {R_INT, R_INT}, &SetPlanetSelectable);
    insert(741, "GetPlanetSelectable", R_INT, {R_INT}, &GetPlanetSelectable);
    insert(742, "SetPlanetAvailable", R_VOID, {R_INT, R_INT}, &SetPlanetAvailable);
    insert(743, "GetPlanetAvailable", R_INT, {R_INT}, &GetPlanetAvailable);
    insert(744, "GetSelectedPlanet", R_INT, {}, &GetSelectedPlanet);
    insert(745, "SoundObjectFadeAndStop", R_VOID, {R_OBJECT, R_FLOAT}, &SoundObjectFadeAndStop);
    insert(746, "SetAreaFogColor", R_VOID, {R_OBJECT, R_FLOAT, R_FLOAT, R_FLOAT}, &SetAreaFogColor);
    insert(747, "ChangeItemCost", R_VOID, {R_STRING, R_FLOAT}, &ChangeItemCost);
    insert(748, "GetIsLiveContentAvailable", R_INT, {R_INT}, &GetIsLiveContentAvailable);
    insert(749, "ResetDialogState", R_VOID, {}, &ResetDialogState);
    insert(750, "SetGoodEvilValue", R_VOID, {R_OBJECT, R_INT}, &SetGoodEvilValue);
    insert(751, "GetIsPoisoned", R_INT, {R_OBJECT}, &GetIsPoisoned);
    insert(752, "GetSpellTarget", R_OBJECT, {R_OBJECT}, &GetSpellTarget);
    insert(753, "SetSoloMode", R_VOID, {R_INT}, &SetSoloMode);
    insert(757, "CancelPostDialogCharacterSwitch", R_VOID, {}, &CancelPostDialogCharacterSwitch);
    insert(758, "SetMaxHitPoints", R_VOID, {R_OBJECT, R_INT}, &SetMaxHitPoints);
    insert(759, "NoClicksFor", R_VOID, {R_FLOAT}, &NoClicksFor);
    insert(760, "HoldWorldFadeInForDialog", R_VOID, {}, &HoldWorldFadeInForDialog);
    insert(761, "ShipBuild", R_INT, {}, &ShipBuild);
    insert(762, "SurrenderRetainBuffs", R_VOID, {}, &SurrenderRetainBuffs);
    insert(763, "SuppressStatusSummaryEntry", R_VOID, {R_INT}, &SuppressStatusSummaryEntry);
    insert(764, "GetCheatCode", R_INT, {R_INT}, &GetCheatCode);
    insert(765, "SetMusicVolume", R_VOID, {R_FLOAT}, &SetMusicVolume);
    insert(766, "CreateItemOnFloor", R_OBJECT, {R_STRING, R_LOCATION, R_INT}, &CreateItemOnFloor);
    insert(767, "SetAvailableNPCId", R_VOID, {R_INT, R_OBJECT}, &SetAvailableNPCId);
    insert(768, "IsMoviePlaying", R_INT, {}, &IsMoviePlaying);
    insert(769, "QueueMovie", R_VOID, {R_STRING, R_INT}, &QueueMovie);
    insert(770, "PlayMovieQueue", R_VOID, {R_INT}, &PlayMovieQueue);
    insert(771, "YavinHackCloseDoor", R_VOID, {R_OBJECT}, &YavinHackDoorClose);
}

void Routines::registerMainTslRoutines() {
    insert(0, "Random", R_INT, {R_INT}, &Random);
    insert(1, "PrintString", R_VOID, {R_STRING}, &PrintString);
    insert(2, "PrintFloat", R_VOID, {R_FLOAT, R_INT, R_INT}, &PrintFloat);
    insert(3, "FloatToString", R_STRING, {R_FLOAT, R_INT, R_INT}, &FloatToString);
    insert(4, "PrintInteger", R_VOID, {R_INT}, &PrintInteger);
    insert(5, "PrintObject", R_VOID, {R_OBJECT}, &PrintObject);
    insert(6, "AssignCommand", R_VOID, {R_OBJECT, R_ACTION}, &AssignCommand);
    insert(7, "DelayCommand", R_VOID, {R_FLOAT, R_ACTION}, &DelayCommand);
    insert(8, "ExecuteScript", R_VOID, {R_STRING, R_OBJECT, R_INT}, &ExecuteScript);
    insert(9, "ClearAllActions", R_VOID, {}, &ClearAllActions);
    insert(10, "SetFacing", R_VOID, {R_FLOAT}, &SetFacing);
    insert(11, "SwitchPlayerCharacter", R_INT, {R_INT}, &SwitchPlayerCharacter);
    insert(12, "SetTime", R_VOID, {R_INT, R_INT, R_INT, R_INT}, &SetTime);
    insert(13, "SetPartyLeader", R_INT, {R_INT}, &SetPartyLeader);
    insert(14, "SetAreaUnescapable", R_VOID, {R_INT}, &SetAreaUnescapable);
    insert(15, "GetAreaUnescapable", R_INT, {}, &GetAreaUnescapable);
    insert(16, "GetTimeHour", R_INT, {}, &GetTimeHour);
    insert(17, "GetTimeMinute", R_INT, {}, &GetTimeMinute);
    insert(18, "GetTimeSecond", R_INT, {}, &GetTimeSecond);
    insert(19, "GetTimeMillisecond", R_INT, {}, &GetTimeMillisecond);
    insert(24, "GetArea", R_OBJECT, {R_OBJECT}, &GetArea);
    insert(25, "GetEnteringObject", R_OBJECT, {}, &GetEnteringObject);
    insert(26, "GetExitingObject", R_OBJECT, {}, &GetExitingObject);
    insert(27, "GetPosition", R_VECTOR, {R_OBJECT}, &GetPosition);
    insert(28, "GetFacing", R_FLOAT, {R_OBJECT}, &GetFacing);
    insert(29, "GetItemPossessor", R_OBJECT, {R_OBJECT}, &GetItemPossessor);
    insert(30, "GetItemPossessedBy", R_OBJECT, {R_OBJECT, R_STRING}, &GetItemPossessedBy);
    insert(31, "CreateItemOnObject", R_OBJECT, {R_STRING, R_OBJECT, R_INT, R_INT}, &CreateItemOnObject);
    insert(36, "GetLastAttacker", R_OBJECT, {R_OBJECT}, &GetLastAttacker);
    insert(38, "GetNearestCreature", R_OBJECT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &GetNearestCreature);
    insert(41, "GetDistanceToObject", R_FLOAT, {R_OBJECT}, &GetDistanceToObject);
    insert(42, "GetIsObjectValid", R_INT, {R_OBJECT}, &GetIsObjectValid);
    insert(45, "SetCameraFacing", R_VOID, {R_FLOAT}, &SetCameraFacing);
    insert(46, "PlaySound", R_VOID, {R_STRING}, &PlaySound);
    insert(47, "GetSpellTargetObject", R_OBJECT, {}, &GetSpellTargetObject);
    insert(49, "GetCurrentHitPoints", R_INT, {R_OBJECT}, &GetCurrentHitPoints);
    insert(50, "GetMaxHitPoints", R_INT, {R_OBJECT}, &GetMaxHitPoints);
    insert(52, "GetLastItemEquipped", R_OBJECT, {}, &GetLastItemEquipped);
    insert(53, "GetSubScreenID", R_INT, {}, &GetSubScreenID);
    insert(54, "CancelCombat", R_VOID, {R_OBJECT}, &CancelCombat);
    insert(55, "GetCurrentForcePoints", R_INT, {R_OBJECT}, &GetCurrentForcePoints);
    insert(56, "GetMaxForcePoints", R_INT, {R_OBJECT}, &GetMaxForcePoints);
    insert(57, "PauseGame", R_VOID, {R_INT}, &PauseGame);
    insert(58, "SetPlayerRestrictMode", R_VOID, {R_INT}, &SetPlayerRestrictMode);
    insert(59, "GetStringLength", R_INT, {R_STRING}, &GetStringLength);
    insert(60, "GetStringUpperCase", R_STRING, {R_STRING}, &GetStringUpperCase);
    insert(61, "GetStringLowerCase", R_STRING, {R_STRING}, &GetStringLowerCase);
    insert(62, "GetStringRight", R_STRING, {R_STRING, R_INT}, &GetStringRight);
    insert(63, "GetStringLeft", R_STRING, {R_STRING, R_INT}, &GetStringLeft);
    insert(64, "InsertString", R_STRING, {R_STRING, R_STRING, R_INT}, &InsertString);
    insert(65, "GetSubString", R_STRING, {R_STRING, R_INT, R_INT}, &GetSubString);
    insert(66, "FindSubString", R_INT, {R_STRING, R_STRING}, &FindSubString);
    insert(67, "fabs", R_FLOAT, {R_FLOAT}, &fabs);
    insert(68, "cos", R_FLOAT, {R_FLOAT}, &cos);
    insert(69, "sin", R_FLOAT, {R_FLOAT}, &sin);
    insert(70, "tan", R_FLOAT, {R_FLOAT}, &tan);
    insert(71, "acos", R_FLOAT, {R_FLOAT}, &acos);
    insert(72, "asin", R_FLOAT, {R_FLOAT}, &asin);
    insert(73, "atan", R_FLOAT, {R_FLOAT}, &atan);
    insert(74, "log", R_FLOAT, {R_FLOAT}, &log);
    insert(75, "pow", R_FLOAT, {R_FLOAT, R_FLOAT}, &pow);
    insert(76, "sqrt", R_FLOAT, {R_FLOAT}, &sqrt);
    insert(77, "abs", R_INT, {R_INT}, &abs);
    insert(83, "GetPlayerRestrictMode", R_INT, {R_OBJECT}, &GetPlayerRestrictMode);
    insert(84, "GetCasterLevel", R_INT, {R_OBJECT}, &GetCasterLevel);
    insert(85, "GetFirstEffect", R_EFFECT, {R_OBJECT}, &GetFirstEffect);
    insert(86, "GetNextEffect", R_EFFECT, {R_OBJECT}, &GetNextEffect);
    insert(87, "RemoveEffect", R_VOID, {R_OBJECT, R_EFFECT}, &RemoveEffect);
    insert(88, "GetIsEffectValid", R_INT, {R_EFFECT}, &GetIsEffectValid);
    insert(89, "GetEffectDurationType", R_INT, {R_EFFECT}, &GetEffectDurationType);
    insert(90, "GetEffectSubType", R_INT, {R_EFFECT}, &GetEffectSubType);
    insert(91, "GetEffectCreator", R_OBJECT, {R_EFFECT}, &GetEffectCreator);
    insert(92, "IntToString", R_STRING, {R_INT}, &IntToString);
    insert(93, "GetFirstObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &GetFirstObjectInArea);
    insert(94, "GetNextObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &GetNextObjectInArea);
    insert(95, "d2", R_INT, {R_INT}, &d2);
    insert(96, "d3", R_INT, {R_INT}, &d3);
    insert(97, "d4", R_INT, {R_INT}, &d4);
    insert(98, "d6", R_INT, {R_INT}, &d6);
    insert(99, "d8", R_INT, {R_INT}, &d8);
    insert(100, "d10", R_INT, {R_INT}, &d10);
    insert(101, "d12", R_INT, {R_INT}, &d12);
    insert(102, "d20", R_INT, {R_INT}, &d20);
    insert(103, "d100", R_INT, {R_INT}, &d100);
    insert(104, "VectorMagnitude", R_FLOAT, {R_VECTOR}, &VectorMagnitude);
    insert(105, "GetMetaMagicFeat", R_INT, {}, &GetMetaMagicFeat);
    insert(106, "GetObjectType", R_INT, {R_OBJECT}, &GetObjectType);
    insert(107, "GetRacialType", R_INT, {R_OBJECT}, &GetRacialType);
    insert(108, "FortitudeSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &FortitudeSave);
    insert(109, "ReflexSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &ReflexSave);
    insert(110, "WillSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &WillSave);
    insert(111, "GetSpellSaveDC", R_INT, {}, &GetSpellSaveDC);
    insert(112, "MagicalEffect", R_EFFECT, {R_EFFECT}, &MagicalEffect);
    insert(113, "SupernaturalEffect", R_EFFECT, {R_EFFECT}, &SupernaturalEffect);
    insert(114, "ExtraordinaryEffect", R_EFFECT, {R_EFFECT}, &ExtraordinaryEffect);
    insert(116, "GetAC", R_INT, {R_OBJECT, R_INT}, &GetAC);
    insert(121, "RoundsToSeconds", R_FLOAT, {R_INT}, &RoundsToSeconds);
    insert(122, "HoursToSeconds", R_FLOAT, {R_INT}, &HoursToSeconds);
    insert(123, "TurnsToSeconds", R_FLOAT, {R_INT}, &TurnsToSeconds);
    insert(124, "SoundObjectSetFixedVariance", R_VOID, {R_OBJECT, R_FLOAT}, &SoundObjectSetFixedVariance);
    insert(125, "GetGoodEvilValue", R_INT, {R_OBJECT}, &GetGoodEvilValue);
    insert(126, "GetPartyMemberCount", R_INT, {}, &GetPartyMemberCount);
    insert(127, "GetAlignmentGoodEvil", R_INT, {R_OBJECT}, &GetAlignmentGoodEvil);
    insert(128, "GetFirstObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &GetFirstObjectInShape);
    insert(129, "GetNextObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &GetNextObjectInShape);
    insert(131, "SignalEvent", R_VOID, {R_OBJECT, R_EVENT}, &SignalEvent);
    insert(132, "EventUserDefined", R_EVENT, {R_INT}, &EventUserDefined);
    insert(137, "VectorNormalize", R_VECTOR, {R_VECTOR}, &VectorNormalize);
    insert(138, "GetItemStackSize", R_INT, {R_OBJECT}, &GetItemStackSize);
    insert(139, "GetAbilityScore", R_INT, {R_OBJECT, R_INT}, &GetAbilityScore);
    insert(140, "GetIsDead", R_INT, {R_OBJECT}, &GetIsDead);
    insert(141, "PrintVector", R_VOID, {R_VECTOR, R_INT}, &PrintVector);
    insert(142, "Vector", R_VECTOR, {R_FLOAT, R_FLOAT, R_FLOAT}, &Vector);
    insert(143, "SetFacingPoint", R_VOID, {R_VECTOR}, &SetFacingPoint);
    insert(144, "AngleToVector", R_VECTOR, {R_FLOAT}, &AngleToVector);
    insert(145, "VectorToAngle", R_FLOAT, {R_VECTOR}, &VectorToAngle);
    insert(146, "TouchAttackMelee", R_INT, {R_OBJECT, R_INT}, &TouchAttackMelee);
    insert(147, "TouchAttackRanged", R_INT, {R_OBJECT, R_INT}, &TouchAttackRanged);
    insert(150, "SetItemStackSize", R_VOID, {R_OBJECT, R_INT}, &SetItemStackSize);
    insert(151, "GetDistanceBetween", R_FLOAT, {R_OBJECT, R_OBJECT}, &GetDistanceBetween);
    insert(152, "SetReturnStrref", R_VOID, {R_INT, R_INT, R_INT}, &SetReturnStrref);
    insert(155, "GetItemInSlot", R_OBJECT, {R_INT, R_OBJECT}, &GetItemInSlot);
    insert(160, "SetGlobalString", R_VOID, {R_STRING, R_STRING}, &SetGlobalString);
    insert(162, "SetCommandable", R_VOID, {R_INT, R_OBJECT}, &SetCommandable);
    insert(163, "GetCommandable", R_INT, {R_OBJECT}, &GetCommandable);
    insert(166, "GetHitDice", R_INT, {R_OBJECT}, &GetHitDice);
    insert(168, "GetTag", R_STRING, {R_OBJECT}, &GetTag);
    insert(169, "ResistForce", R_INT, {R_OBJECT, R_OBJECT}, &ResistForce);
    insert(170, "GetEffectType", R_INT, {R_EFFECT}, &GetEffectType);
    insert(172, "GetFactionEqual", R_INT, {R_OBJECT, R_OBJECT}, &GetFactionEqual);
    insert(173, "ChangeFaction", R_VOID, {R_OBJECT, R_OBJECT}, &ChangeFaction);
    insert(174, "GetIsListening", R_INT, {R_OBJECT}, &GetIsListening);
    insert(175, "SetListening", R_VOID, {R_OBJECT, R_INT}, &SetListening);
    insert(176, "SetListenPattern", R_VOID, {R_OBJECT, R_STRING, R_INT}, &SetListenPattern);
    insert(177, "TestStringAgainstPattern", R_INT, {R_STRING, R_STRING}, &TestStringAgainstPattern);
    insert(178, "GetMatchedSubstring", R_STRING, {R_INT}, &GetMatchedSubstring);
    insert(179, "GetMatchedSubstringsCount", R_INT, {}, &GetMatchedSubstringsCount);
    insert(181, "GetFactionWeakestMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionWeakestMember);
    insert(182, "GetFactionStrongestMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionStrongestMember);
    insert(183, "GetFactionMostDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionMostDamagedMember);
    insert(184, "GetFactionLeastDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionLeastDamagedMember);
    insert(185, "GetFactionGold", R_INT, {R_OBJECT}, &GetFactionGold);
    insert(186, "GetFactionAverageReputation", R_INT, {R_OBJECT, R_OBJECT}, &GetFactionAverageReputation);
    insert(187, "GetFactionAverageGoodEvilAlignment", R_INT, {R_OBJECT}, &GetFactionAverageGoodEvilAlignment);
    insert(188, "SoundObjectGetFixedVariance", R_FLOAT, {R_OBJECT}, &SoundObjectGetFixedVariance);
    insert(189, "GetFactionAverageLevel", R_INT, {R_OBJECT}, &GetFactionAverageLevel);
    insert(190, "GetFactionAverageXP", R_INT, {R_OBJECT}, &GetFactionAverageXP);
    insert(191, "GetFactionMostFrequentClass", R_INT, {R_OBJECT}, &GetFactionMostFrequentClass);
    insert(192, "GetFactionWorstAC", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionWorstAC);
    insert(193, "GetFactionBestAC", R_OBJECT, {R_OBJECT, R_INT}, &GetFactionBestAC);
    insert(194, "GetGlobalString", R_STRING, {R_STRING}, &GetGlobalString);
    insert(195, "GetListenPatternNumber", R_INT, {}, &GetListenPatternNumber);
    insert(197, "GetWaypointByTag", R_OBJECT, {R_STRING}, &GetWaypointByTag);
    insert(198, "GetTransitionTarget", R_OBJECT, {R_OBJECT}, &GetTransitionTarget);
    insert(200, "GetObjectByTag", R_OBJECT, {R_STRING, R_INT}, &GetObjectByTag);
    insert(201, "AdjustAlignment", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &AdjustAlignment);
    insert(203, "SetAreaTransitionBMP", R_VOID, {R_INT, R_STRING}, &SetAreaTransitionBMP);
    insert(208, "GetReputation", R_INT, {R_OBJECT, R_OBJECT}, &GetReputation);
    insert(209, "AdjustReputation", R_VOID, {R_OBJECT, R_OBJECT, R_INT}, &AdjustReputation);
    insert(210, "GetModuleFileName", R_STRING, {}, &GetModuleFileName);
    insert(211, "GetGoingToBeAttackedBy", R_OBJECT, {R_OBJECT}, &GetGoingToBeAttackedBy);
    insert(213, "GetLocation", R_LOCATION, {R_OBJECT}, &GetLocation);
    insert(215, "Location", R_LOCATION, {R_VECTOR, R_FLOAT}, &CreateLocation);
    insert(216, "ApplyEffectAtLocation", R_VOID, {R_INT, R_EFFECT, R_LOCATION, R_FLOAT}, &ApplyEffectAtLocation);
    insert(217, "GetIsPC", R_INT, {R_OBJECT}, &GetIsPC);
    insert(218, "FeetToMeters", R_FLOAT, {R_FLOAT}, &FeetToMeters);
    insert(219, "YardsToMeters", R_FLOAT, {R_FLOAT}, &YardsToMeters);
    insert(220, "ApplyEffectToObject", R_VOID, {R_INT, R_EFFECT, R_OBJECT, R_FLOAT}, &ApplyEffectToObject);
    insert(221, "SpeakString", R_VOID, {R_STRING, R_INT}, &SpeakString);
    insert(222, "GetSpellTargetLocation", R_LOCATION, {}, &GetSpellTargetLocation);
    insert(223, "GetPositionFromLocation", R_VECTOR, {R_LOCATION}, &GetPositionFromLocation);
    insert(225, "GetFacingFromLocation", R_FLOAT, {R_LOCATION}, &GetFacingFromLocation);
    insert(226, "GetNearestCreatureToLocation", R_OBJECT, {R_INT, R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT, R_INT}, &GetNearestCreatureToLocation);
    insert(227, "GetNearestObject", R_OBJECT, {R_INT, R_OBJECT, R_INT}, &GetNearestObject);
    insert(228, "GetNearestObjectToLocation", R_OBJECT, {R_INT, R_LOCATION, R_INT}, &GetNearestObjectToLocation);
    insert(229, "GetNearestObjectByTag", R_OBJECT, {R_STRING, R_OBJECT, R_INT}, &GetNearestObjectByTag);
    insert(230, "IntToFloat", R_FLOAT, {R_INT}, &IntToFloat);
    insert(231, "FloatToInt", R_INT, {R_FLOAT}, &FloatToInt);
    insert(232, "StringToInt", R_INT, {R_STRING}, &StringToInt);
    insert(233, "StringToFloat", R_FLOAT, {R_STRING}, &StringToFloat);
    insert(235, "GetIsEnemy", R_INT, {R_OBJECT, R_OBJECT}, &GetIsEnemy);
    insert(236, "GetIsFriend", R_INT, {R_OBJECT, R_OBJECT}, &GetIsFriend);
    insert(237, "GetIsNeutral", R_INT, {R_OBJECT, R_OBJECT}, &GetIsNeutral);
    insert(238, "GetPCSpeaker", R_OBJECT, {}, &GetPCSpeaker);
    insert(239, "GetStringByStrRef", R_STRING, {R_INT}, &GetStringByStrRef);
    insert(241, "DestroyObject", R_VOID, {R_OBJECT, R_FLOAT, R_INT, R_FLOAT, R_INT}, &DestroyObject);
    insert(242, "GetModule", R_OBJECT, {}, &GetModule);
    insert(243, "CreateObject", R_OBJECT, {R_INT, R_STRING, R_LOCATION, R_INT}, &CreateObject);
    insert(244, "EventSpellCastAt", R_EVENT, {R_OBJECT, R_INT, R_INT}, &EventSpellCastAt);
    insert(245, "GetLastSpellCaster", R_OBJECT, {}, &GetLastSpellCaster);
    insert(246, "GetLastSpell", R_INT, {}, &GetLastSpell);
    insert(247, "GetUserDefinedEventNumber", R_INT, {}, &GetUserDefinedEventNumber);
    insert(248, "GetSpellId", R_INT, {}, &GetSpellId);
    insert(249, "RandomName", R_STRING, {}, &RandomName);
    insert(251, "GetLoadFromSaveGame", R_INT, {}, &GetLoadFromSaveGame);
    insert(253, "GetName", R_STRING, {R_OBJECT}, &GetName);
    insert(254, "GetLastSpeaker", R_OBJECT, {}, &GetLastSpeaker);
    insert(255, "BeginConversation", R_INT, {R_STRING, R_OBJECT}, &BeginConversation);
    insert(256, "GetLastPerceived", R_OBJECT, {}, &GetLastPerceived);
    insert(257, "GetLastPerceptionHeard", R_INT, {}, &GetLastPerceptionHeard);
    insert(258, "GetLastPerceptionInaudible", R_INT, {}, &GetLastPerceptionInaudible);
    insert(259, "GetLastPerceptionSeen", R_INT, {}, &GetLastPerceptionSeen);
    insert(260, "GetLastClosedBy", R_OBJECT, {}, &GetLastClosedBy);
    insert(261, "GetLastPerceptionVanished", R_INT, {}, &GetLastPerceptionVanished);
    insert(262, "GetFirstInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &GetFirstInPersistentObject);
    insert(263, "GetNextInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &GetNextInPersistentObject);
    insert(264, "GetAreaOfEffectCreator", R_OBJECT, {R_OBJECT}, &GetAreaOfEffectCreator);
    insert(265, "ShowLevelUpGUI", R_INT, {}, &ShowLevelUpGUI);
    insert(266, "SetItemNonEquippable", R_VOID, {R_OBJECT, R_INT}, &SetItemNonEquippable);
    insert(267, "GetButtonMashCheck", R_INT, {}, &GetButtonMashCheck);
    insert(268, "SetButtonMashCheck", R_VOID, {R_INT}, &SetButtonMashCheck);
    insert(271, "GiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &GiveItem);
    insert(272, "ObjectToString", R_STRING, {R_OBJECT}, &ObjectToString);
    insert(274, "GetIsImmune", R_INT, {R_OBJECT, R_INT, R_OBJECT}, &GetIsImmune);
    insert(276, "GetEncounterActive", R_INT, {R_OBJECT}, &GetEncounterActive);
    insert(277, "SetEncounterActive", R_VOID, {R_INT, R_OBJECT}, &SetEncounterActive);
    insert(278, "GetEncounterSpawnsMax", R_INT, {R_OBJECT}, &GetEncounterSpawnsMax);
    insert(279, "SetEncounterSpawnsMax", R_VOID, {R_INT, R_OBJECT}, &SetEncounterSpawnsMax);
    insert(280, "GetEncounterSpawnsCurrent", R_INT, {R_OBJECT}, &GetEncounterSpawnsCurrent);
    insert(281, "SetEncounterSpawnsCurrent", R_VOID, {R_INT, R_OBJECT}, &SetEncounterSpawnsCurrent);
    insert(282, "GetModuleItemAcquired", R_OBJECT, {}, &GetModuleItemAcquired);
    insert(283, "GetModuleItemAcquiredFrom", R_OBJECT, {}, &GetModuleItemAcquiredFrom);
    insert(284, "SetCustomToken", R_VOID, {R_INT, R_STRING}, &SetCustomToken);
    insert(285, "GetHasFeat", R_INT, {R_INT, R_OBJECT}, &GetHasFeat);
    insert(286, "GetHasSkill", R_INT, {R_INT, R_OBJECT}, &GetHasSkill);
    insert(289, "GetObjectSeen", R_INT, {R_OBJECT, R_OBJECT}, &GetObjectSeen);
    insert(290, "GetObjectHeard", R_INT, {R_OBJECT, R_OBJECT}, &GetObjectHeard);
    insert(291, "GetLastPlayerDied", R_OBJECT, {}, &GetLastPlayerDied);
    insert(292, "GetModuleItemLost", R_OBJECT, {}, &GetModuleItemLost);
    insert(293, "GetModuleItemLostBy", R_OBJECT, {}, &GetModuleItemLostBy);
    insert(295, "EventConversation", R_EVENT, {}, &EventConversation);
    insert(296, "SetEncounterDifficulty", R_VOID, {R_INT, R_OBJECT}, &SetEncounterDifficulty);
    insert(297, "GetEncounterDifficulty", R_INT, {R_OBJECT}, &GetEncounterDifficulty);
    insert(298, "GetDistanceBetweenLocations", R_FLOAT, {R_LOCATION, R_LOCATION}, &GetDistanceBetweenLocations);
    insert(299, "GetReflexAdjustedDamage", R_INT, {R_INT, R_OBJECT, R_INT, R_INT, R_OBJECT}, &GetReflexAdjustedDamage);
    insert(300, "PlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &PlayAnimation);
    insert(301, "TalentSpell", R_TALENT, {R_INT}, &TalentSpell);
    insert(302, "TalentFeat", R_TALENT, {R_INT}, &TalentFeat);
    insert(303, "TalentSkill", R_TALENT, {R_INT}, &TalentSkill);
    insert(304, "GetHasSpellEffect", R_INT, {R_INT, R_OBJECT}, &GetHasSpellEffect);
    insert(305, "GetEffectSpellId", R_INT, {R_EFFECT}, &GetEffectSpellId);
    insert(306, "GetCreatureHasTalent", R_INT, {R_TALENT, R_OBJECT}, &GetCreatureHasTalent);
    insert(307, "GetCreatureTalentRandom", R_TALENT, {R_INT, R_OBJECT, R_INT}, &GetCreatureTalentRandom);
    insert(308, "GetCreatureTalentBest", R_TALENT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT}, &GetCreatureTalentBest);
    insert(311, "GetGoldPieceValue", R_INT, {R_OBJECT}, &GetGoldPieceValue);
    insert(312, "GetIsPlayableRacialType", R_INT, {R_OBJECT}, &GetIsPlayableRacialType);
    insert(313, "JumpToLocation", R_VOID, {R_LOCATION}, &JumpToLocation);
    insert(315, "GetSkillRank", R_INT, {R_INT, R_OBJECT}, &GetSkillRank);
    insert(316, "GetAttackTarget", R_OBJECT, {R_OBJECT}, &GetAttackTarget);
    insert(317, "GetLastAttackType", R_INT, {R_OBJECT}, &GetLastAttackType);
    insert(318, "GetLastAttackMode", R_INT, {R_OBJECT}, &GetLastAttackMode);
    insert(319, "GetDistanceBetween2D", R_FLOAT, {R_OBJECT, R_OBJECT}, &GetDistanceBetween2D);
    insert(320, "GetIsInCombat", R_INT, {R_OBJECT, R_INT}, &GetIsInCombat);
    insert(321, "GetLastAssociateCommand", R_INT, {R_OBJECT}, &GetLastAssociateCommand);
    insert(322, "GiveGoldToCreature", R_VOID, {R_OBJECT, R_INT}, &GiveGoldToCreature);
    insert(323, "SetIsDestroyable", R_VOID, {R_INT, R_INT, R_INT}, &SetIsDestroyable);
    insert(324, "SetLocked", R_VOID, {R_OBJECT, R_INT}, &SetLocked);
    insert(325, "GetLocked", R_INT, {R_OBJECT}, &GetLocked);
    insert(326, "GetClickingObject", R_OBJECT, {}, &GetClickingObject);
    insert(327, "SetAssociateListenPatterns", R_VOID, {R_OBJECT}, &SetAssociateListenPatterns);
    insert(328, "GetLastWeaponUsed", R_OBJECT, {R_OBJECT}, &GetLastWeaponUsed);
    insert(330, "GetLastUsedBy", R_OBJECT, {}, &GetLastUsedBy);
    insert(331, "GetAbilityModifier", R_INT, {R_INT, R_OBJECT}, &GetAbilityModifier);
    insert(332, "GetIdentified", R_INT, {R_OBJECT}, &GetIdentified);
    insert(333, "SetIdentified", R_VOID, {R_OBJECT, R_INT}, &SetIdentified);
    insert(334, "GetDistanceBetweenLocations2D", R_FLOAT, {R_LOCATION, R_LOCATION}, &GetDistanceBetweenLocations2D);
    insert(335, "GetDistanceToObject2D", R_FLOAT, {R_OBJECT}, &GetDistanceToObject2D);
    insert(336, "GetBlockingDoor", R_OBJECT, {}, &GetBlockingDoor);
    insert(337, "GetIsDoorActionPossible", R_INT, {R_OBJECT, R_INT}, &GetIsDoorActionPossible);
    insert(338, "DoDoorAction", R_VOID, {R_OBJECT, R_INT}, &DoDoorAction);
    insert(339, "GetFirstItemInInventory", R_OBJECT, {R_OBJECT}, &GetFirstItemInInventory);
    insert(340, "GetNextItemInInventory", R_OBJECT, {R_OBJECT}, &GetNextItemInInventory);
    insert(341, "GetClassByPosition", R_INT, {R_INT, R_OBJECT}, &GetClassByPosition);
    insert(342, "GetLevelByPosition", R_INT, {R_INT, R_OBJECT}, &GetLevelByPosition);
    insert(343, "GetLevelByClass", R_INT, {R_INT, R_OBJECT}, &GetLevelByClass);
    insert(344, "GetDamageDealtByType", R_INT, {R_INT}, &GetDamageDealtByType);
    insert(345, "GetTotalDamageDealt", R_INT, {}, &GetTotalDamageDealt);
    insert(346, "GetLastDamager", R_OBJECT, {}, &GetLastDamager);
    insert(347, "GetLastDisarmed", R_OBJECT, {}, &GetLastDisarmed);
    insert(348, "GetLastDisturbed", R_OBJECT, {}, &GetLastDisturbed);
    insert(349, "GetLastLocked", R_OBJECT, {}, &GetLastLocked);
    insert(350, "GetLastUnlocked", R_OBJECT, {}, &GetLastUnlocked);
    insert(352, "GetInventoryDisturbType", R_INT, {}, &GetInventoryDisturbType);
    insert(353, "GetInventoryDisturbItem", R_OBJECT, {}, &GetInventoryDisturbItem);
    insert(354, "ShowUpgradeScreen", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT, R_STRING}, &ShowUpgradeScreen);
    insert(355, "VersusAlignmentEffect", R_EFFECT, {R_EFFECT, R_INT, R_INT}, &VersusAlignmentEffect);
    insert(356, "VersusRacialTypeEffect", R_EFFECT, {R_EFFECT, R_INT}, &VersusRacialTypeEffect);
    insert(357, "VersusTrapEffect", R_EFFECT, {R_EFFECT}, &VersusTrapEffect);
    insert(358, "GetGender", R_INT, {R_OBJECT}, &GetGender);
    insert(359, "GetIsTalentValid", R_INT, {R_TALENT}, &GetIsTalentValid);
    insert(361, "GetAttemptedAttackTarget", R_OBJECT, {}, &GetAttemptedAttackTarget);
    insert(362, "GetTypeFromTalent", R_INT, {R_TALENT}, &GetTypeFromTalent);
    insert(363, "GetIdFromTalent", R_INT, {R_TALENT}, &GetIdFromTalent);
    insert(364, "PlayPazaak", R_VOID, {R_INT, R_STRING, R_INT, R_INT, R_OBJECT}, &PlayPazaak);
    insert(365, "GetLastPazaakResult", R_INT, {}, &GetLastPazaakResult);
    insert(366, "DisplayFeedBackText", R_VOID, {R_OBJECT, R_INT}, &DisplayFeedBackText);
    insert(367, "AddJournalQuestEntry", R_VOID, {R_STRING, R_INT, R_INT}, &AddJournalQuestEntry);
    insert(368, "RemoveJournalQuestEntry", R_VOID, {R_STRING}, &RemoveJournalQuestEntry);
    insert(369, "GetJournalEntry", R_INT, {R_STRING}, &GetJournalEntry);
    insert(370, "PlayRumblePattern", R_INT, {R_INT}, &PlayRumblePattern);
    insert(371, "StopRumblePattern", R_INT, {R_INT}, &StopRumblePattern);
    insert(374, "SendMessageToPC", R_VOID, {R_OBJECT, R_STRING}, &SendMessageToPC);
    insert(375, "GetAttemptedSpellTarget", R_OBJECT, {}, &GetAttemptedSpellTarget);
    insert(376, "GetLastOpenedBy", R_OBJECT, {}, &GetLastOpenedBy);
    insert(377, "GetHasSpell", R_INT, {R_INT, R_OBJECT}, &GetHasSpell);
    insert(378, "OpenStore", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &OpenStore);
    insert(380, "GetFirstFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &GetFirstFactionMember);
    insert(381, "GetNextFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &GetNextFactionMember);
    insert(384, "GetJournalQuestExperience", R_INT, {R_STRING}, &GetJournalQuestExperience);
    insert(385, "JumpToObject", R_VOID, {R_OBJECT, R_INT}, &JumpToObject);
    insert(386, "SetMapPinEnabled", R_VOID, {R_OBJECT, R_INT}, &SetMapPinEnabled);
    insert(388, "PopUpGUIPanel", R_VOID, {R_OBJECT, R_INT}, &PopUpGUIPanel);
    insert(389, "AddMultiClass", R_VOID, {R_INT, R_OBJECT}, &AddMultiClass);
    insert(390, "GetIsLinkImmune", R_INT, {R_OBJECT, R_EFFECT}, &GetIsLinkImmune);
    insert(393, "GiveXPToCreature", R_VOID, {R_OBJECT, R_INT}, &GiveXPToCreature);
    insert(394, "SetXP", R_VOID, {R_OBJECT, R_INT}, &SetXP);
    insert(395, "GetXP", R_INT, {R_OBJECT}, &GetXP);
    insert(396, "IntToHexString", R_STRING, {R_INT}, &IntToHexString);
    insert(397, "GetBaseItemType", R_INT, {R_OBJECT}, &GetBaseItemType);
    insert(398, "GetItemHasItemProperty", R_INT, {R_OBJECT, R_INT}, &GetItemHasItemProperty);
    insert(401, "GetItemACValue", R_INT, {R_OBJECT}, &GetItemACValue);
    insert(403, "ExploreAreaForPlayer", R_VOID, {R_OBJECT, R_OBJECT}, &ExploreAreaForPlayer);
    insert(405, "GetIsDay", R_INT, {}, &GetIsDay);
    insert(406, "GetIsNight", R_INT, {}, &GetIsNight);
    insert(407, "GetIsDawn", R_INT, {}, &GetIsDawn);
    insert(408, "GetIsDusk", R_INT, {}, &GetIsDusk);
    insert(409, "GetIsEncounterCreature", R_INT, {R_OBJECT}, &GetIsEncounterCreature);
    insert(410, "GetLastPlayerDying", R_OBJECT, {}, &GetLastPlayerDying);
    insert(411, "GetStartingLocation", R_LOCATION, {}, &GetStartingLocation);
    insert(412, "ChangeToStandardFaction", R_VOID, {R_OBJECT, R_INT}, &ChangeToStandardFaction);
    insert(413, "SoundObjectPlay", R_VOID, {R_OBJECT}, &SoundObjectPlay);
    insert(414, "SoundObjectStop", R_VOID, {R_OBJECT}, &SoundObjectStop);
    insert(415, "SoundObjectSetVolume", R_VOID, {R_OBJECT, R_INT}, &SoundObjectSetVolume);
    insert(416, "SoundObjectSetPosition", R_VOID, {R_OBJECT, R_VECTOR}, &SoundObjectSetPosition);
    insert(417, "SpeakOneLinerConversation", R_VOID, {R_STRING, R_OBJECT}, &SpeakOneLinerConversation);
    insert(418, "GetGold", R_INT, {R_OBJECT}, &GetGold);
    insert(419, "GetLastRespawnButtonPresser", R_OBJECT, {}, &GetLastRespawnButtonPresser);
    insert(421, "SetLightsaberPowered", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &SetLightsaberPowered);
    insert(422, "GetIsWeaponEffective", R_INT, {R_OBJECT, R_INT}, &GetIsWeaponEffective);
    insert(423, "GetLastSpellHarmful", R_INT, {}, &GetLastSpellHarmful);
    insert(424, "EventActivateItem", R_EVENT, {R_OBJECT, R_LOCATION, R_OBJECT}, &EventActivateItem);
    insert(425, "MusicBackgroundPlay", R_VOID, {R_OBJECT}, &MusicBackgroundPlay);
    insert(426, "MusicBackgroundStop", R_VOID, {R_OBJECT}, &MusicBackgroundStop);
    insert(427, "MusicBackgroundSetDelay", R_VOID, {R_OBJECT, R_INT}, &MusicBackgroundSetDelay);
    insert(428, "MusicBackgroundChangeDay", R_VOID, {R_OBJECT, R_INT, R_INT}, &MusicBackgroundChangeDay);
    insert(429, "MusicBackgroundChangeNight", R_VOID, {R_OBJECT, R_INT, R_INT}, &MusicBackgroundChangeNight);
    insert(430, "MusicBattlePlay", R_VOID, {R_OBJECT}, &MusicBattlePlay);
    insert(431, "MusicBattleStop", R_VOID, {R_OBJECT}, &MusicBattleStop);
    insert(432, "MusicBattleChange", R_VOID, {R_OBJECT, R_INT}, &MusicBattleChange);
    insert(433, "AmbientSoundPlay", R_VOID, {R_OBJECT}, &AmbientSoundPlay);
    insert(434, "AmbientSoundStop", R_VOID, {R_OBJECT}, &AmbientSoundStop);
    insert(435, "AmbientSoundChangeDay", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundChangeDay);
    insert(436, "AmbientSoundChangeNight", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundChangeNight);
    insert(437, "GetLastKiller", R_OBJECT, {}, &GetLastKiller);
    insert(438, "GetSpellCastItem", R_OBJECT, {}, &GetSpellCastItem);
    insert(439, "GetItemActivated", R_OBJECT, {}, &GetItemActivated);
    insert(440, "GetItemActivator", R_OBJECT, {}, &GetItemActivator);
    insert(441, "GetItemActivatedTargetLocation", R_LOCATION, {}, &GetItemActivatedTargetLocation);
    insert(442, "GetItemActivatedTarget", R_OBJECT, {}, &GetItemActivatedTarget);
    insert(443, "GetIsOpen", R_INT, {R_OBJECT}, &GetIsOpen);
    insert(444, "TakeGoldFromCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &TakeGoldFromCreature);
    insert(445, "GetIsInConversation", R_INT, {R_OBJECT}, &GetIsInConversation);
    insert(455, "GetPlotFlag", R_INT, {R_OBJECT}, &GetPlotFlag);
    insert(456, "SetPlotFlag", R_VOID, {R_OBJECT, R_INT}, &SetPlotFlag);
    insert(461, "SetDialogPlaceableCamera", R_VOID, {R_INT}, &SetDialogPlaceableCamera);
    insert(462, "GetSoloMode", R_INT, {}, &GetSoloMode);
    insert(464, "GetMaxStealthXP", R_INT, {}, &GetMaxStealthXP);
    insert(468, "SetMaxStealthXP", R_VOID, {R_INT}, &SetMaxStealthXP);
    insert(474, "GetCurrentStealthXP", R_INT, {}, &GetCurrentStealthXP);
    insert(475, "GetNumStackedItems", R_INT, {R_OBJECT}, &GetNumStackedItems);
    insert(476, "SurrenderToEnemies", R_VOID, {}, &SurrenderToEnemies);
    insert(478, "SetCurrentStealthXP", R_VOID, {R_INT}, &SetCurrentStealthXP);
    insert(479, "GetCreatureSize", R_INT, {R_OBJECT}, &GetCreatureSize);
    insert(480, "AwardStealthXP", R_VOID, {R_OBJECT}, &AwardStealthXP);
    insert(481, "GetStealthXPEnabled", R_INT, {}, &GetStealthXPEnabled);
    insert(482, "SetStealthXPEnabled", R_VOID, {R_INT}, &SetStealthXPEnabled);
    insert(486, "GetLastTrapDetected", R_OBJECT, {R_OBJECT}, &GetLastTrapDetected);
    insert(488, "GetNearestTrapToObject", R_OBJECT, {R_OBJECT, R_INT}, &GetNearestTrapToObject);
    insert(489, "GetAttemptedMovementTarget", R_OBJECT, {}, &GetAttemptedMovementTarget);
    insert(490, "GetBlockingCreature", R_OBJECT, {R_OBJECT}, &GetBlockingCreature);
    insert(491, "GetFortitudeSavingThrow", R_INT, {R_OBJECT}, &GetFortitudeSavingThrow);
    insert(492, "GetWillSavingThrow", R_INT, {R_OBJECT}, &GetWillSavingThrow);
    insert(493, "GetReflexSavingThrow", R_INT, {R_OBJECT}, &GetReflexSavingThrow);
    insert(494, "GetChallengeRating", R_FLOAT, {R_OBJECT}, &GetChallengeRating);
    insert(495, "GetFoundEnemyCreature", R_OBJECT, {R_OBJECT}, &GetFoundEnemyCreature);
    insert(496, "GetMovementRate", R_INT, {R_OBJECT}, &GetMovementRate);
    insert(497, "GetSubRace", R_INT, {R_OBJECT}, &GetSubRace);
    insert(498, "GetStealthXPDecrement", R_INT, {}, &GetStealthXPDecrement);
    insert(499, "SetStealthXPDecrement", R_VOID, {R_INT}, &SetStealthXPDecrement);
    insert(500, "DuplicateHeadAppearance", R_VOID, {R_OBJECT, R_OBJECT}, &DuplicateHeadAppearance);
    insert(503, "CutsceneAttack", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &CutsceneAttack);
    insert(504, "SetCameraMode", R_VOID, {R_OBJECT, R_INT}, &SetCameraMode);
    insert(505, "SetLockOrientationInDialog", R_VOID, {R_OBJECT, R_INT}, &SetLockOrientationInDialog);
    insert(506, "SetLockHeadFollowInDialog", R_VOID, {R_OBJECT, R_INT}, &SetLockHeadFollowInDialog);
    insert(507, "CutsceneMove", R_VOID, {R_OBJECT, R_VECTOR, R_INT}, &CutsceneMove);
    insert(508, "EnableVideoEffect", R_VOID, {R_INT}, &EnableVideoEffect);
    insert(509, "StartNewModule", R_VOID, {R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING}, &StartNewModule);
    insert(510, "DisableVideoEffect", R_VOID, {}, &DisableVideoEffect);
    insert(511, "GetWeaponRanged", R_INT, {R_OBJECT}, &GetWeaponRanged);
    insert(512, "DoSinglePlayerAutoSave", R_VOID, {}, &DoSinglePlayerAutoSave);
    insert(513, "GetGameDifficulty", R_INT, {}, &GetGameDifficulty);
    insert(514, "GetUserActionsPending", R_INT, {}, &GetUserActionsPending);
    insert(515, "RevealMap", R_VOID, {R_VECTOR, R_INT}, &RevealMap);
    insert(516, "SetTutorialWindowsEnabled", R_VOID, {R_INT}, &SetTutorialWindowsEnabled);
    insert(517, "ShowTutorialWindow", R_VOID, {R_INT}, &ShowTutorialWindow);
    insert(518, "StartCreditSequence", R_VOID, {R_INT}, &StartCreditSequence);
    insert(519, "IsCreditSequenceInProgress", R_INT, {}, &IsCreditSequenceInProgress);
    insert(522, "GetCurrentAction", R_INT, {R_OBJECT}, &GetCurrentAction);
    insert(523, "GetDifficultyModifier", R_FLOAT, {}, &GetDifficultyModifier);
    insert(524, "GetAppearanceType", R_INT, {R_OBJECT}, &GetAppearanceType);
    insert(525, "FloatingTextStrRefOnCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &FloatingTextStrRefOnCreature);
    insert(526, "FloatingTextStringOnCreature", R_VOID, {R_STRING, R_OBJECT, R_INT}, &FloatingTextStringOnCreature);
    insert(527, "GetTrapDisarmable", R_INT, {R_OBJECT}, &GetTrapDisarmable);
    insert(528, "GetTrapDetectable", R_INT, {R_OBJECT}, &GetTrapDetectable);
    insert(529, "GetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &GetTrapDetectedBy);
    insert(530, "GetTrapFlagged", R_INT, {R_OBJECT}, &GetTrapFlagged);
    insert(531, "GetTrapBaseType", R_INT, {R_OBJECT}, &GetTrapBaseType);
    insert(532, "GetTrapOneShot", R_INT, {R_OBJECT}, &GetTrapOneShot);
    insert(533, "GetTrapCreator", R_OBJECT, {R_OBJECT}, &GetTrapCreator);
    insert(534, "GetTrapKeyTag", R_STRING, {R_OBJECT}, &GetTrapKeyTag);
    insert(535, "GetTrapDisarmDC", R_INT, {R_OBJECT}, &GetTrapDisarmDC);
    insert(536, "GetTrapDetectDC", R_INT, {R_OBJECT}, &GetTrapDetectDC);
    insert(537, "GetLockKeyRequired", R_INT, {R_OBJECT}, &GetLockKeyRequired);
    insert(538, "GetLockKeyTag", R_INT, {R_OBJECT}, &GetLockKeyTag);
    insert(539, "GetLockLockable", R_INT, {R_OBJECT}, &GetLockLockable);
    insert(540, "GetLockUnlockDC", R_INT, {R_OBJECT}, &GetLockUnlockDC);
    insert(541, "GetLockLockDC", R_INT, {R_OBJECT}, &GetLockLockDC);
    insert(542, "GetPCLevellingUp", R_OBJECT, {}, &GetPCLevellingUp);
    insert(543, "GetHasFeatEffect", R_INT, {R_INT, R_OBJECT}, &GetHasFeatEffect);
    insert(544, "SetPlaceableIllumination", R_VOID, {R_OBJECT, R_INT}, &SetPlaceableIllumination);
    insert(545, "GetPlaceableIllumination", R_INT, {R_OBJECT}, &GetPlaceableIllumination);
    insert(546, "GetIsPlaceableObjectActionPossible", R_INT, {R_OBJECT, R_INT}, &GetIsPlaceableObjectActionPossible);
    insert(547, "DoPlaceableObjectAction", R_VOID, {R_OBJECT, R_INT}, &DoPlaceableObjectAction);
    insert(548, "GetFirstPC", R_OBJECT, {}, &GetFirstPC);
    insert(549, "GetNextPC", R_OBJECT, {}, &GetNextPC);
    insert(550, "SetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &SetTrapDetectedBy);
    insert(551, "GetIsTrapped", R_INT, {R_OBJECT}, &GetIsTrapped);
    insert(552, "SetEffectIcon", R_EFFECT, {R_EFFECT, R_INT}, &SetEffectIcon);
    insert(553, "FaceObjectAwayFromObject", R_VOID, {R_OBJECT, R_OBJECT}, &FaceObjectAwayFromObject);
    insert(554, "PopUpDeathGUIPanel", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT, R_STRING}, &PopUpDeathGUIPanel);
    insert(555, "SetTrapDisabled", R_VOID, {R_OBJECT}, &SetTrapDisabled);
    insert(556, "GetLastHostileActor", R_OBJECT, {R_OBJECT}, &GetLastHostileActor);
    insert(557, "ExportAllCharacters", R_VOID, {}, &ExportAllCharacters);
    insert(558, "MusicBackgroundGetDayTrack", R_INT, {R_OBJECT}, &MusicBackgroundGetDayTrack);
    insert(559, "MusicBackgroundGetNightTrack", R_INT, {R_OBJECT}, &MusicBackgroundGetNightTrack);
    insert(560, "WriteTimestampedLogEntry", R_VOID, {R_STRING}, &WriteTimestampedLogEntry);
    insert(561, "GetModuleName", R_STRING, {}, &GetModuleName);
    insert(562, "GetFactionLeader", R_OBJECT, {R_OBJECT}, &GetFactionLeader);
    insert(564, "EndGame", R_VOID, {R_INT}, &EndGame);
    insert(565, "GetRunScriptVar", R_INT, {}, &GetRunScriptVar);
    insert(566, "GetCreatureMovmentType", R_INT, {R_OBJECT}, &GetCreatureMovmentType);
    insert(567, "AmbientSoundSetDayVolume", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundSetDayVolume);
    insert(568, "AmbientSoundSetNightVolume", R_VOID, {R_OBJECT, R_INT}, &AmbientSoundSetNightVolume);
    insert(569, "MusicBackgroundGetBattleTrack", R_INT, {R_OBJECT}, &MusicBackgroundGetBattleTrack);
    insert(570, "GetHasInventory", R_INT, {R_OBJECT}, &GetHasInventory);
    insert(571, "GetStrRefSoundDuration", R_FLOAT, {R_INT}, &GetStrRefSoundDuration);
    insert(572, "AddToParty", R_VOID, {R_OBJECT, R_OBJECT}, &AddToParty);
    insert(573, "RemoveFromParty", R_VOID, {R_OBJECT}, &RemoveFromParty);
    insert(574, "AddPartyMember", R_INT, {R_INT, R_OBJECT}, &AddPartyMember);
    insert(575, "RemovePartyMember", R_INT, {R_INT}, &RemovePartyMember);
    insert(576, "IsObjectPartyMember", R_INT, {R_OBJECT}, &IsObjectPartyMember);
    insert(577, "GetPartyMemberByIndex", R_OBJECT, {R_INT}, &GetPartyMemberByIndex);
    insert(578, "GetGlobalBoolean", R_INT, {R_STRING}, &GetGlobalBoolean);
    insert(579, "SetGlobalBoolean", R_VOID, {R_STRING, R_INT}, &SetGlobalBoolean);
    insert(580, "GetGlobalNumber", R_INT, {R_STRING}, &GetGlobalNumber);
    insert(581, "SetGlobalNumber", R_VOID, {R_STRING, R_INT}, &SetGlobalNumber);
    insert(582, "AurPostString", R_VOID, {R_STRING, R_INT, R_INT, R_FLOAT}, &AurPostString);
    insert(669, "AddJournalWorldEntry", R_VOID, {R_INT, R_STRING, R_STRING}, &AddJournalWorldEntry);
    insert(670, "AddJournalWorldEntryStrref", R_VOID, {R_INT, R_INT}, &AddJournalWorldEntryStrref);
    insert(671, "BarkString", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &BarkString);
    insert(672, "DeleteJournalWorldAllEntries", R_VOID, {}, &DeleteJournalWorldAllEntries);
    insert(673, "DeleteJournalWorldEntry", R_VOID, {R_INT}, &DeleteJournalWorldEntry);
    insert(674, "DeleteJournalWorldEntryStrref", R_VOID, {R_INT}, &DeleteJournalWorldEntryStrref);
    insert(677, "PlayVisualAreaEffect", R_VOID, {R_INT, R_LOCATION}, &PlayVisualAreaEffect);
    insert(678, "SetJournalQuestEntryPicture", R_VOID, {R_STRING, R_OBJECT, R_INT, R_INT, R_INT}, &SetJournalQuestEntryPicture);
    insert(679, "GetLocalBoolean", R_INT, {R_OBJECT, R_INT}, &GetLocalBoolean);
    insert(680, "SetLocalBoolean", R_VOID, {R_OBJECT, R_INT, R_INT}, &SetLocalBoolean);
    insert(681, "GetLocalNumber", R_INT, {R_OBJECT, R_INT}, &GetLocalNumber);
    insert(682, "SetLocalNumber", R_VOID, {R_OBJECT, R_INT, R_INT}, &SetLocalNumber);
    insert(689, "SoundObjectGetPitchVariance", R_FLOAT, {R_OBJECT}, &SoundObjectGetPitchVariance);
    insert(690, "SoundObjectSetPitchVariance", R_VOID, {R_OBJECT, R_FLOAT}, &SoundObjectSetPitchVariance);
    insert(691, "SoundObjectGetVolume", R_INT, {R_OBJECT}, &SoundObjectGetVolume);
    insert(692, "GetGlobalLocation", R_LOCATION, {R_STRING}, &GetGlobalLocation);
    insert(693, "SetGlobalLocation", R_VOID, {R_STRING, R_LOCATION}, &SetGlobalLocation);
    insert(694, "AddAvailableNPCByObject", R_INT, {R_INT, R_OBJECT}, &AddAvailableNPCByObject);
    insert(695, "RemoveAvailableNPC", R_INT, {R_INT}, &RemoveAvailableNPC);
    insert(696, "IsAvailableCreature", R_INT, {R_INT}, &IsAvailableCreature);
    insert(697, "AddAvailableNPCByTemplate", R_INT, {R_INT, R_STRING}, &AddAvailableNPCByTemplate);
    insert(698, "SpawnAvailableNPC", R_OBJECT, {R_INT, R_LOCATION}, &SpawnAvailableNPC);
    insert(699, "IsNPCPartyMember", R_INT, {R_INT}, &IsNPCPartyMember);
    insert(701, "GetIsConversationActive", R_INT, {}, &GetIsConversationActive);
    insert(704, "GetPartyAIStyle", R_INT, {}, &GetPartyAIStyle);
    insert(705, "GetNPCAIStyle", R_INT, {R_OBJECT}, &GetNPCAIStyle);
    insert(706, "SetPartyAIStyle", R_VOID, {R_INT}, &SetPartyAIStyle);
    insert(707, "SetNPCAIStyle", R_VOID, {R_OBJECT, R_INT}, &SetNPCAIStyle);
    insert(708, "SetNPCSelectability", R_VOID, {R_INT, R_INT}, &SetNPCSelectability);
    insert(709, "GetNPCSelectability", R_INT, {R_INT}, &GetNPCSelectability);
    insert(710, "ClearAllEffects", R_VOID, {}, &ClearAllEffects);
    insert(711, "GetLastConversation", R_STRING, {}, &GetLastConversation);
    insert(712, "ShowPartySelectionGUI", R_VOID, {R_STRING, R_INT, R_INT, R_INT}, &ShowPartySelectionGUI);
    insert(713, "GetStandardFaction", R_INT, {R_OBJECT}, &GetStandardFaction);
    insert(714, "GivePlotXP", R_VOID, {R_STRING, R_INT}, &GivePlotXP);
    insert(715, "GetMinOneHP", R_INT, {R_OBJECT}, &GetMinOneHP);
    insert(716, "SetMinOneHP", R_VOID, {R_OBJECT, R_INT}, &SetMinOneHP);
    insert(719, "SetGlobalFadeIn", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &SetGlobalFadeIn);
    insert(720, "SetGlobalFadeOut", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &SetGlobalFadeOut);
    insert(721, "GetLastHostileTarget", R_OBJECT, {R_OBJECT}, &GetLastHostileTarget);
    insert(722, "GetLastAttackAction", R_INT, {R_OBJECT}, &GetLastAttackAction);
    insert(723, "GetLastForcePowerUsed", R_INT, {R_OBJECT}, &GetLastForcePowerUsed);
    insert(724, "GetLastCombatFeatUsed", R_INT, {R_OBJECT}, &GetLastCombatFeatUsed);
    insert(725, "GetLastAttackResult", R_INT, {R_OBJECT}, &GetLastAttackResult);
    insert(726, "GetWasForcePowerSuccessful", R_INT, {R_OBJECT}, &GetWasForcePowerSuccessful);
    insert(727, "GetFirstAttacker", R_OBJECT, {R_OBJECT}, &GetFirstAttacker);
    insert(728, "GetNextAttacker", R_OBJECT, {R_OBJECT}, &GetNextAttacker);
    insert(729, "SetFormation", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &SetFormation);
    insert(731, "SetForcePowerUnsuccessful", R_VOID, {R_INT, R_OBJECT}, &SetForcePowerUnsuccessful);
    insert(732, "GetIsDebilitated", R_INT, {R_OBJECT}, &GetIsDebilitated);
    insert(733, "PlayMovie", R_VOID, {R_STRING, R_INT}, &PlayMovie);
    insert(734, "SaveNPCState", R_VOID, {R_INT}, &SaveNPCState);
    insert(735, "GetCategoryFromTalent", R_INT, {R_TALENT}, &GetCategoryFromTalent);
    insert(736, "SurrenderByFaction", R_VOID, {R_INT, R_INT}, &SurrenderByFaction);
    insert(737, "ChangeFactionByFaction", R_VOID, {R_INT, R_INT}, &ChangeFactionByFaction);
    insert(738, "PlayRoomAnimation", R_VOID, {R_STRING, R_INT}, &PlayRoomAnimation);
    insert(739, "ShowGalaxyMap", R_VOID, {R_INT}, &ShowGalaxyMap);
    insert(740, "SetPlanetSelectable", R_VOID, {R_INT, R_INT}, &SetPlanetSelectable);
    insert(741, "GetPlanetSelectable", R_INT, {R_INT}, &GetPlanetSelectable);
    insert(742, "SetPlanetAvailable", R_VOID, {R_INT, R_INT}, &SetPlanetAvailable);
    insert(743, "GetPlanetAvailable", R_INT, {R_INT}, &GetPlanetAvailable);
    insert(744, "GetSelectedPlanet", R_INT, {}, &GetSelectedPlanet);
    insert(745, "SoundObjectFadeAndStop", R_VOID, {R_OBJECT, R_FLOAT}, &SoundObjectFadeAndStop);
    insert(746, "SetAreaFogColor", R_VOID, {R_OBJECT, R_FLOAT, R_FLOAT, R_FLOAT}, &SetAreaFogColor);
    insert(747, "ChangeItemCost", R_VOID, {R_STRING, R_FLOAT}, &ChangeItemCost);
    insert(748, "GetIsLiveContentAvailable", R_INT, {R_INT}, &GetIsLiveContentAvailable);
    insert(749, "ResetDialogState", R_VOID, {}, &ResetDialogState);
    insert(750, "SetGoodEvilValue", R_VOID, {R_OBJECT, R_INT}, &SetGoodEvilValue);
    insert(751, "GetIsPoisoned", R_INT, {R_OBJECT}, &GetIsPoisoned);
    insert(752, "GetSpellTarget", R_OBJECT, {R_OBJECT}, &GetSpellTarget);
    insert(753, "SetSoloMode", R_VOID, {R_INT}, &SetSoloMode);
    insert(757, "CancelPostDialogCharacterSwitch", R_VOID, {}, &CancelPostDialogCharacterSwitch);
    insert(758, "SetMaxHitPoints", R_VOID, {R_OBJECT, R_INT}, &SetMaxHitPoints);
    insert(759, "NoClicksFor", R_VOID, {R_FLOAT}, &NoClicksFor);
    insert(760, "HoldWorldFadeInForDialog", R_VOID, {}, &HoldWorldFadeInForDialog);
    insert(761, "ShipBuild", R_INT, {}, &ShipBuild);
    insert(762, "SurrenderRetainBuffs", R_VOID, {}, &SurrenderRetainBuffs);
    insert(763, "SuppressStatusSummaryEntry", R_VOID, {R_INT}, &SuppressStatusSummaryEntry);
    insert(764, "GetCheatCode", R_INT, {R_INT}, &GetCheatCode);
    insert(765, "SetMusicVolume", R_VOID, {R_FLOAT}, &SetMusicVolume);
    insert(766, "CreateItemOnFloor", R_OBJECT, {R_STRING, R_LOCATION, R_INT}, &CreateItemOnFloor);
    insert(767, "SetAvailableNPCId", R_VOID, {R_INT, R_OBJECT}, &SetAvailableNPCId);
    insert(768, "GetScriptParameter", R_INT, {R_INT}, &GetScriptParameter);
    insert(769, "SetFadeUntilScript", R_VOID, {}, &SetFadeUntilScript);
    insert(771, "GetItemComponent", R_INT, {}, &GetItemComponent);
    insert(772, "GetItemComponentPieceValue", R_INT, {}, &GetItemComponentPieceValue);
    insert(773, "ShowChemicalUpgradeScreen", R_VOID, {R_OBJECT}, &ShowChemicalUpgradeScreen);
    insert(774, "GetChemicals", R_INT, {}, &GetChemicals);
    insert(775, "GetChemicalPieceValue", R_INT, {}, &GetChemicalPieceValue);
    insert(776, "GetSpellForcePointCost", R_INT, {}, &GetSpellForcePointCost);
    insert(783, "GetFeatAcquired", R_INT, {R_INT, R_OBJECT}, &GetFeatAcquired);
    insert(784, "GetSpellAcquired", R_INT, {R_INT, R_OBJECT}, &GetSpellAcquired);
    insert(785, "ShowSwoopUpgradeScreen", R_VOID, {}, &ShowSwoopUpgradeScreen);
    insert(786, "GrantFeat", R_VOID, {R_INT, R_OBJECT}, &GrantFeat);
    insert(787, "GrantSpell", R_VOID, {R_INT, R_OBJECT}, &GrantSpell);
    insert(788, "SpawnMine", R_VOID, {R_INT, R_LOCATION, R_INT, R_INT, R_OBJECT}, &SpawnMine);
    insert(791, "SetFakeCombatState", R_VOID, {R_OBJECT, R_INT}, &SetFakeCombatState);
    insert(793, "GetOwnerDemolitionsSkill", R_INT, {R_OBJECT}, &GetOwnerDemolitionsSkill);
    insert(794, "SetOrientOnClick", R_VOID, {R_OBJECT, R_INT}, &SetOrientOnClick);
    insert(795, "GetInfluence", R_INT, {R_INT}, &GetInfluence);
    insert(796, "SetInfluence", R_VOID, {R_INT, R_INT}, &SetInfluence);
    insert(797, "ModifyInfluence", R_VOID, {R_INT, R_INT}, &ModifyInfluence);
    insert(798, "GetRacialSubType", R_INT, {R_OBJECT}, &GetRacialSubType);
    insert(799, "IncrementGlobalNumber", R_VOID, {R_STRING, R_INT}, &IncrementGlobalNumber);
    insert(800, "DecrementGlobalNumber", R_VOID, {R_STRING, R_INT}, &DecrementGlobalNumber);
    insert(801, "SetBonusForcePoints", R_VOID, {R_OBJECT, R_INT}, &SetBonusForcePoints);
    insert(802, "AddBonusForcePoints", R_VOID, {R_OBJECT, R_INT}, &AddBonusForcePoints);
    insert(803, "GetBonusForcePoints", R_VOID, {R_OBJECT}, &GetBonusForcePoints);
    insert(805, "IsMoviePlaying", R_INT, {}, &IsMoviePlaying);
    insert(806, "QueueMovie", R_VOID, {R_STRING, R_INT}, &QueueMovie);
    insert(807, "PlayMovieQueue", R_VOID, {R_INT}, &PlayMovieQueue);
    insert(808, "YavinHackDoorClose", R_VOID, {R_OBJECT}, &YavinHackDoorClose);
    insert(810, "IsStealthed", R_INT, {R_OBJECT}, &IsStealthed);
    insert(811, "IsMeditating", R_INT, {R_OBJECT}, &IsMeditating);
    insert(812, "IsInTotalDefense", R_INT, {R_OBJECT}, &IsInTotalDefense);
    insert(813, "SetHealTarget", R_VOID, {R_OBJECT, R_OBJECT}, &SetHealTarget);
    insert(814, "GetHealTarget", R_OBJECT, {R_OBJECT}, &GetHealTarget);
    insert(815, "GetRandomDestination", R_VECTOR, {R_OBJECT, R_INT}, &GetRandomDestination);
    insert(816, "IsFormActive", R_INT, {R_OBJECT, R_INT}, &IsFormActive);
    insert(817, "GetSpellFormMask", R_INT, {R_INT}, &GetSpellFormMask);
    insert(818, "GetSpellBaseForcePointCost", R_INT, {R_INT}, &GetSpellBaseForcePointCost);
    insert(819, "SetKeepStealthInDialog", R_VOID, {R_INT}, &SetKeepStealthInDialog);
    insert(820, "HasLineOfSight", R_INT, {R_VECTOR, R_VECTOR, R_OBJECT, R_OBJECT}, &HasLineOfSight);
    insert(821, "ShowDemoScreen", R_INT, {R_STRING, R_INT, R_INT, R_INT, R_INT}, &ShowDemoScreen);
    insert(822, "ForceHeartbeat", R_VOID, {R_OBJECT}, &ForceHeartbeat);
    insert(824, "IsRunning", R_INT, {R_OBJECT}, &IsRunning);
    insert(826, "SetForfeitConditions", R_VOID, {R_INT}, &SetForfeitConditions);
    insert(827, "GetLastForfeitViolation", R_INT, {}, &GetLastForfeitViolation);
    insert(828, "ModifyReflexSavingThrowBase", R_VOID, {R_OBJECT, R_INT}, &ModifyReflexSavingThrowBase);
    insert(829, "ModifyFortitudeSavingThrowBase", R_VOID, {R_OBJECT, R_INT}, &ModifyFortitudeSavingThrowBase);
    insert(830, "ModifyWillSavingThrowBase", R_VOID, {R_OBJECT, R_INT}, &ModifyWillSavingThrowBase);
    insert(831, "GetScriptStringParameter", R_STRING, {}, &GetScriptStringParameter);
    insert(832, "GetObjectPersonalSpace", R_FLOAT, {R_OBJECT}, &GetObjectPersonalSpace);
    insert(833, "AdjustCreatureAttributes", R_VOID, {R_OBJECT, R_INT, R_INT}, &AdjustCreatureAttributes);
    insert(834, "SetCreatureAILevel", R_VOID, {R_OBJECT, R_INT}, &SetCreatureAILevel);
    insert(835, "ResetCreatureAILevel", R_VOID, {R_OBJECT}, &ResetCreatureAILevel);
    insert(836, "AddAvailablePUPByTemplate", R_INT, {R_INT, R_STRING}, &AddAvailablePUPByTemplate);
    insert(837, "AddAvailablePUPByObject", R_INT, {R_INT, R_OBJECT}, &AddAvailablePUPByObject);
    insert(838, "AssignPUP", R_INT, {R_INT, R_INT}, &AssignPUP);
    insert(839, "SpawnAvailablePUP", R_OBJECT, {R_INT, R_LOCATION}, &SpawnAvailablePUP);
    insert(840, "AddPartyPuppet", R_INT, {R_INT, R_OBJECT}, &AddPartyPuppet);
    insert(841, "GetPUPOwner", R_OBJECT, {R_OBJECT}, &GetPUPOwner);
    insert(842, "GetIsPuppet", R_INT, {R_OBJECT}, &GetIsPuppet);
    insert(844, "GetIsPartyLeader", R_INT, {R_OBJECT}, &GetIsPartyLeader);
    insert(845, "GetPartyLeader", R_OBJECT, {}, &GetPartyLeader);
    insert(846, "RemoveNPCFromPartyToBase", R_INT, {R_INT}, &RemoveNPCFromPartyToBase);
    insert(847, "CreatureFlourishWeapon", R_VOID, {R_OBJECT}, &CreatureFlourishWeapon);
    insert(850, "ChangeObjectAppearance", R_VOID, {R_OBJECT, R_INT}, &ChangeObjectAppearance);
    insert(851, "GetIsXBox", R_INT, {}, &GetIsXBox);
    insert(854, "PlayOverlayAnimation", R_VOID, {R_OBJECT, R_INT}, &PlayOverlayAnimation);
    insert(855, "UnlockAllSongs", R_VOID, {}, &UnlockAllSongs);
    insert(856, "DisableMap", R_VOID, {R_INT}, &DisableMap);
    insert(857, "DetonateMine", R_VOID, {R_OBJECT}, &DetonateMine);
    insert(858, "DisableHealthRegen", R_VOID, {R_INT}, &DisableHealthRegen);
    insert(859, "SetCurrentForm", R_VOID, {R_OBJECT, R_INT}, &SetCurrentForm);
    insert(860, "SetDisableTransit", R_VOID, {R_INT}, &SetDisableTransit);
    insert(861, "SetInputClass", R_VOID, {R_INT}, &SetInputClass);
    insert(862, "SetForceAlwaysUpdate", R_VOID, {R_OBJECT, R_INT}, &SetForceAlwaysUpdate);
    insert(863, "EnableRain", R_VOID, {R_INT}, &EnableRain);
    insert(864, "DisplayMessageBox", R_VOID, {R_INT, R_STRING}, &DisplayMessageBox);
    insert(865, "DisplayDatapad", R_VOID, {R_OBJECT}, &DisplayDatapad);
    insert(866, "RemoveHeartbeat", R_VOID, {R_OBJECT}, &RemoveHeartbeat);
    insert(867, "RemoveEffectByID", R_VOID, {R_OBJECT, R_INT}, &RemoveEffectByID);
    insert(868, "RemoveEffectByExactMatch", R_VOID, {R_OBJECT, R_EFFECT}, &RemoveEffectByExactMatch);
    insert(869, "AdjustCreatureSkills", R_VOID, {R_OBJECT, R_INT, R_INT}, &AdjustCreatureSkills);
    insert(870, "GetSkillRankBase", R_INT, {R_INT, R_OBJECT}, &GetSkillRankBase);
    insert(871, "EnableRendering", R_VOID, {R_OBJECT, R_INT}, &EnableRendering);
    insert(872, "GetCombatActionsPending", R_INT, {R_OBJECT}, &GetCombatActionsPending);
    insert(873, "SaveNPCByObject", R_VOID, {R_INT, R_OBJECT}, &SaveNPCByObject);
    insert(874, "SavePUPByObject", R_VOID, {R_INT, R_OBJECT}, &SavePUPByObject);
    insert(875, "GetIsPlayerMadeCharacter", R_INT, {R_OBJECT}, &GetIsPlayerMadeCharacter);
    insert(876, "RebuildPartyTable", R_VOID, {}, &RebuildPartyTable);
}

} // namespace game

} // namespace reone
