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

#include "reone/game/script/routine/declarations.h"

#include "reone/resource/di/services.h"
#include "reone/resource/strings.h"
#include "reone/script/exception/unsupportedroutine.h"
#include "reone/script/executioncontext.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/logutil.h"
#include "reone/system/randomutil.h"

#include "reone/game/action/factory.h"
#include "reone/game/combat.h"
#include "reone/game/di/services.h"
#include "reone/game/event.h"
#include "reone/game/game.h"
#include "reone/game/location.h"
#include "reone/game/object.h"
#include "reone/game/object/door.h"
#include "reone/game/party.h"
#include "reone/game/reputes.h"
#include "reone/game/script/runner.h"
#include "reone/game/talent.h"

#include "reone/game/script/routine/argutil.h"
#include "reone/game/script/routine/context.h"
#include "reone/game/script/routine/objectutil.h"

#define R_FALSE 0
#define R_TRUE 1

using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

static constexpr bool kShipBuild = true;

Variable unsupported(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw UnsupportedRoutineException();
}

Variable random(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int maxInteger = getInt(args, 0);
    int result = reone::random(0, maxInteger - 1);

    return Variable::ofInt(result);
}

Variable printString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string str = getString(args, 0);
    info(str);

    return Variable::ofNull();
}

Variable floatToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float flt = getFloat(args, 0);
    int width = getIntOrElse(args, 1, 18);
    int decimals = getIntOrElse(args, 2, 9);

    // TODO: use width and decimals
    return Variable::ofString(std::to_string(flt));
}

Variable assignCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto actionSubject = getObject(args, 0, ctx);
    auto actionToAssign = getAction(args, 1);

    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(actionToAssign));
    actionSubject->addAction(std::move(commandAction));

    return Variable::ofNull();
}

Variable delayCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);
    auto actionToDelay = getAction(args, 1);

    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(actionToDelay));
    getCaller(ctx)->delayAction(std::move(commandAction), seconds);

    return Variable::ofNull();
}

Variable executeScript(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string script = getString(args, 0);
    auto target = getObject(args, 1, ctx);
    int scriptVar = getIntOrElse(args, 2, -1);

    ctx.game.scriptRunner().run(script, target->id(), kObjectInvalid, kObjectInvalid, scriptVar);

    return Variable::ofNull();
}

Variable clearAllActions(const std::vector<Variable> &args, const RoutineContext &ctx) {
    getCaller(ctx)->clearAllActions();
    return Variable::ofNull();
}

Variable setFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    float direction = getFloat(args, 0);

    caller->setFacing(glm::radians(direction));

    return Variable::ofNull();
}

Variable switchPlayerCharacter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    throw NotImplementedException();
}

Variable setPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    ctx.game.party().setPartyLeader(npc);

    return Variable::ofNull();
}

Variable setAreaUnescapable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool unescapable = getIntAsBool(args, 0);
    ctx.game.module()->area()->setUnescapable(unescapable);

    return Variable::ofNull();
}

Variable getAreaUnescapable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool unescapable = ctx.game.module()->area()->isUnescapable();
    return Variable::ofInt(static_cast<int>(unescapable));
}

Variable getArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = ctx.game.module()->area();
    return Variable::ofObject(getObjectIdOrInvalid(area));
}

Variable getEnteringObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getExitingObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    return Variable::ofVector(target->position());
}

Variable getFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    float facing = glm::degrees(target->getFacing());

    return Variable::ofFloat(facing);
}

Variable getItemPossessor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getItemPossessedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    std::string itemTag = boost::to_lower_copy(getString(args, 1));
    if (itemTag.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }

    auto item = creature->getItemByTag(itemTag);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable createItemOnObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string itemTemplate = boost::to_lower_copy(getString(args, 0));
    if (itemTemplate.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }

    auto target = getObjectOrCaller(args, 1, ctx);
    int stackSize = getIntOrElse(args, 2, 1);
    bool hideMessage = getIntAsBoolOrElse(args, 3, 0);

    // TODO: use hideMessage

    auto item = target->addItem(itemTemplate, stackSize, true);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getLastAttacker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attackee = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getNearestCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto target = getObjectOrCaller(args, 2, ctx);
    int nth = getIntOrElse(args, 3, 1);
    int secondCriteriaType = getIntOrElse(args, 4, -1);
    int secondCriteriaValue = getIntOrElse(args, 5, -1);
    int thirdCriteriaType = getIntOrElse(args, 6, -1);
    int thirdCriteriaValue = getIntOrElse(args, 7, -1);

    Area::SearchCriteriaList criterias;
    criterias.push_back(std::make_pair(static_cast<CreatureType>(firstCriteriaType), firstCriteriaValue));
    if (secondCriteriaType != -1) {
        criterias.push_back(std::make_pair(static_cast<CreatureType>(secondCriteriaType), secondCriteriaValue));
    }
    if (thirdCriteriaType != -1) {
        criterias.push_back(std::make_pair(static_cast<CreatureType>(thirdCriteriaType), thirdCriteriaValue));
    }
    auto creature = ctx.game.module()->area()->getNearestCreature(target, criterias, nth - 1);

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable getDistanceToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    auto object = getObject(args, 0, ctx);

    return Variable::ofFloat(caller->getDistanceTo(*object));
}

Variable getIsObjectValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Object, args[0].type);

    uint32_t objectId = args[0].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    }
    auto object = ctx.game.getObjectById(objectId);

    return Variable::ofInt(static_cast<int>(object != nullptr));
}

Variable playSound(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string soundName = getString(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSpellTargetObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getCurrentHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    int hitPoints = object->currentHitPoints();

    return Variable::ofInt(hitPoints);
}

Variable getMaxHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    int hitPoints = object->maxHitPoints();

    return Variable::ofInt(hitPoints);
}

Variable getSubScreenID(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable cancelCombat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    throw NotImplementedException();
}

Variable getCurrentForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);

    throw NotImplementedException();
}

Variable getMaxForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);

    throw NotImplementedException();
}

Variable pauseGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool pause = getIntAsBool(args, 0);

    throw NotImplementedException();
}

Variable setPlayerRestrictMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool restrict = getIntAsBool(args, 0);
    ctx.game.module()->player().setRestrictMode(restrict);

    return Variable::ofNull();
}

Variable getStringLength(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string str = getString(args, 0);
    int result = str.length();

    return Variable::ofInt(static_cast<int>(result));
}

Variable getStringRight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string str = getString(args, 0);
    int count = getInt(args, 1);

    std::string result;
    if (str.size() >= count) {
        result = str.substr(str.length() - count, count);
    }

    return Variable::ofString(std::move(result));
}

Variable getStringLeft(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string str = getString(args, 0);
    int count = getInt(args, 1);

    std::string result;
    if (str.size() >= count) {
        result = str.substr(0, count);
    }

    return Variable::ofString(std::move(result));
}

Variable getSubString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string str = getString(args, 0);
    int start = getInt(args, 1);
    int count = getInt(args, 2);

    return Variable::ofString(str.substr(start, count));
}

Variable findSubString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string str = getString(args, 0);
    std::string subString = getString(args, 1);
    size_t pos = str.find(subString);

    return Variable::ofInt(pos != std::string::npos ? static_cast<int>(pos) : -1);
}

Variable abs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    int result = glm::abs(value);

    return Variable::ofInt(result);
}

Variable getPlayerRestrictMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);

    // TODO: why is this object necessary?

    return Variable::ofInt(static_cast<int>(ctx.game.module()->player().isRestrictMode()));
}

Variable getCasterLevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    throw NotImplementedException();
}

Variable getFirstEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofEffect(creature->getFirstEffect());
}

Variable getNextEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofEffect(creature->getNextEffect());
}

Variable removeEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto effect = getEffect(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsEffectValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Effect, args[0].type);

    auto effect = std::static_pointer_cast<Effect>(args[0].engineType);

    return Variable::ofInt(static_cast<int>(effect && effect->type() != EffectType::Invalid));
}

Variable getEffectCreator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable intToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int integer = getInt(args, 0);

    return Variable::ofString(std::to_string(integer));
}

Variable getFirstObjectInArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsAreaOrCallerArea(args, 0, ctx);
    int objectFilter = getIntOrElse(args, 1, static_cast<int>(ObjectType::Creature));

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getNextObjectInArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsAreaOrCallerArea(args, 0, ctx);
    int objectFilter = getIntOrElse(args, 1, static_cast<int>(ObjectType::Creature));

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable d2(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return Variable::ofInt(result);
}

Variable d3(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return Variable::ofInt(result);
}

Variable d4(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return Variable::ofInt(result);
}

Variable d6(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return Variable::ofInt(result);
}

Variable d8(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return Variable::ofInt(result);
}

Variable d10(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return Variable::ofInt(result);
}

Variable d12(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return Variable::ofInt(result);
}

Variable d20(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return Variable::ofInt(result);
}

Variable d100(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return Variable::ofInt(result);
}

Variable getMetaMagicFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->type()));
}

Variable getRacialType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->racialType()));
}

Variable fortitudeSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto saveType = getIntAsEnumOrElse(args, 1, SavingThrowType::None);
    auto saveVersus = getObjectOrCaller(args, 2, ctx);

    throw NotImplementedException();
}

Variable reflexSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto saveType = getIntAsEnumOrElse(args, 1, SavingThrowType::None);
    auto saveVersus = getObjectOrCaller(args, 2, ctx);

    throw NotImplementedException();
}

Variable willSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto saveType = getIntAsEnumOrElse(args, 1, SavingThrowType::None);
    auto saveVersus = getObjectOrCaller(args, 2, ctx);

    throw NotImplementedException();
}

Variable getSpellSaveDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable roundsToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int rounds = getInt(args, 0);
    return Variable::ofFloat(rounds / 6.0f);
}

Variable hoursToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int hours = getInt(args, 0);
    return Variable::ofInt(hours * 3600);
}

Variable soundObjectSetFixedVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    float fixedVariance = getFloat(args, 1);

    throw NotImplementedException();
}

Variable getGoodEvilValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getPartyMemberCount(const std::vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.game.party().getSize());
}

Variable getAlignmentGoodEvil(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    throw NotImplementedException();
}

Variable getFirstObjectInShape(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto shape = getIntAsEnum<Shape>(args, 0);
    float size = getFloat(args, 1);
    auto target = getLocationArgument(args, 2);
    bool lineOfSight = getIntAsBoolOrElse(args, 3, false);
    int objectFilter = getIntOrElse(args, 4, static_cast<int>(ObjectType::Creature));
    auto origin = getVectorOrElse(args, 5, glm::vec3(0.0f));

    throw NotImplementedException();
}

Variable getNextObjectInShape(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto shape = getIntAsEnum<Shape>(args, 0);
    float size = getFloat(args, 1);
    auto target = getLocationArgument(args, 2);
    bool lineOfSight = getIntAsBoolOrElse(args, 3, false);
    int objectFilter = getIntOrElse(args, 4, static_cast<int>(ObjectType::Creature));
    auto origin = getVectorOrElse(args, 5, glm::vec3(0.0f));

    throw NotImplementedException();
}

Variable signalEvent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto evToRun = getEvent(args, 1);

    debug(boost::format("Event signalled: %s %s") % object->tag() % evToRun->number(), LogChannels::script);
    ctx.game.scriptRunner().run(object->getOnUserDefined(), object->id(), kObjectInvalid, evToRun->number());

    return Variable::ofNull();
}

Variable eventUserDefined(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int userDefinedEventNumber = getInt(args, 0);
    auto event = std::make_shared<Event>(userDefinedEventNumber);

    return Variable::ofEvent(std::move(event));
}

Variable vectorNormalize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto vector = getVector(args, 0);
    return Variable::ofVector(glm::normalize(vector));
}

Variable getItemStackSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    return Variable::ofInt(item->stackSize());
}

Variable getAbilityScore(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto abilityType = getIntAsEnum<Ability>(args, 1);
    int result = creature->attributes().getAbilityScore(abilityType);

    return Variable::ofInt(result);
}

Variable getIsDead(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool dead = creature->isDead();

    return Variable::ofInt(static_cast<int>(dead));
}

Variable vectorCreate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float x = getFloatOrElse(args, 0, 0.0f);
    float y = getFloatOrElse(args, 1, 0.0f);
    float z = getFloatOrElse(args, 2, 0.0f);

    return Variable::ofVector(glm::vec3(x, y, z));
}

Variable setFacingPoint(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    auto target = getVector(args, 0);

    caller->face(target);

    return Variable::ofNull();
}

Variable angleToVector(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float angle = glm::radians(getFloat(args, 0));
    auto vector = glm::vec3(glm::cos(angle), glm::sin(angle), 0.0f);

    return Variable::ofVector(std::move(vector));
}

Variable setItemStackSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    int stackSize = glm::max(1, getInt(args, 1));

    item->setStackSize(stackSize);

    return Variable::ofNull();
}

Variable getDistanceBetween(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto objectA = getObject(args, 0, ctx);
    auto objectB = getObject(args, 1, ctx);

    return Variable::ofFloat(objectA->getDistanceTo(*objectB));
}

Variable setReturnStrref(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool show = getIntAsBool(args, 0);
    int stringRef = getIntOrElse(args, 1, 0);
    int returnQueryStrRef = getIntOrElse(args, 1, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getItemInSlot(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int slot = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    auto item = creature->getEquippedItem(slot);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable setGlobalString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    std::string value = getString(args, 1);

    ctx.game.setGlobalString(identifier, value);

    return Variable::ofNull();
}

Variable setCommandable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool commandable = getIntAsBool(args, 0);
    auto target = getObjectOrCaller(args, 1, ctx);

    target->setCommandable(commandable);

    return Variable::ofNull();
}

Variable getCommandable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isCommandable()));
}

Variable getHitDice(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(creature->attributes().getAggregateLevel());
}

Variable getTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofString(object->tag());
}

Variable resistForce(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto source = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getEffectType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);
    return Variable::ofInt(static_cast<int>(effect->type()));
}

Variable getFactionEqual(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto firstObject = getObjectAsCreature(args, 0, ctx);
    auto secondObject = getObjectOrCallerAsCreature(args, 1, ctx);

    return Variable::ofInt(static_cast<int>(firstObject->faction() == secondObject->faction()));
}

Variable setListening(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool value = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setListenPattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    std::string pattern = getString(args, 1);
    int number = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getFactionAverageReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sourceFactionMember = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getGlobalString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    std::string result = ctx.game.getGlobalString(identifier);

    return Variable::ofString(std::move(result));
}

Variable getListenPatternNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getWaypointByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string waypointTag = boost::to_lower_copy(getString(args, 0));

    std::shared_ptr<Object> result;
    for (auto &object : ctx.game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (object->tag() == waypointTag) {
            result = object;
            break;
        }
    }

    return Variable::ofObject(getObjectIdOrInvalid(result));
}

Variable getObjectByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string tag = boost::to_lower_copy(getString(args, 0));
    int nth = getIntOrElse(args, 1, 0);

    std::shared_ptr<Object> object;
    if (!tag.empty()) {
        object = ctx.game.module()->area()->getObjectByTag(tag, nth);
    } else {
        object = ctx.game.party().player();
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable adjustAlignment(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto subject = getObject(args, 0, ctx);
    auto alignment = getIntAsEnum<Alignment>(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto source = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable adjustReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    auto sourceFactionMember = getObject(args, 1, ctx);
    auto adjustment = getInt(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable getModuleFileName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto location = std::make_shared<Location>(object->position(), object->getFacing());

    return Variable::ofLocation(std::move(location));
}

Variable location(const std::vector<Variable> &args, const RoutineContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));
    auto location = std::make_shared<Location>(std::move(position), orientation);

    return Variable::ofLocation(location);
}

Variable applyEffectAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto durationType = getIntAsEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto location = getLocationArgument(args, 2);
    float duration = getFloatOrElse(args, 3, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool result = creature == ctx.game.party().player();

    return Variable::ofInt(static_cast<int>(result));
}

Variable applyEffectToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto durationType = getIntAsEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto target = getObject(args, 2, ctx);
    float duration = getFloatOrElse(args, 3, 0.0f);

    target->applyEffect(effect, durationType, duration);

    return Variable::ofNull();
}

Variable speakString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string stringToSpeak = getString(args, 0);
    auto talkVolume = getIntAsEnumOrElse(args, 1, TalkVolume::Talk);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSpellTargetLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getPositionFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationArgument(args, 0);
    return Variable::ofVector(location->position());
}

Variable getFacingFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationArgument(args, 0);
    float result = glm::degrees(location->facing());

    return Variable::ofFloat(result);
}

Variable getNearestObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto objectType = getIntAsEnumOrElse(args, 0, ObjectType::All);
    auto target = getObjectOrCaller(args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    auto object = ctx.game.module()->area()->getNearestObject(target->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    });

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string tag = boost::to_lower_copy(getString(args, 0));
    auto target = getObjectOrCaller(args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    auto object = ctx.game.module()->area()->getNearestObject(target->position(), nth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    });

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable intToFloat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int integer = getInt(args, 0);
    return Variable::ofFloat(static_cast<float>(integer));
}

Variable floatToInt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofInt(static_cast<int>(value));
}

Variable stringToInt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string number = getString(args, 0);

    int result = 0;
    if (!number.empty()) {
        result = stoi(number);
    }

    return Variable::ofInt(result);
}

Variable getIsEnemy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool result = ctx.services.game.reputes.getIsEnemy(*target, *source);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsFriend(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool result = ctx.services.game.reputes.getIsFriend(*target, *source);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsNeutral(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool result = ctx.services.game.reputes.getIsNeutral(*target, *source);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getPCSpeaker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getStringByStrRef(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);
    std::string result = ctx.services.resource.strings.get(strRef);

    return Variable::ofString(std::move(result));
}

Variable destroyObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto destroy = getObject(args, 0, ctx);
    float delay = getFloatOrElse(args, 1, 0.0f);
    bool noFade = getIntAsBoolOrElse(args, 2, false);
    float delayUntilFade = getFloatOrElse(args, 3, 0.0f);
    int hideFeedback = getIntOrElse(args, 4, 0);

    // TODO: use delay, noFade, delayUntilFade and hideFeedback
    ctx.game.module()->area()->destroyObject(*destroy);

    return Variable::ofNull();
}

Variable getModule(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto module = ctx.game.module();
    return Variable::ofObject(getObjectIdOrInvalid(module));
}

Variable createObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto objectType = getIntAsEnum<ObjectType>(args, 0);
    std::string tmplt = boost::to_lower_copy(getString(args, 1));
    auto location = getLocationArgument(args, 2);
    bool useAppearAnimation = getIntAsBoolOrElse(args, 3, false);

    // TODO: use useAppearAnimation
    auto object = ctx.game.module()->area()->createObject(objectType, tmplt, location);

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable eventSpellCastAt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caster = getObject(args, 0, ctx);
    int spell = getInt(args, 1);
    bool harmful = getIntAsBoolOrElse(args, 2, true);

    throw NotImplementedException();
}

Variable getLastSpellCaster(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getUserDefinedEventNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.execution.userDefinedEventNumber);
}

Variable getSpellId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLoadFromSaveGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(static_cast<int>(false));
}

Variable getName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofString(object->name());
}

Variable getLastSpeaker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable beginConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string resRef = boost::to_lower_copy(getStringOrElse(args, 0, ""));
    auto objectToDialog = getObjectOrNull(args, 1, ctx);

    throw NotImplementedException();
}

Variable getLastPerceived(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto object = caller->perception().lastPerceived;

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getLastPerceptionSeen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool seen = caller->perception().lastPerception == PerceptionType::Seen;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getLastClosedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getLastPerceptionVanished(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool vanished = caller->perception().lastPerception == PerceptionType::NotSeen;

    return Variable::ofInt(static_cast<int>(vanished));
}

Variable getFirstInPersistentObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto persistentObject = getObjectOrCaller(args, 0, ctx);
    auto residentObjectType = getIntAsEnumOrElse(args, 1, ObjectType::Creature);
    auto persistentZone = getIntAsEnumOrElse(args, 2, PersistentZone::Active);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getNextInPersistentObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto persistentObject = getObjectOrCaller(args, 0, ctx);
    auto residentObjectType = getIntAsEnumOrElse(args, 1, ObjectType::Creature);
    auto persistentZone = getIntAsEnumOrElse(args, 2, PersistentZone::Active);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable showLevelUpGUI(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setItemNonEquippable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    bool nonEquippable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setButtonMashCheck(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool check = getIntAsBool(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable giveItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable objectToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    std::string result = str(boost::format("%x") % object->id());

    return Variable::ofString(std::move(result));
}

Variable getIsImmune(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto immunityType = getIntAsEnum<ImmunityType>(args, 1);
    auto versus = getObject(args, 2, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getModuleItemAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getEncounterActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto encounter = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setEncounterActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool newValue = getIntAsBool(args, 0);
    auto encounter = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable setCustomToken(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int customTokenNumber = getInt(args, 0);
    std::string tokenValue = getString(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getHasFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto feat = getIntAsEnum<FeatType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasFeat(feat)));
}

Variable getHasSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);
    auto skill = getIntAsEnum<SkillType>(args, 0);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasSkill(skill)));
}

Variable getObjectSeen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool seen = source->perception().seen.count(target) > 0;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getLastPlayerDied(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getReflexAdjustedDamage(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int damage = getInt(args, 0);
    auto target = getObject(args, 1, ctx);
    int dc = getInt(args, 2);
    auto saveType = getIntAsEnumOrElse(args, 3, SavingThrowType::None);
    auto saveVersus = getObject(args, 4, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable playAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    auto animType = getIntAsEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float seconds = getFloatOrElse(args, 2, 0.0f);

    // TODO: use seconds
    AnimationProperties properties;
    properties.speed = speed;
    caller->playAnimation(animType, std::move(properties));

    return Variable::ofNull();
}

Variable talentSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int spell = getInt(args, 0);
    auto talent = std::make_shared<Talent>(TalentType::Spell, spell);

    return Variable::ofTalent(std::move(talent));
}

Variable talentFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int feat = getInt(args, 0);
    auto talent = std::make_shared<Talent>(TalentType::Feat, feat);

    return Variable::ofTalent(std::move(talent));
}

Variable getHasSpellEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int spell = getInt(args, 0);
    auto object = getObjectOrCaller(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getEffectSpellId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getCreatureHasTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getCreatureTalentRandom(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int category = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);
    int inclusion = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofTalent(nullptr);
}

Variable getCreatureTalentBest(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int category = getInt(args, 0);
    int crMax = getInt(args, 1);
    auto creature = getObjectOrCallerAsCreature(args, 2, ctx);
    int inclusion = getIntOrElse(args, 3, 0);
    int excludeType = getIntOrElse(args, 4, -1);
    int excludeId = getIntOrElse(args, 5, -1);

    // TODO: implement

    return Variable::ofTalent(nullptr);
}

Variable jumpToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);

    auto action = ctx.game.actionFactory().newJumpToLocation(std::move(destination));
    getCaller(ctx)->addActionOnTop(std::move(action));

    return Variable::ofNull();
}

Variable getSkillRank(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCallerAsCreature(args, 1, ctx);
    auto skill = getIntAsEnum<SkillType>(args, 0);

    return Variable::ofInt(object->attributes().getSkillRank(skill));
}

Variable getAttackTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);
    auto target = creature->getAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getDistanceBetween2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto objectA = getObject(args, 0, ctx);
    auto objectB = getObject(args, 1, ctx);
    float result = objectA->getDistanceTo(glm::vec2(objectB->position()));

    return Variable::ofFloat(result);
}

Variable getIsInCombat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->isInCombat()));
}

Variable giveGoldToCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto gp = getInt(args, 1);

    creature->giveGold(gp);

    return Variable::ofNull();
}

Variable setIsDestroyable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool destroyabe = getIntAsBool(args, 0);
    bool raiseable = getIntAsBoolOrElse(args, 1, true);
    bool selectableWhenDead = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsDoor(args, 0, ctx);
    bool locked = getIntAsBool(args, 1);

    target->setLocked(locked);

    return Variable::ofNull();
}

Variable getLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsDoor(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isLocked()));
}

Variable getLastWeaponUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastUsedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getAbilityModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto ability = getIntAsEnum<Ability>(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getDistanceToObject2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    auto caller = getCaller(ctx);
    float result = caller->getDistanceTo(glm::vec2(object->position()));

    return Variable::ofFloat(result);
}

Variable getBlockingDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getIsDoorActionPossible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto targetDoor = getObjectAsDoor(args, 0, ctx);
    auto doorAction = getIntAsEnum<DoorAction>(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable doDoorAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto targetDoor = getObjectAsDoor(args, 0, ctx);
    auto doorAction = getIntAsEnum<DoorAction>(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getFirstItemInInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    auto item = target->getFirstItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getNextItemInInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    auto item = target->getNextItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getClassByPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int classPosition = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    auto result = creature->attributes().getClassByPosition(classPosition);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getLevelByPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int classPosition = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    int result = creature->attributes().getLevelByPosition(classPosition);

    return Variable::ofInt(result);
}

Variable getLevelByClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto classType = getIntAsEnum<ClassType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    int result = creature->attributes().getClassLevel(classType);

    return Variable::ofInt(result);
}

Variable getDamageDealtByType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto damageType = getIntAsEnum<DamageType>(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getTotalDamageDealt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable getLastDamager(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getInventoryDisturbType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getInventoryDisturbItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable showUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItemOrNull(args, 0, ctx);
    auto character = getObjectOrNull(args, 1, ctx);
    bool disableItemCreation = getIntAsBoolOrElse(args, 2, false);
    bool disableUpgrade = getIntAsBoolOrElse(args, 3, false);
    std::string override2DA = getStringOrElse(args, 4, "");

    // TODO: implement

    return Variable::ofNull();
}

Variable getGender(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->gender()));
}

Variable getIsTalentValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Talent, args[0].type);
    auto talent = std::static_pointer_cast<Talent>(args[0].engineType);
    return Variable::ofInt(static_cast<int>(talent && talent->type() != TalentType::Invalid));
}

Variable getAttemptedAttackTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto target = caller->getAttemptedAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getTypeFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);
    return Variable::ofInt(static_cast<int>(talent->type()));
}

Variable getIdFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);
    throw NotImplementedException();
}

Variable playPazaak(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int opponentPazaakDeck = getInt(args, 0);
    std::string endScript = getString(args, 1);
    int maxWager = getInt(args, 2);
    bool showTutorial = getIntAsBoolOrElse(args, 3, false);
    auto opponent = getObjectOrNull(args, 4, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastPazaakResult(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable displayFeedBackText(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int textConstant = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable addJournalQuestEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string plotId = getString(args, 0);
    int state = getInt(args, 1);
    bool allowOverrideHigher = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeJournalQuestEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string plotId = getString(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getJournalEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string plotId = getString(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable playRumblePattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int pattern = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable stopRumblePattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int pattern = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getAttemptedSpellTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastOpenedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getHasSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: Force Powers are not implemented at the moment

    return Variable::ofInt(R_FALSE);
}

Variable openStore(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto store = getObject(args, 0, ctx);
    auto pc = getObject(args, 1, ctx);
    int bonusMarkUp = getIntOrElse(args, 2, 0);
    int bonusMarkDown = getIntOrElse(args, 3, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable jumpToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto toJumpTo = getObject(args, 0, ctx);
    bool walkStraightLineToPoint = getIntAsBoolOrElse(args, 1, true);

    auto action = ctx.game.actionFactory().newJumpToObject(std::move(toJumpTo), walkStraightLineToPoint);
    getCaller(ctx)->addActionOnTop(std::move(action));

    return Variable::ofNull();
}

Variable setMapPinEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto mapPin = getObject(args, 0, ctx);
    bool enabled = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable addMultiClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int classType = getInt(args, 0);
    auto source = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsLinkImmune(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    auto effect = getEffect(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable giveXPToCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->giveXP(xpAmount);

    return Variable::ofNull();
}

Variable setXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->setXP(xpAmount);

    return Variable::ofNull();
}

Variable getXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(creature->xp());
}

Variable getBaseItemType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    return Variable::ofInt(item->baseItemType());
}

Variable getItemHasItemProperty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto property = getIntAsEnum<ItemProperty>(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsEncounterCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable changeToStandardFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creatureToChange = getObjectAsCreature(args, 0, ctx);
    auto standardFaction = getIntAsEnum<Faction>(args, 1);

    creatureToChange->setFaction(standardFaction);

    return Variable::ofNull();
}

Variable soundObjectPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    sound->setActive(true);

    return Variable::ofNull();
}

Variable soundObjectStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    sound->setActive(false);

    return Variable::ofNull();
}

Variable soundObjectSetVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    int volume = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable soundObjectSetPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    auto position = getVector(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getGold(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCallerAsCreature(args, 0, ctx);
    return Variable::ofInt(target->gold());
}

Variable setLightsaberPowered(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool ovrrd = getIntAsBool(args, 1);
    bool powered = getIntAsBoolOrElse(args, 2, true);
    bool showTransition = getIntAsBoolOrElse(args, 3, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastSpellHarmful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable musicBackgroundPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBackgroundStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBackgroundChangeDay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);
    int track = getInt(args, 1);
    bool streamingMusic = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBackgroundChangeNight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);
    int track = getInt(args, 1);
    bool streamingMusic = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBattlePlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBattleStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable ambientSoundPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable ambientSoundStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastKiller(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getItemActivated(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getItemActivator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getIsOpen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isOpen()));
}

Variable takeGoldFromCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int amount = getInt(args, 0);
    auto creatureToTakeFrom = getObjectAsCreature(args, 1, ctx);
    bool destroy = getIntAsBoolOrElse(args, 2, false);

    if (creatureToTakeFrom) {
        creatureToTakeFrom->takeGold(amount);
    }
    if (!destroy) {
        auto caller = getCallerAsCreature(ctx);
        caller->giveGold(amount);
    }

    return Variable::ofNull();
}

Variable getIsInConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getPlotFlag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->plotFlag()));
}

Variable setPlotFlag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    bool plotFlag = getIntAsBool(args, 1);

    target->setPlotFlag(plotFlag);

    return Variable::ofNull();
}

Variable setDialogPlaceableCamera(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int cameraId = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSoloMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(ctx.game.party().isSoloMode()));
}

Variable setMaxStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int max = getInt(args, 0);
    ctx.game.module()->area()->setMaxStealthXP(max);

    return Variable::ofNull();
}

Variable getCurrentStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->area()->currentStealthXP();
    return Variable::ofInt(result);
}

Variable getNumStackedItems(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable surrenderToEnemies(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable setCurrentStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int current = getInt(args, 0);
    ctx.game.module()->area()->setCurrentStealthXP(current);

    return Variable::ofNull();
}

Variable getCreatureSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(CreatureSize::Invalid));
}

Variable awardStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable setStealthXPEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool enabled = getIntAsBool(args, 0);
    ctx.game.module()->area()->setStealthXPEnabled(enabled);

    return Variable::ofNull();
}

Variable getAttemptedMovementTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getBlockingCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getChallengeRating(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofFloat(0.0f);
}

Variable getFoundEnemyCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getSubRace(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->subrace()));
}

Variable duplicateHeadAppearance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creatureToChange = getObjectAsCreature(args, 0, ctx);
    auto creatureToMatch = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable cutsceneAttack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    int animation = getInt(args, 1);
    auto attackResult = getIntAsEnum<AttackResultType>(args, 2);
    int damage = getInt(args, 3);

    auto caller = getCallerAsCreature(ctx);
    ctx.game.combat().addAttack(caller, target, nullptr, attackResult, damage);

    return Variable::ofNull();
}

Variable setLockOrientationInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool value = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setLockHeadFollowInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool value = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable enableVideoEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int effectType = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable startNewModule(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string moduleName = boost::to_lower_copy(getString(args, 0));
    std::string waypoint = boost::to_lower_copy(getStringOrElse(args, 1, ""));
    std::string movie1 = boost::to_lower_copy(getStringOrElse(args, 2, ""));
    std::string movie2 = boost::to_lower_copy(getStringOrElse(args, 3, ""));
    std::string movie3 = boost::to_lower_copy(getStringOrElse(args, 4, ""));
    std::string movie4 = boost::to_lower_copy(getStringOrElse(args, 5, ""));
    std::string movie5 = boost::to_lower_copy(getStringOrElse(args, 6, ""));
    std::string movie6 = boost::to_lower_copy(getStringOrElse(args, 7, ""));

    // TODO: use movie arguments
    ctx.game.scheduleModuleTransition(moduleName, waypoint);

    return Variable::ofNull();
}

Variable disableVideoEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable doSinglePlayerAutoSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable getGameDifficulty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(static_cast<int>(GameDifficulty::Normal));
}

Variable getUserActionsPending(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    return Variable::ofInt(static_cast<int>(caller->hasUserActionsPending()));
}

Variable revealMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto point = getVectorOrElse(args, 0, glm::vec3(0.0f));
    int radius = getIntOrElse(args, 1, -1);

    // TODO: implement

    return Variable::ofNull();
}

Variable showTutorialWindow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int window = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable startCreditSequence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool transparentBackground = getIntAsBool(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable isCreditSequenceInProgress(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getCurrentAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    auto action = object->getCurrentAction();

    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

Variable getAppearanceType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getTrapBaseType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto trapObject = getObject(args, 0, ctx);

    // TODO: implement

    throw NotImplementedException();
}

Variable getFirstPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getNextPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable setTrapDetectedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto trap = getObject(args, 0, ctx);
    auto detector = getObject(args, 1, ctx);

    throw NotImplementedException();
}

Variable getIsTrapped(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setEffectIcon(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);
    int icon = getInt(args, 1);

    // TODO: implement

    return Variable::ofEffect(std::move(effect));
}

Variable faceObjectAwayFromObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto facer = getObject(args, 0, ctx);
    auto objectToFaceAwayFrom = getObject(args, 1, ctx);

    facer->faceAwayFrom(*objectToFaceAwayFrom);

    return Variable::ofNull();
}

Variable getLastHostileActor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getModuleName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofString("");
}

Variable endGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool showEndGameGUI = getIntAsBoolOrElse(args, 0, true);

    // TODO: implement

    return Variable::ofNull();
}

Variable getRunScriptVar(const std::vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.execution.scriptVar);
}

Variable getCreatureMovmentType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(MovementSpeed::Immobile));
}

Variable musicBackgroundGetBattleTrack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getHasInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable addToParty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto pc = getObjectAsCreature(args, 0, ctx);
    auto partyLeader = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeFromParty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto pc = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable addPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);
    bool added = ctx.game.party().addAvailableMember(npc, creature->blueprintResRef());

    return Variable::ofInt(static_cast<int>(added));
}

Variable removePartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    bool removed = false;
    if (ctx.game.party().isMember(npc)) {
        ctx.game.party().removeMember(npc);

        auto area = ctx.game.module()->area();
        area->unloadParty();
        area->reloadParty();

        removed = true;
    }

    return Variable::ofInt(static_cast<int>(removed));
}

Variable isObjectPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool member = ctx.game.party().isMember(*creature);

    return Variable::ofInt(static_cast<int>(member));
}

Variable getPartyMemberByIndex(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int index = getInt(args, 0);
    auto member = ctx.game.party().getMember(index);

    return Variable::ofObject(getObjectIdOrInvalid(member));
}

Variable getGlobalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    bool value = ctx.game.getGlobalBoolean(identifier);

    return Variable::ofInt(static_cast<int>(value));
}

Variable setGlobalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    bool value = getIntAsBool(args, 1);

    ctx.game.setGlobalBoolean(identifier, value);

    return Variable::ofNull();
}

Variable getGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    int value = ctx.game.getGlobalNumber(identifier);

    return Variable::ofInt(value);
}

Variable setGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    int value = getInt(args, 1);

    ctx.game.setGlobalNumber(identifier, value);

    return Variable::ofNull();
}

Variable aurPostString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string str = getString(args, 0);
    int x = getInt(args, 1);
    int y = getInt(args, 2);
    float life = getFloat(args, 3);

    // TODO: implement

    return Variable::ofNull();
}

Variable barkString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int strRef = getInt(args, 1);
    int barkX = getIntOrElse(args, 2, -1);
    int barkY = getIntOrElse(args, 3, -1);

    // TODO: implement

    return Variable::ofNull();
}

Variable playVisualAreaEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int effectId = getInt(args, 0);
    auto target = getLocationArgument(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLocalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    bool value = object->getLocalBoolean(index);

    return Variable::ofInt(static_cast<int>(value));
}

Variable setLocalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    bool value = getIntAsBool(args, 2);

    object->setLocalBoolean(index, value);

    return Variable::ofNull();
}

Variable getLocalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);

    return Variable::ofInt(object->getLocalNumber(index));
}

Variable setLocalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    int value = getInt(args, 2);

    object->setLocalNumber(index, value);

    return Variable::ofNull();
}

Variable soundObjectGetPitchVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);

    throw NotImplementedException();
}

Variable getGlobalLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    auto value = ctx.game.getGlobalLocation(identifier);

    return Variable::ofLocation(std::move(value));
}

Variable setGlobalLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    auto value = getLocationArgument(args, 1);

    ctx.game.setGlobalLocation(identifier, value);

    return Variable::ofNull();
}

Variable addAvailableNPCByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable removeAvailableNPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool removed = ctx.game.party().removeAvailableMember(npc);

    return Variable::ofInt(static_cast<int>(removed));
}

Variable isAvailableCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool available = ctx.game.party().isMemberAvailable(npc);

    return Variable::ofInt(static_cast<int>(available));
}

Variable addAvailableNPCByTemplate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    std::string tmplt = boost::to_lower_copy(getString(args, 1));
    bool added = ctx.game.party().addAvailableMember(npc, tmplt);

    return Variable::ofInt(static_cast<int>(added));
}

Variable spawnAvailableNPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto position = getLocationArgument(args, 1);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable isNPCPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool isMember = ctx.game.party().isMember(npc);

    return Variable::ofInt(static_cast<int>(isMember));
}

Variable getIsConversationActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getPartyAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(static_cast<int>(PartyAIStyle::Aggressive));
}

Variable getNPCAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->aiStyle()));
}

Variable setNPCAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto style = getIntAsEnum<NPCAIStyle>(args, 1);

    creature->setAIStyle(style);

    return Variable::ofNull();
}

Variable setNPCSelectability(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    int selectability = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getNPCSelectability(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable clearAllEffects(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    caller->clearAllEffects();

    return Variable::ofNull();
}

Variable showPartySelectionGUI(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string exitScript = boost::to_lower_copy(getStringOrElse(args, 0, ""));
    int forceNpc1 = getIntOrElse(args, 1, -1);
    int forceNpc2 = getIntOrElse(args, 2, -1);
    bool allowCancel = getIntAsBoolOrElse(args, 3, false);

    PartySelectionContext partyCtx;
    partyCtx.exitScript = std::move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    // TODO: use allowCancel
    ctx.game.openPartySelection(partyCtx);

    return Variable::ofNull();
}

Variable getStandardFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->faction()));
}

Variable givePlotXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string plotName = getString(args, 0);
    int percentage = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getMinOneHP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isMinOneHP()));
}

Variable setMinOneHP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool minOneHP = getIntAsBool(args, 1);

    object->setMinOneHP(minOneHP);

    return Variable::ofNull();
}

Variable setGlobalFadeIn(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float wait = getFloatOrElse(args, 0, 0.0f);
    float length = getFloatOrElse(args, 1, 0.0f);
    float r = getFloatOrElse(args, 2, 0.0f);
    float g = getFloatOrElse(args, 3, 0.0f);
    float b = getFloatOrElse(args, 4, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable setGlobalFadeOut(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float wait = getFloatOrElse(args, 0, 0.0f);
    float length = getFloatOrElse(args, 1, 0.0f);
    float r = getFloatOrElse(args, 2, 0.0f);
    float g = getFloatOrElse(args, 3, 0.0f);
    float b = getFloatOrElse(args, 4, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastHostileTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastAttackAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(ActionType::Invalid));
}

Variable getLastForcePowerUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    throw NotImplementedException();
}

Variable getLastCombatFeatUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(FeatType::Invalid));
}

Variable getLastAttackResult(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(AttackResultType::Invalid));
}

Variable getWasForcePowerSuccessful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsDebilitated(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->isDebilitated()));
}

Variable playMovie(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string movie = boost::to_lower_copy(getString(args, 0));
    bool streamingMusic = getIntAsBoolOrElse(args, 1, false);

    // TODO: use streamingMusic
    ctx.game.playVideo(movie);

    return Variable::ofNull();
}

Variable saveNPCState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getCategoryFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);

    throw NotImplementedException();
}

Variable surrenderByFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int factionFrom = getInt(args, 0);
    int factionTo = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable changeFactionByFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int factionFrom = getInt(args, 0);
    int factionTo = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable playRoomAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string room = getString(args, 0);
    int animation = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable showGalaxyMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int planet = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable setPlanetSelectable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int planet = getInt(args, 0);
    bool selectable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setPlanetAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int planet = getInt(args, 0);
    bool available = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSelectedPlanet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(-1);
}

Variable soundObjectFadeAndStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    float seconds = getFloat(args, 1);

    throw NotImplementedException();
}

Variable changeItemCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string item = getString(args, 0);
    float costMultiplier = getFloat(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsLiveContentAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_TRUE);
}

Variable resetDialogState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable getIsPoisoned(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getSpellTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable setSoloMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool activate = getIntAsBool(args, 0);
    ctx.game.party().setSoloMode(activate);

    return Variable::ofNull();
}

Variable cancelPostDialogCharacterSwitch(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable setMaxHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int maxHP = getInt(args, 1);

    object->setMaxHitPoints(maxHP);

    return Variable::ofNull();
}

Variable noClicksFor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float duration = getFloat(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable holdWorldFadeInForDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable shipBuild(const std::vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(kShipBuild));
}

Variable surrenderRetainBuffs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable getCheatCode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int code = getInt(args, 0);
    return Variable::ofInt(R_FALSE); // cheat codes are not supported
}

Variable setMusicVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float volume = getFloatOrElse(args, 0, 1.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable createItemOnFloor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string tmplt = getString(args, 0);
    auto location = getLocationArgument(args, 1);
    bool useAppearAnimation = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setAvailableNPCId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto npcObject = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable queueMovie(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string movie = getString(args, 0);
    bool skippable = getIntAsBoolOrElse(args, 1, true);

    // TODO: implement

    return Variable::ofNull();
}

Variable playMovieQueue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool allowSkips = getIntAsBoolOrElse(args, 0, true);

    // TODO: implement

    return Variable::ofNull();
}

Variable yavinHackCloseDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

// TSL

Variable getScriptParameter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int index = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setFadeUntilScript(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable showChemicalUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto character = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getSpellForcePointCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable getFeatAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto feat = getIntAsEnum<FeatType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getSpellAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable showSwoopUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable grantFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto feat = getIntAsEnum<FeatType>(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable grantSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable spawnMine(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int mineType = getInt(args, 0);
    auto point = getLocationArgument(args, 1);
    int detectDCBase = getInt(args, 2);
    int disarmDCBase = getInt(args, 3);
    auto creature = getObject(args, 4, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable setFakeCombatState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool enable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getOwnerDemolitionsSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setOrientOnClick(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCallerAsCreature(args, 0, ctx);
    bool state = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    int influence = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable modifyInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    int modifier = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable incrementGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    int amount = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable decrementGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    std::string identifier = getString(args, 0);
    int amount = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable addBonusForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int bonusFP = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable isStealthed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable isMeditating(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setHealTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto healer = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getHealTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto healer = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getRandomDestination(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int rangeLimit = getInt(args, 1);

    throw NotImplementedException();
}

Variable isFormActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int formID = getInt(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getSpellBaseForcePointCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto spellID = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setKeepStealthInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool stealthState = getIntAsBool(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable hasLineOfSight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto vSource = getVector(args, 0);
    auto vTarget = getVector(args, 1);
    auto oSource = getObjectOrNull(args, 2, ctx);
    auto oTarget = getObjectOrNull(args, 3, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable forceHeartbeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable isRunning(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setForfeitConditions(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int forfeitFlags = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastForfeitViolation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable modifyReflexSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int modValue = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable modifyFortitudeSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int modValue = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable modifyWillSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int modValue = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getScriptStringParameter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectPersonalSpace(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofFloat(0.0f);
}

Variable adjustCreatureAttributes(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto attribute = getIntAsEnum<Ability>(args, 1);
    int amount = getInt(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable setCreatureAILevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int priority = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable resetCreatureAILevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable addAvailablePUPByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int pup = getInt(args, 0);
    auto puppet = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable assignPUP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int pup = getInt(args, 0);
    int npc = getInt(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable spawnAvailablePUP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int pup = getInt(args, 0);
    auto location = getLocationArgument(args, 1);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getPUPOwner(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto pup = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getIsPuppet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto pup = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto character = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.game.party().getLeader();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable removeNPCFromPartyToBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable creatureFlourishWeapon(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable changeObjectAppearance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int appearance = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsXBox(const std::vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(R_FALSE);
}

Variable playOverlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    int animation = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable unlockAllSongs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable disableMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool flag = getIntAsBoolOrElse(args, 0, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable detonateMine(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto mine = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable disableHealthRegen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool flag = getIntAsBoolOrElse(args, 0, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setCurrentForm(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int formID = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setDisableTransit(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool flag = getIntAsBoolOrElse(args, 0, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setInputClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int clazz = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable setForceAlwaysUpdate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int flag = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable displayMessageBox(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);
    std::string icon = getStringOrElse(args, 1, "");

    // TODO: implement

    return Variable::ofNull();
}

Variable displayDatapad(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto datapad = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeHeartbeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto placeable = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeEffectByID(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int effectID = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeEffectByExactMatch(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto effect = getEffect(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable adjustCreatureSkills(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto skill = getIntAsEnum<SkillType>(args, 1);
    int amount = getInt(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSkillRankBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto skill = getIntAsEnum<SkillType>(args, 0);
    auto object = getObjectOrCaller(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable enableRendering(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool enable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getCombatActionsPending(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsPlayerMadeCharacter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto character = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable rebuildPartyTable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

// END TSL

} // namespace routine

} // namespace game

} // namespace reone
