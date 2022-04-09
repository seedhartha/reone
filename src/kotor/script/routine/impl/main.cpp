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

#include "../declarations.h"

#include "../../../../common/logutil.h"
#include "../../../../common/randomutil.h"
#include "../../../../game/action/factory.h"
#include "../../../../game/combat.h"
#include "../../../../game/event.h"
#include "../../../../game/game.h"
#include "../../../../game/location.h"
#include "../../../../game/object.h"
#include "../../../../game/object/door.h"
#include "../../../../game/party.h"
#include "../../../../game/reputes.h"
#include "../../../../game/script/runner.h"
#include "../../../../game/services.h"
#include "../../../../game/talent.h"
#include "../../../../resource/services.h"
#include "../../../../resource/strings.h"
#include "../../../../script/exception/notimpl.h"
#include "../../../../script/exception/unsupportedroutine.h"
#include "../../../../script/executioncontext.h"

#include "../argutil.h"
#include "../context.h"
#include "../objectutil.h"

#define R_FALSE 0
#define R_TRUE 1

using namespace std;

using namespace reone::game;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

static constexpr bool kShipBuild = true;

Variable unsupported(const vector<Variable> &args, const RoutineContext &ctx) {
    throw UnsupportedRoutineException();
}

Variable random(const vector<Variable> &args, const RoutineContext &ctx) {
    int maxInteger = getInt(args, 0);
    int result = reone::random(0, maxInteger - 1);

    return Variable::ofInt(result);
}

Variable printString(const vector<Variable> &args, const RoutineContext &ctx) {
    string str = getString(args, 0);
    info(str);

    return Variable::ofNull();
}

Variable floatToString(const vector<Variable> &args, const RoutineContext &ctx) {
    float flt = getFloat(args, 0);
    int width = getIntOrElse(args, 1, 18);
    int decimals = getIntOrElse(args, 2, 9);

    // TODO: use width and decimals
    return Variable::ofString(to_string(flt));
}

Variable assignCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    auto actionSubject = getObject(args, 0, ctx);
    auto actionToAssign = getAction(args, 1);

    auto commandAction = ctx.game.actionFactory().newDoCommand(move(actionToAssign));
    actionSubject->addAction(move(commandAction));

    return Variable::ofNull();
}

Variable delayCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);
    auto actionToDelay = getAction(args, 1);

    auto commandAction = ctx.game.actionFactory().newDoCommand(move(actionToDelay));
    getCaller(ctx)->delayAction(move(commandAction), seconds);

    return Variable::ofNull();
}

Variable executeScript(const vector<Variable> &args, const RoutineContext &ctx) {
    string script = getString(args, 0);
    auto target = getObject(args, 1, ctx);
    int scriptVar = getIntOrElse(args, 2, -1);

    ctx.game.scriptRunner().run(script, target->id(), kObjectInvalid, kObjectInvalid, scriptVar);

    return Variable::ofNull();
}

Variable clearAllActions(const vector<Variable> &args, const RoutineContext &ctx) {
    getCaller(ctx)->clearAllActions();
    return Variable::ofNull();
}

Variable setFacing(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    float direction = getFloat(args, 0);

    caller->setFacing(glm::radians(direction));

    return Variable::ofNull();
}

Variable switchPlayerCharacter(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    throw NotImplementedException();
}

Variable setPartyLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    ctx.game.party().setPartyLeader(npc);

    return Variable::ofNull();
}

Variable setAreaUnescapable(const vector<Variable> &args, const RoutineContext &ctx) {
    bool unescapable = getIntAsBool(args, 0);
    ctx.game.module()->area()->setUnescapable(unescapable);

    return Variable::ofNull();
}

Variable getAreaUnescapable(const vector<Variable> &args, const RoutineContext &ctx) {
    bool unescapable = ctx.game.module()->area()->isUnescapable();
    return Variable::ofInt(static_cast<int>(unescapable));
}

Variable getArea(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = ctx.game.module()->area();
    return Variable::ofObject(getObjectIdOrInvalid(area));
}

Variable getEnteringObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getExitingObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getPosition(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    return Variable::ofVector(target->position());
}

Variable getFacing(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    float facing = glm::degrees(target->getFacing());

    return Variable::ofFloat(facing);
}

Variable getItemPossessor(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getItemPossessedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    string itemTag = boost::to_lower_copy(getString(args, 1));
    if (itemTag.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }

    auto item = creature->getItemByTag(itemTag);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable createItemOnObject(const vector<Variable> &args, const RoutineContext &ctx) {
    string itemTemplate = boost::to_lower_copy(getString(args, 0));
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

Variable getLastAttacker(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attackee = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getNearestCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto target = getObjectOrCaller(args, 2, ctx);
    int nth = getIntOrElse(args, 3, 1);
    int secondCriteriaType = getIntOrElse(args, 4, -1);
    int secondCriteriaValue = getIntOrElse(args, 5, -1);
    int thirdCriteriaType = getIntOrElse(args, 6, -1);
    int thirdCriteriaValue = getIntOrElse(args, 7, -1);

    Area::SearchCriteriaList criterias;
    criterias.push_back(make_pair(static_cast<CreatureType>(firstCriteriaType), firstCriteriaValue));
    if (secondCriteriaType != -1) {
        criterias.push_back(make_pair(static_cast<CreatureType>(secondCriteriaType), secondCriteriaValue));
    }
    if (thirdCriteriaType != -1) {
        criterias.push_back(make_pair(static_cast<CreatureType>(thirdCriteriaType), thirdCriteriaValue));
    }
    auto creature = ctx.game.module()->area()->getNearestCreature(target, criterias, nth - 1);

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable getDistanceToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    auto object = getObject(args, 0, ctx);

    return Variable::ofFloat(caller->getDistanceTo(*object));
}

Variable getIsObjectValid(const vector<Variable> &args, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Object, args[0].type);

    uint32_t objectId = args[0].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    }
    auto object = ctx.game.getObjectById(objectId);

    return Variable::ofInt(static_cast<int>(object != nullptr));
}

Variable playSound(const vector<Variable> &args, const RoutineContext &ctx) {
    string soundName = getString(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSpellTargetObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getCurrentHitPoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    int hitPoints = object->currentHitPoints();

    return Variable::ofInt(hitPoints);
}

Variable getMaxHitPoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    int hitPoints = object->maxHitPoints();

    return Variable::ofInt(hitPoints);
}

Variable getSubScreenID(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable cancelCombat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    throw NotImplementedException();
}

Variable getCurrentForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);

    throw NotImplementedException();
}

Variable getMaxForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);

    throw NotImplementedException();
}

Variable pauseGame(const vector<Variable> &args, const RoutineContext &ctx) {
    bool pause = getIntAsBool(args, 0);

    throw NotImplementedException();
}

Variable setPlayerRestrictMode(const vector<Variable> &args, const RoutineContext &ctx) {
    bool restrict = getIntAsBool(args, 0);
    ctx.game.module()->player().setRestrictMode(restrict);

    return Variable::ofNull();
}

Variable getStringLength(const vector<Variable> &args, const RoutineContext &ctx) {
    string str = getString(args, 0);
    int result = str.length();

    return Variable::ofInt(static_cast<int>(result));
}

Variable getStringRight(const vector<Variable> &args, const RoutineContext &ctx) {
    string str = getString(args, 0);
    int count = getInt(args, 1);

    string result;
    if (str.size() >= count) {
        result = str.substr(str.length() - count, count);
    }

    return Variable::ofString(move(result));
}

Variable getStringLeft(const vector<Variable> &args, const RoutineContext &ctx) {
    string str = getString(args, 0);
    int count = getInt(args, 1);

    string result;
    if (str.size() >= count) {
        result = str.substr(0, count);
    }

    return Variable::ofString(move(result));
}

Variable getSubString(const vector<Variable> &args, const RoutineContext &ctx) {
    string str = getString(args, 0);
    int start = getInt(args, 1);
    int count = getInt(args, 2);

    return Variable::ofString(str.substr(start, count));
}

Variable findSubString(const vector<Variable> &args, const RoutineContext &ctx) {
    string str = getString(args, 0);
    string subString = getString(args, 1);
    size_t pos = str.find(subString);

    return Variable::ofInt(pos != string::npos ? static_cast<int>(pos) : -1);
}

Variable abs(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    int result = glm::abs(value);

    return Variable::ofInt(result);
}

Variable getPlayerRestrictMode(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);

    // TODO: why is this object necessary?

    return Variable::ofInt(static_cast<int>(ctx.game.module()->player().isRestrictMode()));
}

Variable getCasterLevel(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    throw NotImplementedException();
}

Variable getFirstEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofEffect(creature->getFirstEffect());
}

Variable getNextEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofEffect(creature->getNextEffect());
}

Variable removeEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto effect = getEffect(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsEffectValid(const vector<Variable> &args, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Effect, args[0].type);

    auto effect = static_pointer_cast<Effect>(args[0].engineType);

    return Variable::ofInt(static_cast<int>(effect && effect->type() != EffectType::Invalid));
}

Variable getEffectCreator(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable intToString(const vector<Variable> &args, const RoutineContext &ctx) {
    int integer = getInt(args, 0);

    return Variable::ofString(to_string(integer));
}

Variable getFirstObjectInArea(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsAreaOrCallerArea(args, 0, ctx);
    int objectFilter = getIntOrElse(args, 1, static_cast<int>(ObjectType::Creature));

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getNextObjectInArea(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsAreaOrCallerArea(args, 0, ctx);
    int objectFilter = getIntOrElse(args, 1, static_cast<int>(ObjectType::Creature));

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable d2(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return Variable::ofInt(result);
}

Variable d3(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return Variable::ofInt(result);
}

Variable d4(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return Variable::ofInt(result);
}

Variable d6(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return Variable::ofInt(result);
}

Variable d8(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return Variable::ofInt(result);
}

Variable d10(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return Variable::ofInt(result);
}

Variable d12(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return Variable::ofInt(result);
}

Variable d20(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return Variable::ofInt(result);
}

Variable d100(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return Variable::ofInt(result);
}

Variable getMetaMagicFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->type()));
}

Variable getRacialType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->racialType()));
}

Variable fortitudeSave(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto saveType = getIntAsEnumOrElse(args, 1, SavingThrowType::None);
    auto saveVersus = getObjectOrCaller(args, 2, ctx);

    throw NotImplementedException();
}

Variable reflexSave(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto saveType = getIntAsEnumOrElse(args, 1, SavingThrowType::None);
    auto saveVersus = getObjectOrCaller(args, 2, ctx);

    throw NotImplementedException();
}

Variable willSave(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto saveType = getIntAsEnumOrElse(args, 1, SavingThrowType::None);
    auto saveVersus = getObjectOrCaller(args, 2, ctx);

    throw NotImplementedException();
}

Variable getSpellSaveDC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable roundsToSeconds(const vector<Variable> &args, const RoutineContext &ctx) {
    int rounds = getInt(args, 0);
    return Variable::ofFloat(rounds / 6.0f);
}

Variable hoursToSeconds(const vector<Variable> &args, const RoutineContext &ctx) {
    int hours = getInt(args, 0);
    return Variable::ofInt(hours * 3600);
}

Variable soundObjectSetFixedVariance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    float fixedVariance = getFloat(args, 1);

    throw NotImplementedException();
}

Variable getGoodEvilValue(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getPartyMemberCount(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.game.party().getSize());
}

Variable getAlignmentGoodEvil(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    throw NotImplementedException();
}

Variable getFirstObjectInShape(const vector<Variable> &args, const RoutineContext &ctx) {
    auto shape = getIntAsEnum<Shape>(args, 0);
    float size = getFloat(args, 1);
    auto target = getLocationArgument(args, 2);
    bool lineOfSight = getIntAsBoolOrElse(args, 3, false);
    int objectFilter = getIntOrElse(args, 4, static_cast<int>(ObjectType::Creature));
    auto origin = getVectorOrElse(args, 5, glm::vec3(0.0f));

    throw NotImplementedException();
}

Variable getNextObjectInShape(const vector<Variable> &args, const RoutineContext &ctx) {
    auto shape = getIntAsEnum<Shape>(args, 0);
    float size = getFloat(args, 1);
    auto target = getLocationArgument(args, 2);
    bool lineOfSight = getIntAsBoolOrElse(args, 3, false);
    int objectFilter = getIntOrElse(args, 4, static_cast<int>(ObjectType::Creature));
    auto origin = getVectorOrElse(args, 5, glm::vec3(0.0f));

    throw NotImplementedException();
}

Variable signalEvent(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto evToRun = getEvent(args, 1);

    debug(boost::format("Event signalled: %s %s") % object->tag() % evToRun->number(), LogChannels::script);
    ctx.game.scriptRunner().run(object->getOnUserDefined(), object->id(), kObjectInvalid, evToRun->number());

    return Variable::ofNull();
}

Variable eventUserDefined(const vector<Variable> &args, const RoutineContext &ctx) {
    int userDefinedEventNumber = getInt(args, 0);
    auto event = make_shared<Event>(userDefinedEventNumber);

    return Variable::ofEvent(move(event));
}

Variable vectorNormalize(const vector<Variable> &args, const RoutineContext &ctx) {
    auto vector = getVector(args, 0);
    return Variable::ofVector(glm::normalize(vector));
}

Variable getItemStackSize(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    return Variable::ofInt(item->stackSize());
}

Variable getAbilityScore(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto abilityType = getIntAsEnum<Ability>(args, 1);
    int result = creature->attributes().getAbilityScore(abilityType);

    return Variable::ofInt(result);
}

Variable getIsDead(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool dead = creature->isDead();

    return Variable::ofInt(static_cast<int>(dead));
}

Variable vectorCreate(const vector<Variable> &args, const RoutineContext &ctx) {
    float x = getFloatOrElse(args, 0, 0.0f);
    float y = getFloatOrElse(args, 1, 0.0f);
    float z = getFloatOrElse(args, 2, 0.0f);

    return Variable::ofVector(glm::vec3(x, y, z));
}

Variable setFacingPoint(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    auto target = getVector(args, 0);

    caller->face(target);

    return Variable::ofNull();
}

Variable angleToVector(const vector<Variable> &args, const RoutineContext &ctx) {
    float angle = glm::radians(getFloat(args, 0));
    auto vector = glm::vec3(glm::cos(angle), glm::sin(angle), 0.0f);

    return Variable::ofVector(move(vector));
}

Variable setItemStackSize(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    int stackSize = glm::max(1, getInt(args, 1));

    item->setStackSize(stackSize);

    return Variable::ofNull();
}

Variable getDistanceBetween(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectA = getObject(args, 0, ctx);
    auto objectB = getObject(args, 1, ctx);

    return Variable::ofFloat(objectA->getDistanceTo(*objectB));
}

Variable setReturnStrref(const vector<Variable> &args, const RoutineContext &ctx) {
    bool show = getIntAsBool(args, 0);
    int stringRef = getIntOrElse(args, 1, 0);
    int returnQueryStrRef = getIntOrElse(args, 1, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getItemInSlot(const vector<Variable> &args, const RoutineContext &ctx) {
    int slot = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    auto item = creature->getEquippedItem(slot);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable setGlobalString(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    string value = getString(args, 1);

    ctx.game.setGlobalString(identifier, value);

    return Variable::ofNull();
}

Variable setCommandable(const vector<Variable> &args, const RoutineContext &ctx) {
    bool commandable = getIntAsBool(args, 0);
    auto target = getObjectOrCaller(args, 1, ctx);

    target->setCommandable(commandable);

    return Variable::ofNull();
}

Variable getCommandable(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isCommandable()));
}

Variable getHitDice(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(creature->attributes().getAggregateLevel());
}

Variable getTag(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofString(object->tag());
}

Variable resistForce(const vector<Variable> &args, const RoutineContext &ctx) {
    auto source = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getEffectType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);
    return Variable::ofInt(static_cast<int>(effect->type()));
}

Variable getFactionEqual(const vector<Variable> &args, const RoutineContext &ctx) {
    auto firstObject = getObjectAsCreature(args, 0, ctx);
    auto secondObject = getObjectOrCallerAsCreature(args, 1, ctx);

    return Variable::ofInt(static_cast<int>(firstObject->faction() == secondObject->faction()));
}

Variable setListening(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool value = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setListenPattern(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    string pattern = getString(args, 1);
    int number = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getFactionAverageReputation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sourceFactionMember = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getGlobalString(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    string result = ctx.game.getGlobalString(identifier);

    return Variable::ofString(move(result));
}

Variable getListenPatternNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getWaypointByTag(const vector<Variable> &args, const RoutineContext &ctx) {
    string waypointTag = boost::to_lower_copy(getString(args, 0));

    shared_ptr<Object> result;
    for (auto &object : ctx.game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (object->tag() == waypointTag) {
            result = object;
            break;
        }
    }

    return Variable::ofObject(getObjectIdOrInvalid(result));
}

Variable getObjectByTag(const vector<Variable> &args, const RoutineContext &ctx) {
    string tag = boost::to_lower_copy(getString(args, 0));
    int nth = getIntOrElse(args, 1, 0);

    shared_ptr<Object> object;
    if (!tag.empty()) {
        object = ctx.game.module()->area()->getObjectByTag(tag, nth);
    } else {
        object = ctx.game.party().player();
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable adjustAlignment(const vector<Variable> &args, const RoutineContext &ctx) {
    auto subject = getObject(args, 0, ctx);
    auto alignment = getIntAsEnum<Alignment>(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getReputation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto source = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable adjustReputation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    auto sourceFactionMember = getObject(args, 1, ctx);
    auto adjustment = getInt(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable getModuleFileName(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto location = make_shared<Location>(object->position(), object->getFacing());

    return Variable::ofLocation(move(location));
}

Variable location(const vector<Variable> &args, const RoutineContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));
    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable applyEffectAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto durationType = getIntAsEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto location = getLocationArgument(args, 2);
    float duration = getFloatOrElse(args, 3, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsPC(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool result = creature == ctx.game.party().player();

    return Variable::ofInt(static_cast<int>(result));
}

Variable applyEffectToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto durationType = getIntAsEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto target = getObject(args, 2, ctx);
    float duration = getFloatOrElse(args, 3, 0.0f);

    target->applyEffect(effect, durationType, duration);

    return Variable::ofNull();
}

Variable speakString(const vector<Variable> &args, const RoutineContext &ctx) {
    string stringToSpeak = getString(args, 0);
    auto talkVolume = getIntAsEnumOrElse(args, 1, TalkVolume::Talk);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSpellTargetLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getPositionFromLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationArgument(args, 0);
    return Variable::ofVector(location->position());
}

Variable getFacingFromLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationArgument(args, 0);
    float result = glm::degrees(location->facing());

    return Variable::ofFloat(result);
}

Variable getNearestObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectType = getIntAsEnumOrElse(args, 0, ObjectType::All);
    auto target = getObjectOrCaller(args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    auto object = ctx.game.module()->area()->getNearestObject(target->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    });

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectByTag(const vector<Variable> &args, const RoutineContext &ctx) {
    string tag = boost::to_lower_copy(getString(args, 0));
    auto target = getObjectOrCaller(args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    auto object = ctx.game.module()->area()->getNearestObject(target->position(), nth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    });

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable intToFloat(const vector<Variable> &args, const RoutineContext &ctx) {
    int integer = getInt(args, 0);
    return Variable::ofFloat(static_cast<float>(integer));
}

Variable floatToInt(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofInt(static_cast<int>(value));
}

Variable stringToInt(const vector<Variable> &args, const RoutineContext &ctx) {
    string number = getString(args, 0);

    int result = 0;
    if (!number.empty()) {
        result = stoi(number);
    }

    return Variable::ofInt(result);
}

Variable getIsEnemy(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool result = ctx.services.reputes.getIsEnemy(*target, *source);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsFriend(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool result = ctx.services.reputes.getIsFriend(*target, *source);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsNeutral(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool result = ctx.services.reputes.getIsNeutral(*target, *source);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getPCSpeaker(const vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getStringByStrRef(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);
    string result = ctx.services.resource.strings.get(strRef);

    return Variable::ofString(move(result));
}

Variable destroyObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destroy = getObject(args, 0, ctx);
    float delay = getFloatOrElse(args, 1, 0.0f);
    bool noFade = getIntAsBoolOrElse(args, 2, false);
    float delayUntilFade = getFloatOrElse(args, 3, 0.0f);
    int hideFeedback = getIntOrElse(args, 4, 0);

    // TODO: use delay, noFade, delayUntilFade and hideFeedback
    ctx.game.module()->area()->destroyObject(*destroy);

    return Variable::ofNull();
}

Variable getModule(const vector<Variable> &args, const RoutineContext &ctx) {
    auto module = ctx.game.module();
    return Variable::ofObject(getObjectIdOrInvalid(module));
}

Variable createObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectType = getIntAsEnum<ObjectType>(args, 0);
    string tmplt = boost::to_lower_copy(getString(args, 1));
    auto location = getLocationArgument(args, 2);
    bool useAppearAnimation = getIntAsBoolOrElse(args, 3, false);

    // TODO: use useAppearAnimation
    auto object = ctx.game.module()->area()->createObject(objectType, tmplt, location);

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable eventSpellCastAt(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caster = getObject(args, 0, ctx);
    int spell = getInt(args, 1);
    bool harmful = getIntAsBoolOrElse(args, 2, true);

    throw NotImplementedException();
}

Variable getLastSpellCaster(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastSpell(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getUserDefinedEventNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.execution.userDefinedEventNumber);
}

Variable getSpellId(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLoadFromSaveGame(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(static_cast<int>(false));
}

Variable getName(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofString(object->name());
}

Variable getLastSpeaker(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable beginConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    string resRef = boost::to_lower_copy(getStringOrElse(args, 0, ""));
    auto objectToDialog = getObjectOrNull(args, 1, ctx);

    throw NotImplementedException();
}

Variable getLastPerceived(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto object = caller->perception().lastPerceived;

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getLastPerceptionSeen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool seen = caller->perception().lastPerception == PerceptionType::Seen;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getLastClosedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getLastPerceptionVanished(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    bool vanished = caller->perception().lastPerception == PerceptionType::NotSeen;

    return Variable::ofInt(static_cast<int>(vanished));
}

Variable getFirstInPersistentObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto persistentObject = getObjectOrCaller(args, 0, ctx);
    auto residentObjectType = getIntAsEnumOrElse(args, 1, ObjectType::Creature);
    auto persistentZone = getIntAsEnumOrElse(args, 2, PersistentZone::Active);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getNextInPersistentObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto persistentObject = getObjectOrCaller(args, 0, ctx);
    auto residentObjectType = getIntAsEnumOrElse(args, 1, ObjectType::Creature);
    auto persistentZone = getIntAsEnumOrElse(args, 2, PersistentZone::Active);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable showLevelUpGUI(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setItemNonEquippable(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    bool nonEquippable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setButtonMashCheck(const vector<Variable> &args, const RoutineContext &ctx) {
    bool check = getIntAsBool(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable giveItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable objectToString(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    string result = str(boost::format("%x") % object->id());

    return Variable::ofString(move(result));
}

Variable getIsImmune(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto immunityType = getIntAsEnum<ImmunityType>(args, 1);
    auto versus = getObject(args, 2, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getModuleItemAcquired(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getEncounterActive(const vector<Variable> &args, const RoutineContext &ctx) {
    auto encounter = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setEncounterActive(const vector<Variable> &args, const RoutineContext &ctx) {
    bool newValue = getIntAsBool(args, 0);
    auto encounter = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable setCustomToken(const vector<Variable> &args, const RoutineContext &ctx) {
    int customTokenNumber = getInt(args, 0);
    string tokenValue = getString(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getHasFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto feat = getIntAsEnum<FeatType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasFeat(feat)));
}

Variable getHasSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);
    auto skill = getIntAsEnum<SkillType>(args, 0);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasSkill(skill)));
}

Variable getObjectSeen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsCreature(args, 0, ctx);
    auto source = getObjectOrCallerAsCreature(args, 1, ctx);

    bool seen = source->perception().seen.count(target) > 0;

    return Variable::ofInt(static_cast<int>(seen));
}

Variable getLastPlayerDied(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getReflexAdjustedDamage(const vector<Variable> &args, const RoutineContext &ctx) {
    int damage = getInt(args, 0);
    auto target = getObject(args, 1, ctx);
    int dc = getInt(args, 2);
    auto saveType = getIntAsEnumOrElse(args, 3, SavingThrowType::None);
    auto saveVersus = getObject(args, 4, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable playAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    auto animType = getIntAsEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float seconds = getFloatOrElse(args, 2, 0.0f);

    // TODO: use seconds
    AnimationProperties properties;
    properties.speed = speed;
    caller->playAnimation(animType, move(properties));

    return Variable::ofNull();
}

Variable talentSpell(const vector<Variable> &args, const RoutineContext &ctx) {
    int spell = getInt(args, 0);
    auto talent = make_shared<Talent>(TalentType::Spell, spell);

    return Variable::ofTalent(move(talent));
}

Variable talentFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    int feat = getInt(args, 0);
    auto talent = make_shared<Talent>(TalentType::Feat, feat);

    return Variable::ofTalent(move(talent));
}

Variable getHasSpellEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    int spell = getInt(args, 0);
    auto object = getObjectOrCaller(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getEffectSpellId(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable getCreatureHasTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getCreatureTalentRandom(const vector<Variable> &args, const RoutineContext &ctx) {
    int category = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);
    int inclusion = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofTalent(nullptr);
}

Variable getCreatureTalentBest(const vector<Variable> &args, const RoutineContext &ctx) {
    int category = getInt(args, 0);
    int crMax = getInt(args, 1);
    auto creature = getObjectOrCallerAsCreature(args, 2, ctx);
    int inclusion = getIntOrElse(args, 3, 0);
    int excludeType = getIntOrElse(args, 4, -1);
    int excludeId = getIntOrElse(args, 5, -1);

    // TODO: implement

    return Variable::ofTalent(nullptr);
}

Variable jumpToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);

    auto action = ctx.game.actionFactory().newJumpToLocation(move(destination));
    getCaller(ctx)->addActionOnTop(move(action));

    return Variable::ofNull();
}

Variable getSkillRank(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCallerAsCreature(args, 1, ctx);
    auto skill = getIntAsEnum<SkillType>(args, 0);

    return Variable::ofInt(object->attributes().getSkillRank(skill));
}

Variable getAttackTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);
    auto target = creature->getAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getDistanceBetween2D(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectA = getObject(args, 0, ctx);
    auto objectB = getObject(args, 1, ctx);
    float result = objectA->getDistanceTo(glm::vec2(objectB->position()));

    return Variable::ofFloat(result);
}

Variable getIsInCombat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->isInCombat()));
}

Variable giveGoldToCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto gp = getInt(args, 1);

    creature->giveGold(gp);

    return Variable::ofNull();
}

Variable setIsDestroyable(const vector<Variable> &args, const RoutineContext &ctx) {
    bool destroyabe = getIntAsBool(args, 0);
    bool raiseable = getIntAsBoolOrElse(args, 1, true);
    bool selectableWhenDead = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setLocked(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsDoor(args, 0, ctx);
    bool locked = getIntAsBool(args, 1);

    target->setLocked(locked);

    return Variable::ofNull();
}

Variable getLocked(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectAsDoor(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isLocked()));
}

Variable getLastWeaponUsed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastUsedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getAbilityModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto ability = getIntAsEnum<Ability>(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getDistanceToObject2D(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    auto caller = getCaller(ctx);
    float result = caller->getDistanceTo(glm::vec2(object->position()));

    return Variable::ofFloat(result);
}

Variable getBlockingDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getIsDoorActionPossible(const vector<Variable> &args, const RoutineContext &ctx) {
    auto targetDoor = getObjectAsDoor(args, 0, ctx);
    auto doorAction = getIntAsEnum<DoorAction>(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable doDoorAction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto targetDoor = getObjectAsDoor(args, 0, ctx);
    auto doorAction = getIntAsEnum<DoorAction>(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getFirstItemInInventory(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    auto item = target->getFirstItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getNextItemInInventory(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    auto item = target->getNextItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getClassByPosition(const vector<Variable> &args, const RoutineContext &ctx) {
    int classPosition = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    auto result = creature->attributes().getClassByPosition(classPosition);

    return Variable::ofInt(static_cast<int>(result));
}

Variable getLevelByPosition(const vector<Variable> &args, const RoutineContext &ctx) {
    int classPosition = getInt(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    int result = creature->attributes().getLevelByPosition(classPosition);

    return Variable::ofInt(result);
}

Variable getLevelByClass(const vector<Variable> &args, const RoutineContext &ctx) {
    auto classType = getIntAsEnum<ClassType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    int result = creature->attributes().getClassLevel(classType);

    return Variable::ofInt(result);
}

Variable getDamageDealtByType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto damageType = getIntAsEnum<DamageType>(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getTotalDamageDealt(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable getLastDamager(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getInventoryDisturbType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getInventoryDisturbItem(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable showUpgradeScreen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItemOrNull(args, 0, ctx);
    auto character = getObjectOrNull(args, 1, ctx);
    bool disableItemCreation = getIntAsBoolOrElse(args, 2, false);
    bool disableUpgrade = getIntAsBoolOrElse(args, 3, false);
    string override2DA = getStringOrElse(args, 4, "");

    // TODO: implement

    return Variable::ofNull();
}

Variable getGender(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->gender()));
}

Variable getIsTalentValid(const vector<Variable> &args, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Talent, args[0].type);
    auto talent = static_pointer_cast<Talent>(args[0].engineType);
    return Variable::ofInt(static_cast<int>(talent && talent->type() != TalentType::Invalid));
}

Variable getAttemptedAttackTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto target = caller->getAttemptedAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getTypeFromTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);
    return Variable::ofInt(static_cast<int>(talent->type()));
}

Variable getIdFromTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);
    throw NotImplementedException();
}

Variable playPazaak(const vector<Variable> &args, const RoutineContext &ctx) {
    int opponentPazaakDeck = getInt(args, 0);
    string endScript = getString(args, 1);
    int maxWager = getInt(args, 2);
    bool showTutorial = getIntAsBoolOrElse(args, 3, false);
    auto opponent = getObjectOrNull(args, 4, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastPazaakResult(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable displayFeedBackText(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int textConstant = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable addJournalQuestEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    string plotId = getString(args, 0);
    int state = getInt(args, 1);
    bool allowOverrideHigher = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeJournalQuestEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    string plotId = getString(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getJournalEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    string plotId = getString(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable playRumblePattern(const vector<Variable> &args, const RoutineContext &ctx) {
    int pattern = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable stopRumblePattern(const vector<Variable> &args, const RoutineContext &ctx) {
    int pattern = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getAttemptedSpellTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastOpenedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getHasSpell(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: Force Powers are not implemented at the moment

    return Variable::ofInt(R_FALSE);
}

Variable openStore(const vector<Variable> &args, const RoutineContext &ctx) {
    auto store = getObject(args, 0, ctx);
    auto pc = getObject(args, 1, ctx);
    int bonusMarkUp = getIntOrElse(args, 2, 0);
    int bonusMarkDown = getIntOrElse(args, 3, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable jumpToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto toJumpTo = getObject(args, 0, ctx);
    bool walkStraightLineToPoint = getIntAsBoolOrElse(args, 1, true);

    auto action = ctx.game.actionFactory().newJumpToObject(move(toJumpTo), walkStraightLineToPoint);
    getCaller(ctx)->addActionOnTop(move(action));

    return Variable::ofNull();
}

Variable setMapPinEnabled(const vector<Variable> &args, const RoutineContext &ctx) {
    auto mapPin = getObject(args, 0, ctx);
    bool enabled = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable addMultiClass(const vector<Variable> &args, const RoutineContext &ctx) {
    int classType = getInt(args, 0);
    auto source = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsLinkImmune(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    auto effect = getEffect(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable giveXPToCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->giveXP(xpAmount);

    return Variable::ofNull();
}

Variable setXP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->setXP(xpAmount);

    return Variable::ofNull();
}

Variable getXP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(creature->xp());
}

Variable getBaseItemType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    return Variable::ofInt(item->baseItemType());
}

Variable getItemHasItemProperty(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto property = getIntAsEnum<ItemProperty>(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsEncounterCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable changeToStandardFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creatureToChange = getObjectAsCreature(args, 0, ctx);
    auto standardFaction = getIntAsEnum<Faction>(args, 1);

    creatureToChange->setFaction(standardFaction);

    return Variable::ofNull();
}

Variable soundObjectPlay(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    sound->setActive(true);

    return Variable::ofNull();
}

Variable soundObjectStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    sound->setActive(false);

    return Variable::ofNull();
}

Variable soundObjectSetVolume(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    int volume = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable soundObjectSetPosition(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    auto position = getVector(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getGold(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCallerAsCreature(args, 0, ctx);
    return Variable::ofInt(target->gold());
}

Variable setLightsaberPowered(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool ovrrd = getIntAsBool(args, 1);
    bool powered = getIntAsBoolOrElse(args, 2, true);
    bool showTransition = getIntAsBoolOrElse(args, 3, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastSpellHarmful(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable musicBackgroundPlay(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBackgroundStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBackgroundChangeDay(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);
    int track = getInt(args, 1);
    bool streamingMusic = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBackgroundChangeNight(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);
    int track = getInt(args, 1);
    bool streamingMusic = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBattlePlay(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable musicBattleStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable ambientSoundPlay(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable ambientSoundStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastKiller(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getItemActivated(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getItemActivator(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getIsOpen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isOpen()));
}

Variable takeGoldFromCreature(const vector<Variable> &args, const RoutineContext &ctx) {
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

Variable getIsInConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getPlotFlag(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->plotFlag()));
}

Variable setPlotFlag(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    bool plotFlag = getIntAsBool(args, 1);

    target->setPlotFlag(plotFlag);

    return Variable::ofNull();
}

Variable setDialogPlaceableCamera(const vector<Variable> &args, const RoutineContext &ctx) {
    int cameraId = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSoloMode(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(ctx.game.party().isSoloMode()));
}

Variable setMaxStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    int max = getInt(args, 0);
    ctx.game.module()->area()->setMaxStealthXP(max);

    return Variable::ofNull();
}

Variable getCurrentStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->area()->currentStealthXP();
    return Variable::ofInt(result);
}

Variable getNumStackedItems(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable surrenderToEnemies(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable setCurrentStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    int current = getInt(args, 0);
    ctx.game.module()->area()->setCurrentStealthXP(current);

    return Variable::ofNull();
}

Variable getCreatureSize(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(CreatureSize::Invalid));
}

Variable awardStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable setStealthXPEnabled(const vector<Variable> &args, const RoutineContext &ctx) {
    bool enabled = getIntAsBool(args, 0);
    ctx.game.module()->area()->setStealthXPEnabled(enabled);

    return Variable::ofNull();
}

Variable getAttemptedMovementTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getBlockingCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getChallengeRating(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofFloat(0.0f);
}

Variable getFoundEnemyCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getSubRace(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->subrace()));
}

Variable duplicateHeadAppearance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creatureToChange = getObjectAsCreature(args, 0, ctx);
    auto creatureToMatch = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable cutsceneAttack(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    int animation = getInt(args, 1);
    auto attackResult = getIntAsEnum<AttackResultType>(args, 2);
    int damage = getInt(args, 3);

    auto caller = getCallerAsCreature(ctx);
    ctx.game.combat().addAttack(caller, target, nullptr, attackResult, damage);

    return Variable::ofNull();
}

Variable setLockOrientationInDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool value = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setLockHeadFollowInDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool value = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable enableVideoEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    int effectType = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable startNewModule(const vector<Variable> &args, const RoutineContext &ctx) {
    string moduleName = boost::to_lower_copy(getString(args, 0));
    string waypoint = boost::to_lower_copy(getStringOrElse(args, 1, ""));
    string movie1 = boost::to_lower_copy(getStringOrElse(args, 2, ""));
    string movie2 = boost::to_lower_copy(getStringOrElse(args, 3, ""));
    string movie3 = boost::to_lower_copy(getStringOrElse(args, 4, ""));
    string movie4 = boost::to_lower_copy(getStringOrElse(args, 5, ""));
    string movie5 = boost::to_lower_copy(getStringOrElse(args, 6, ""));
    string movie6 = boost::to_lower_copy(getStringOrElse(args, 7, ""));

    // TODO: use movie arguments
    ctx.game.scheduleModuleTransition(moduleName, waypoint);

    return Variable::ofNull();
}

Variable disableVideoEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable doSinglePlayerAutoSave(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable getGameDifficulty(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(static_cast<int>(GameDifficulty::Normal));
}

Variable getUserActionsPending(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    return Variable::ofInt(static_cast<int>(caller->hasUserActionsPending()));
}

Variable revealMap(const vector<Variable> &args, const RoutineContext &ctx) {
    auto point = getVectorOrElse(args, 0, glm::vec3(0.0f));
    int radius = getIntOrElse(args, 1, -1);

    // TODO: implement

    return Variable::ofNull();
}

Variable showTutorialWindow(const vector<Variable> &args, const RoutineContext &ctx) {
    int window = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable startCreditSequence(const vector<Variable> &args, const RoutineContext &ctx) {
    bool transparentBackground = getIntAsBool(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable isCreditSequenceInProgress(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getCurrentAction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    auto action = object->getCurrentAction();

    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

Variable getAppearanceType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getTrapBaseType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto trapObject = getObject(args, 0, ctx);

    // TODO: implement

    throw NotImplementedException();
}

Variable getFirstPC(const vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getNextPC(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable setTrapDetectedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    auto trap = getObject(args, 0, ctx);
    auto detector = getObject(args, 1, ctx);

    throw NotImplementedException();
}

Variable getIsTrapped(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setEffectIcon(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = getEffect(args, 0);
    int icon = getInt(args, 1);

    // TODO: implement

    return Variable::ofEffect(move(effect));
}

Variable faceObjectAwayFromObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto facer = getObject(args, 0, ctx);
    auto objectToFaceAwayFrom = getObject(args, 1, ctx);

    facer->faceAwayFrom(*objectToFaceAwayFrom);

    return Variable::ofNull();
}

Variable getLastHostileActor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getModuleName(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofString("");
}

Variable endGame(const vector<Variable> &args, const RoutineContext &ctx) {
    bool showEndGameGUI = getIntAsBoolOrElse(args, 0, true);

    // TODO: implement

    return Variable::ofNull();
}

Variable getRunScriptVar(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.execution.scriptVar);
}

Variable getCreatureMovmentType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(MovementSpeed::Immobile));
}

Variable musicBackgroundGetBattleTrack(const vector<Variable> &args, const RoutineContext &ctx) {
    auto area = getObjectAsArea(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getHasInventory(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable addToParty(const vector<Variable> &args, const RoutineContext &ctx) {
    auto pc = getObjectAsCreature(args, 0, ctx);
    auto partyLeader = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeFromParty(const vector<Variable> &args, const RoutineContext &ctx) {
    auto pc = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable addPartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);
    bool added = ctx.game.party().addAvailableMember(npc, creature->blueprintResRef());

    return Variable::ofInt(static_cast<int>(added));
}

Variable removePartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
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

Variable isObjectPartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    bool member = ctx.game.party().isMember(*creature);

    return Variable::ofInt(static_cast<int>(member));
}

Variable getPartyMemberByIndex(const vector<Variable> &args, const RoutineContext &ctx) {
    int index = getInt(args, 0);
    auto member = ctx.game.party().getMember(index);

    return Variable::ofObject(getObjectIdOrInvalid(member));
}

Variable getGlobalBoolean(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    bool value = ctx.game.getGlobalBoolean(identifier);

    return Variable::ofInt(static_cast<int>(value));
}

Variable setGlobalBoolean(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    bool value = getIntAsBool(args, 1);

    ctx.game.setGlobalBoolean(identifier, value);

    return Variable::ofNull();
}

Variable getGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    int value = ctx.game.getGlobalNumber(identifier);

    return Variable::ofInt(value);
}

Variable setGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    int value = getInt(args, 1);

    ctx.game.setGlobalNumber(identifier, value);

    return Variable::ofNull();
}

Variable aurPostString(const vector<Variable> &args, const RoutineContext &ctx) {
    string str = getString(args, 0);
    int x = getInt(args, 1);
    int y = getInt(args, 2);
    float life = getFloat(args, 3);

    // TODO: implement

    return Variable::ofNull();
}

Variable barkString(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int strRef = getInt(args, 1);
    int barkX = getIntOrElse(args, 2, -1);
    int barkY = getIntOrElse(args, 3, -1);

    // TODO: implement

    return Variable::ofNull();
}

Variable playVisualAreaEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    int effectId = getInt(args, 0);
    auto target = getLocationArgument(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLocalBoolean(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    bool value = object->getLocalBoolean(index);

    return Variable::ofInt(static_cast<int>(value));
}

Variable setLocalBoolean(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    bool value = getIntAsBool(args, 2);

    object->setLocalBoolean(index, value);

    return Variable::ofNull();
}

Variable getLocalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);

    return Variable::ofInt(object->getLocalNumber(index));
}

Variable setLocalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    int value = getInt(args, 2);

    object->setLocalNumber(index, value);

    return Variable::ofNull();
}

Variable soundObjectGetPitchVariance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);

    throw NotImplementedException();
}

Variable getGlobalLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    auto value = ctx.game.getGlobalLocation(identifier);

    return Variable::ofLocation(move(value));
}

Variable setGlobalLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    auto value = getLocationArgument(args, 1);

    ctx.game.setGlobalLocation(identifier, value);

    return Variable::ofNull();
}

Variable addAvailableNPCByObject(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable removeAvailableNPC(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool removed = ctx.game.party().removeAvailableMember(npc);

    return Variable::ofInt(static_cast<int>(removed));
}

Variable isAvailableCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool available = ctx.game.party().isMemberAvailable(npc);

    return Variable::ofInt(static_cast<int>(available));
}

Variable addAvailableNPCByTemplate(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    string tmplt = boost::to_lower_copy(getString(args, 1));
    bool added = ctx.game.party().addAvailableMember(npc, tmplt);

    return Variable::ofInt(static_cast<int>(added));
}

Variable spawnAvailableNPC(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto position = getLocationArgument(args, 1);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable isNPCPartyMember(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    bool isMember = ctx.game.party().isMember(npc);

    return Variable::ofInt(static_cast<int>(isMember));
}

Variable getIsConversationActive(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getPartyAIStyle(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(static_cast<int>(PartyAIStyle::Aggressive));
}

Variable getNPCAIStyle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->aiStyle()));
}

Variable setNPCAIStyle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto style = getIntAsEnum<NPCAIStyle>(args, 1);

    creature->setAIStyle(style);

    return Variable::ofNull();
}

Variable setNPCSelectability(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    int selectability = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getNPCSelectability(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(-1);
}

Variable clearAllEffects(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    caller->clearAllEffects();

    return Variable::ofNull();
}

Variable showPartySelectionGUI(const vector<Variable> &args, const RoutineContext &ctx) {
    string exitScript = boost::to_lower_copy(getStringOrElse(args, 0, ""));
    int forceNpc1 = getIntOrElse(args, 1, -1);
    int forceNpc2 = getIntOrElse(args, 2, -1);
    bool allowCancel = getIntAsBoolOrElse(args, 3, false);

    PartySelectionContext partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    // TODO: use allowCancel
    ctx.game.openPartySelection(partyCtx);

    return Variable::ofNull();
}

Variable getStandardFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->faction()));
}

Variable givePlotXP(const vector<Variable> &args, const RoutineContext &ctx) {
    string plotName = getString(args, 0);
    int percentage = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getMinOneHP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isMinOneHP()));
}

Variable setMinOneHP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool minOneHP = getIntAsBool(args, 1);

    object->setMinOneHP(minOneHP);

    return Variable::ofNull();
}

Variable setGlobalFadeIn(const vector<Variable> &args, const RoutineContext &ctx) {
    float wait = getFloatOrElse(args, 0, 0.0f);
    float length = getFloatOrElse(args, 1, 0.0f);
    float r = getFloatOrElse(args, 2, 0.0f);
    float g = getFloatOrElse(args, 3, 0.0f);
    float b = getFloatOrElse(args, 4, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable setGlobalFadeOut(const vector<Variable> &args, const RoutineContext &ctx) {
    float wait = getFloatOrElse(args, 0, 0.0f);
    float length = getFloatOrElse(args, 1, 0.0f);
    float r = getFloatOrElse(args, 2, 0.0f);
    float g = getFloatOrElse(args, 3, 0.0f);
    float b = getFloatOrElse(args, 4, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastHostileTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastAttackAction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(ActionType::Invalid));
}

Variable getLastForcePowerUsed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    throw NotImplementedException();
}

Variable getLastCombatFeatUsed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(FeatType::Invalid));
}

Variable getLastAttackResult(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(static_cast<int>(AttackResultType::Invalid));
}

Variable getWasForcePowerSuccessful(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsDebilitated(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->isDebilitated()));
}

Variable playMovie(const vector<Variable> &args, const RoutineContext &ctx) {
    string movie = boost::to_lower_copy(getString(args, 0));
    bool streamingMusic = getIntAsBoolOrElse(args, 1, false);

    // TODO: use streamingMusic
    ctx.game.playVideo(movie);

    return Variable::ofNull();
}

Variable saveNPCState(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getCategoryFromTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    auto talent = getTalent(args, 0);

    throw NotImplementedException();
}

Variable surrenderByFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    int factionFrom = getInt(args, 0);
    int factionTo = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable changeFactionByFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    int factionFrom = getInt(args, 0);
    int factionTo = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable playRoomAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    string room = getString(args, 0);
    int animation = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable showGalaxyMap(const vector<Variable> &args, const RoutineContext &ctx) {
    int planet = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable setPlanetSelectable(const vector<Variable> &args, const RoutineContext &ctx) {
    int planet = getInt(args, 0);
    bool selectable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setPlanetAvailable(const vector<Variable> &args, const RoutineContext &ctx) {
    int planet = getInt(args, 0);
    bool available = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSelectedPlanet(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(-1);
}

Variable soundObjectFadeAndStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getObjectAsSound(args, 0, ctx);
    float seconds = getFloat(args, 1);

    throw NotImplementedException();
}

Variable changeItemCost(const vector<Variable> &args, const RoutineContext &ctx) {
    string item = getString(args, 0);
    float costMultiplier = getFloat(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsLiveContentAvailable(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(R_TRUE);
}

Variable resetDialogState(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable getIsPoisoned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getSpellTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectOrCallerAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable setSoloMode(const vector<Variable> &args, const RoutineContext &ctx) {
    bool activate = getIntAsBool(args, 0);
    ctx.game.party().setSoloMode(activate);

    return Variable::ofNull();
}

Variable cancelPostDialogCharacterSwitch(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable setMaxHitPoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int maxHP = getInt(args, 1);

    object->setMaxHitPoints(maxHP);

    return Variable::ofNull();
}

Variable noClicksFor(const vector<Variable> &args, const RoutineContext &ctx) {
    float duration = getFloat(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable holdWorldFadeInForDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable shipBuild(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(kShipBuild));
}

Variable surrenderRetainBuffs(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable getCheatCode(const vector<Variable> &args, const RoutineContext &ctx) {
    int code = getInt(args, 0);
    return Variable::ofInt(R_FALSE); // cheat codes are not supported
}

Variable setMusicVolume(const vector<Variable> &args, const RoutineContext &ctx) {
    float volume = getFloatOrElse(args, 0, 1.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable createItemOnFloor(const vector<Variable> &args, const RoutineContext &ctx) {
    string tmplt = getString(args, 0);
    auto location = getLocationArgument(args, 1);
    bool useAppearAnimation = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setAvailableNPCId(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    auto npcObject = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable queueMovie(const vector<Variable> &args, const RoutineContext &ctx) {
    string movie = getString(args, 0);
    bool skippable = getIntAsBoolOrElse(args, 1, true);

    // TODO: implement

    return Variable::ofNull();
}

Variable playMovieQueue(const vector<Variable> &args, const RoutineContext &ctx) {
    bool allowSkips = getIntAsBoolOrElse(args, 0, true);

    // TODO: implement

    return Variable::ofNull();
}

Variable yavinHackCloseDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

// TSL

Variable getScriptParameter(const vector<Variable> &args, const RoutineContext &ctx) {
    int index = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setFadeUntilScript(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable showChemicalUpgradeScreen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto character = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getSpellForcePointCost(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable getFeatAcquired(const vector<Variable> &args, const RoutineContext &ctx) {
    auto feat = getIntAsEnum<FeatType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getSpellAcquired(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto creature = getObjectOrCallerAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable showSwoopUpgradeScreen(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable grantFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto feat = getIntAsEnum<FeatType>(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable grantSpell(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto creature = getObjectAsCreature(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable spawnMine(const vector<Variable> &args, const RoutineContext &ctx) {
    int mineType = getInt(args, 0);
    auto point = getLocationArgument(args, 1);
    int detectDCBase = getInt(args, 2);
    int disarmDCBase = getInt(args, 3);
    auto creature = getObject(args, 4, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable setFakeCombatState(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool enable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getOwnerDemolitionsSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setOrientOnClick(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCallerAsCreature(args, 0, ctx);
    bool state = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getInfluence(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setInfluence(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    int influence = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable modifyInfluence(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);
    int modifier = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable incrementGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    int amount = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable decrementGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    string identifier = getString(args, 0);
    int amount = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable addBonusForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int bonusFP = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable isStealthed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable isMeditating(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable setHealTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto healer = getObject(args, 0, ctx);
    auto target = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable getHealTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto healer = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getRandomDestination(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int rangeLimit = getInt(args, 1);

    throw NotImplementedException();
}

Variable isFormActive(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int formID = getInt(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getSpellBaseForcePointCost(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spellID = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setKeepStealthInDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    bool stealthState = getIntAsBool(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable hasLineOfSight(const vector<Variable> &args, const RoutineContext &ctx) {
    auto vSource = getVector(args, 0);
    auto vTarget = getVector(args, 1);
    auto oSource = getObjectOrNull(args, 2, ctx);
    auto oTarget = getObjectOrNull(args, 3, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable forceHeartbeat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable isRunning(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable setForfeitConditions(const vector<Variable> &args, const RoutineContext &ctx) {
    int forfeitFlags = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastForfeitViolation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofInt(0);
}

Variable modifyReflexSavingThrowBase(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int modValue = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable modifyFortitudeSavingThrowBase(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int modValue = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable modifyWillSavingThrowBase(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int modValue = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getScriptStringParameter(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectPersonalSpace(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofFloat(0.0f);
}

Variable adjustCreatureAttributes(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto attribute = getIntAsEnum<Ability>(args, 1);
    int amount = getInt(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable setCreatureAILevel(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int priority = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable resetCreatureAILevel(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable addAvailablePUPByObject(const vector<Variable> &args, const RoutineContext &ctx) {
    int pup = getInt(args, 0);
    auto puppet = getObject(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable assignPUP(const vector<Variable> &args, const RoutineContext &ctx) {
    int pup = getInt(args, 0);
    int npc = getInt(args, 1);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable spawnAvailablePUP(const vector<Variable> &args, const RoutineContext &ctx) {
    int pup = getInt(args, 0);
    auto location = getLocationArgument(args, 1);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getPUPOwner(const vector<Variable> &args, const RoutineContext &ctx) {
    auto pup = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getIsPuppet(const vector<Variable> &args, const RoutineContext &ctx) {
    auto pup = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsPartyLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    auto character = getObjectOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getPartyLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.game.party().getLeader();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable removeNPCFromPartyToBase(const vector<Variable> &args, const RoutineContext &ctx) {
    int npc = getInt(args, 0);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable creatureFlourishWeapon(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable changeObjectAppearance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int appearance = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getIsXBox(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(R_FALSE);
}

Variable playOverlayAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    int animation = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable unlockAllSongs(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

Variable disableMap(const vector<Variable> &args, const RoutineContext &ctx) {
    bool flag = getIntAsBoolOrElse(args, 0, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable detonateMine(const vector<Variable> &args, const RoutineContext &ctx) {
    auto mine = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable disableHealthRegen(const vector<Variable> &args, const RoutineContext &ctx) {
    bool flag = getIntAsBoolOrElse(args, 0, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setCurrentForm(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int formID = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setDisableTransit(const vector<Variable> &args, const RoutineContext &ctx) {
    bool flag = getIntAsBoolOrElse(args, 0, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setInputClass(const vector<Variable> &args, const RoutineContext &ctx) {
    int clazz = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable setForceAlwaysUpdate(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int flag = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable displayMessageBox(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);
    string icon = getStringOrElse(args, 1, "");

    // TODO: implement

    return Variable::ofNull();
}

Variable displayDatapad(const vector<Variable> &args, const RoutineContext &ctx) {
    auto datapad = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeHeartbeat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto placeable = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeEffectByID(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    int effectID = getInt(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable removeEffectByExactMatch(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObjectAsCreature(args, 0, ctx);
    auto effect = getEffect(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable adjustCreatureSkills(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto skill = getIntAsEnum<SkillType>(args, 1);
    int amount = getInt(args, 2);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSkillRankBase(const vector<Variable> &args, const RoutineContext &ctx) {
    auto skill = getIntAsEnum<SkillType>(args, 0);
    auto object = getObjectOrCaller(args, 1, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable enableRendering(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool enable = getIntAsBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable getCombatActionsPending(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable getIsPlayerMadeCharacter(const vector<Variable> &args, const RoutineContext &ctx) {
    auto character = getObject(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(R_FALSE);
}

Variable rebuildPartyTable(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofNull();
}

// END TSL

} // namespace routine

} // namespace kotor

} // namespace reone
