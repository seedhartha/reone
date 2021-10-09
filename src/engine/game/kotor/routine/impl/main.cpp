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
 *  Implementation of main routines.
 */

#include "declarations.h"

#include "../../../../common/logutil.h"
#include "../../../../resource/strings.h"
#include "../../../../script/exception/argument.h"
#include "../../../../script/exception/notimpl.h"
#include "../../../../script/executioncontext.h"

#include "../../../core/action/factory.h"
#include "../../../core/combat.h"
#include "../../../core/game.h"
#include "../../../core/location.h"
#include "../../../core/object/door.h"
#include "../../../core/object/spatial.h"
#include "../../../core/party.h"
#include "../../../core/reputes.h"
#include "../../../core/script/routine/context.h"
#include "../../../core/script/runner.h"

#include "argutil.h"
#include "objectutil.h"

using namespace std;

using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

static constexpr bool kShipBuild = true;

Variable assignCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    auto subject = getObject(args, 0, ctx);
    auto action = getAction(args, 1);

    auto objectAction = ctx.actionFactory.newDoCommand(move(action));
    subject->addAction(move(objectAction));

    return Variable::ofNull();
}

Variable delayCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);
    auto action = getAction(args, 1);

    auto objectAction = ctx.actionFactory.newDoCommand(move(action));
    getCaller(ctx)->delayAction(move(objectAction), seconds);

    return Variable::ofNull();
}

Variable executeScript(const vector<Variable> &args, const RoutineContext &ctx) {
    string script(getString(args, 0));
    auto target = getObject(args, 1, ctx);
    int scriptVar = getIntOrElse(args, 2, -1);

    ctx.scriptRunner.run(script, target->id(), kObjectInvalid, kObjectInvalid, scriptVar);

    return Variable::ofNull();
}

Variable clearAllActions(const vector<Variable> &args, const RoutineContext &ctx) {
    getCaller(ctx)->clearAllActions();
    return Variable::ofNull();
}

Variable setFacing(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    float direction = getFloat(args, 0);

    caller->setFacing(glm::radians(direction));

    return Variable::ofNull();
}

Variable switchPlayerCharacter(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setAreaUnescapable(const vector<Variable> &args, const RoutineContext &ctx) {
    bool unescapable = getBool(args, 0);
    ctx.game.module()->area()->setUnescapable(unescapable);
    return Variable::ofNull();
}

Variable getAreaUnescapable(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(ctx.game.module()->area()->isUnescapable()));
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
    auto target = getSpatialObject(args, 0, ctx);
    return Variable::ofVector(target->position());
}

Variable getFacing(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto target = getSpatialObject(args, 0, ctx);
        return Variable::ofFloat(glm::degrees(target->getFacing()));
    } catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getItemPossessor(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemPossessedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    auto itemTag = boost::to_lower_copy(getString(args, 1));
    if (itemTag.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }

    auto item = creature->getItemByTag(itemTag);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable createItemOnObject(const vector<Variable> &args, const RoutineContext &ctx) {
    string itemTemplate(boost::to_lower_copy(getString(args, 0)));
    if (itemTemplate.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }

    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int stackSize = getIntOrElse(args, 2, 1);
    int hideMessage = getIntOrElse(args, 3, 0);

    auto item = target->addItem(itemTemplate, stackSize, true);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getLastAttacker(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNearestCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto target = getSpatialObjectOrCaller(args, 2, ctx);
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

    shared_ptr<Creature> creature(ctx.game.module()->area()->getNearestCreature(target, criterias, nth - 1));

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable getDistanceToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto caller = getCallerAsSpatial(ctx);
        auto object = getSpatialObject(args, 0, ctx);

        return Variable::ofFloat(caller->getDistanceTo(*object));
    } catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getIsObjectValid(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto object = getObject(args, 0, ctx);
        return Variable::ofInt(1);
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable setCameraFacing(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable playSound(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellTargetObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCurrentHitPoints(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto object = getObjectOrCaller(args, 0, ctx);
        int hitPoints = object->currentHitPoints();

        return Variable::ofInt(hitPoints);
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getMaxHitPoints(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto object = getObjectOrCaller(args, 0, ctx);
        int hitPoints = object->maxHitPoints();

        return Variable::ofInt(hitPoints);
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getLastItemEquipped(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSubScreenID(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable cancelCombat(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCurrentForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getMaxForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable pauseGame(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setPlayerRestrictMode(const vector<Variable> &args, const RoutineContext &ctx) {
    bool restrict = getBool(args, 0);
    ctx.game.module()->player().setRestrictMode(restrict);
    return Variable::ofNull();
}

Variable getPlayerRestrictMode(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: why is this object necessary?
    auto object = getCreatureOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(ctx.game.module()->player().isRestrictMode()));
}

Variable getCasterLevel(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    return Variable::ofEffect(creature->getFirstEffect());
}

Variable getNextEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    return Variable::ofEffect(creature->getNextEffect());
}

Variable removeEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstObjectInArea(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNextObjectInArea(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getMetaMagicFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->type()));
}

Variable getRacialType(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto creature = getCreature(args, 0, ctx);
        return Variable::ofInt(static_cast<int>(creature->racialType()));
    } catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(RacialType::Invalid));
    }
}

Variable fortitudeSave(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable reflexSave(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable willSave(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellSaveDC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getAC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getGoodEvilValue(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getAlignmentGoodEvil(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstObjectInShape(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNextObjectInShape(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemStackSize(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    return Variable::ofInt(item->stackSize());
}

Variable getAbilityScore(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    auto type = getEnum<Ability>(args, 1);

    int result = creature->attributes().getAbilityScore(type);

    return Variable::ofInt(result);
}

Variable getIsDead(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto creature = getCreature(args, 0, ctx);
        bool dead = creature->isDead();

        return Variable::ofInt(static_cast<int>(dead));
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable setFacingPoint(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    glm::vec3 target(getVector(args, 0));

    caller->face(target);

    return Variable::ofNull();
}

Variable touchAttackMelee(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable touchAttackRanged(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setItemStackSize(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    int stackSize = getInt(args, 1);

    item->setStackSize(stackSize);

    return Variable::ofNull();
}

Variable getDistanceBetween(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectA = getSpatialObject(args, 0, ctx);
    auto objectB = getSpatialObject(args, 1, ctx);

    return Variable::ofFloat(objectA->getDistanceTo(*objectB));
}

Variable setReturnStrref(const vector<Variable> &args, const RoutineContext &ctx) {
    bool show = getBool(args, 0);
    int strRef = getIntOrElse(args, 1, 0);
    int returnQueryStrRef = getIntOrElse(args, 1, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getItemInSlot(const vector<Variable> &args, const RoutineContext &ctx) {
    int slot = getInt(args, 0);
    auto creature = getCreatureOrCaller(args, 1, ctx);

    auto item = creature->getEquippedItem(slot);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable setGlobalString(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    string value(getString(args, 1));

    ctx.game.setGlobalString(id, value);

    return Variable::ofNull();
}

Variable setCommandable(const vector<Variable> &args, const RoutineContext &ctx) {
    bool commandable = getBool(args, 0);
    auto target = getObjectOrCaller(args, 1, ctx);

    target->setCommandable(commandable);

    return Variable::ofNull();
}

Variable getCommandable(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isCommandable()));
}

Variable getHitDice(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    return Variable::ofInt(creature->attributes().getAggregateLevel());
}

Variable getTag(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofString(object->tag());
}

Variable resistForce(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionEqual(const vector<Variable> &args, const RoutineContext &ctx) {
    auto firstObject = getCreature(args, 0, ctx);
    auto secondObject = getCreatureOrCaller(args, 1, ctx);

    return Variable::ofInt(static_cast<int>(firstObject->faction() == secondObject->faction()));
}

Variable changeFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectToChangeFaction = getCreature(args, 0, ctx);
    auto memberOfFactionToJoin = getCreature(args, 1, ctx);

    objectToChangeFaction->setFaction(memberOfFactionToJoin->faction());

    return Variable::ofNull();
}

Variable getIsListening(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setListening(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool value = getBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setListenPattern(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    string pattern(getString(args, 1));
    int number = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getFactionWeakestMember(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionStrongestMember(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionMostDamagedMember(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionLeastDamagedMember(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionGold(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageReputation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageGoodEvilAlignment(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageLevel(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageXP(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionMostFrequentClass(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionWorstAC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionBestAC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getGlobalString(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofString(ctx.game.getGlobalString(id));
}

Variable getListenPatternNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement
    return Variable::ofInt(-1);
}

Variable getWaypointByTag(const vector<Variable> &args, const RoutineContext &ctx) {
    string tag(boost::to_lower_copy(getString(args, 0)));

    shared_ptr<SpatialObject> object;
    for (auto &waypoint : ctx.game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (waypoint->tag() == tag) {
            object = waypoint;
            break;
        }
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getTransitionTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectByTag(const vector<Variable> &args, const RoutineContext &ctx) {
    shared_ptr<Object> object;
    string tag(boost::to_lower_copy(getString(args, 0)));
    int nth = getIntOrElse(args, 1, 0);

    if (!tag.empty()) {
        object = ctx.game.module()->area()->getObjectByTag(tag, nth);
    } else {
        // Apparently, empty tag in this context stands for the player
        object = ctx.party.player();
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable adjustAlignment(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setAreaTransitionBMP(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getReputation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable adjustReputation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleFileName(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getGoingToBeAttackedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getSpatialObject(args, 0, ctx);

    glm::vec3 position(object->position());
    float facing = object->getFacing();
    auto result = make_shared<Location>(move(position), facing);

    return Variable::ofLocation(move(result));
}

Variable location(const vector<Variable> &args, const RoutineContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));

    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable applyEffectAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto durationType = getEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto location = getLocationEngineType(args, 2);
    float duration = getFloatOrElse(args, 3, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable applyEffectToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto durationType = getEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto target = getSpatialObject(args, 2, ctx);
    float duration = getFloatOrElse(args, 3, 0.0f);

    target->applyEffect(effect, durationType, duration);

    return Variable::ofNull();
}

Variable getIsPC(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature == ctx.party.player()));
}

Variable speakString(const vector<Variable> &args, const RoutineContext &ctx) {
    string stringToSpeak(getString(args, 0));
    auto talkVolume = getEnumOrElse<TalkVolume>(args, 1, TalkVolume::Talk);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSpellTargetLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getPositionFromLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    return Variable::ofVector(location->position());
}

Variable getFacingFromLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto location = getLocationEngineType(args, 0);
        return Variable::ofFloat(glm::degrees(location->facing()));
    } catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getNearestCreatureToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto location = getLocationEngineType(args, 2);
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

    shared_ptr<Creature> creature(ctx.game.module()->area()->getNearestCreatureToLocation(*location, criterias, nth - 1));

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable getNearestObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectType = getEnumOrElse(args, 0, ObjectType::All);
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    shared_ptr<SpatialObject> object(ctx.game.module()->area()->getNearestObject(target->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    auto location = getLocationEngineType(args, 1);
    int nth = getIntOrElse(args, 2, 1);

    shared_ptr<SpatialObject> object(ctx.game.module()->area()->getNearestObject(location->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectByTag(const vector<Variable> &args, const RoutineContext &ctx) {
    string tag(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    shared_ptr<SpatialObject> object(ctx.game.module()->area()->getNearestObject(target->position(), nth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getIsEnemy(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto target = getCreature(args, 0, ctx);
        auto source = getCreatureOrCaller(args, 1, ctx);
        bool enemy = ctx.reputes.getIsEnemy(*target, *source);

        return Variable::ofInt(static_cast<int>(enemy));
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getIsFriend(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto target = getCreature(args, 0, ctx);
        auto source = getCreatureOrCaller(args, 1, ctx);
        bool isFriend = ctx.reputes.getIsFriend(*target, *source);

        return Variable::ofInt(static_cast<int>(isFriend));
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getIsNeutral(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto target = getCreature(args, 0, ctx);
        auto source = getCreatureOrCaller(args, 1, ctx);
        bool neutral = ctx.reputes.getIsNeutral(*target, *source);

        return Variable::ofInt(static_cast<int>(neutral));
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getPCSpeaker(const vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.party.player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getStringByStrRef(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);
    string str(ctx.strings.get(strRef));

    return Variable::ofString(move(str));
}

Variable destroyObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destroy = getSpatialObject(args, 0, ctx);
    ctx.game.module()->area()->destroyObject(*destroy);

    return Variable::ofNull();
}

Variable getModule(const vector<Variable> &args, const RoutineContext &ctx) {
    auto module = ctx.game.module();
    return Variable::ofObject(getObjectIdOrInvalid(module));
}

Variable createObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    string blueprint(boost::to_lower_copy(getString(args, 1)));
    auto location = getLocationEngineType(args, 2);
    bool useAppearAnimation = getBoolOrElse(args, 3, false);

    auto object = ctx.game.module()->area()->createObject(objectType, blueprint, location);

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getLastSpellCaster(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
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

Variable randomName(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLoadFromSaveGame(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(ctx.game.isLoadFromSaveGame()));
}

Variable getName(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofString(object->name());
}

Variable getLastSpeaker(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable beginConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastClosedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstInPersistentObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNextInPersistentObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getAreaOfEffectCreator(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showLevelUpGUI(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setItemNonEquippable(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getButtonMashCheck(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setButtonMashCheck(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable giveItem(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable objectToString(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofString(str(boost::format("%x") % object->id()));
}

Variable getIsImmune(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemAcquired(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemAcquiredFrom(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setCustomToken(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getHasFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto feat = getEnum<FeatType>(args, 0);
    auto creature = getCreatureOrCaller(args, 1, ctx);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasFeat(feat)));
}

Variable getHasSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasSkill(skill)));
}

Variable getLastPlayerDied(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemLost(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemLostBy(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getDistanceBetweenLocations(const vector<Variable> &args, const RoutineContext &ctx) {
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);

    float result = glm::distance(locationA->position(), locationB->position());

    return Variable::ofFloat(result);
}

Variable getReflexAdjustedDamage(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable playAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    auto animType = getEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float seconds = getFloatOrElse(args, 2, 0.0f); // TODO: handle duration

    AnimationProperties properties;
    properties.speed = speed;
    caller->playAnimation(animType, move(properties));

    return Variable::ofNull();
}

Variable getHasSpellEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCreatureHasTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto talent = getTalent(args, 0);
        auto creature = getCreatureOrCaller(args, 1, ctx);

        // TODO: implement

        return Variable::ofInt(0);
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getCreatureTalentRandom(const vector<Variable> &args, const RoutineContext &ctx) {
    int category = getInt(args, 0);
    auto creature = getCreatureOrCaller(args, 1, ctx);
    int inclusion = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofTalent(nullptr);
}

Variable getCreatureTalentBest(const vector<Variable> &args, const RoutineContext &ctx) {
    int category = getInt(args, 0);
    int crMax = getInt(args, 1);
    auto creature = getCreatureOrCaller(args, 2, ctx);
    int inclusion = getIntOrElse(args, 3, 0);
    int excludeType = getIntOrElse(args, 4, -1);
    int excludeId = getIntOrElse(args, 5, -1);

    // TODO: implement

    return Variable::ofTalent(nullptr);
}

Variable getGoldPieceValue(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPlayableRacialType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable jumpToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationEngineType(args, 0);

    auto action = ctx.actionFactory.newJumpToLocation(move(destination));
    getCaller(ctx)->addActionOnTop(move(action));

    return Variable::ofNull();
}

Variable getSkillRank(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getCreatureOrCaller(args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    return Variable::ofInt(object->attributes().getSkillRank(skill));
}

Variable getAttackTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreatureOrCaller(args, 0, ctx);
    auto target = creature->getAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getLastAttackType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastAttackMode(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getDistanceBetween2D(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectA = getSpatialObject(args, 0, ctx);
    auto objectB = getSpatialObject(args, 1, ctx);
    float result = objectA->getDistanceTo(glm::vec2(objectB->position()));

    return Variable::ofFloat(result);
}

Variable getIsInCombat(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreatureOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->isInCombat()));
}

Variable getLastAssociateCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable giveGoldToCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    auto gp = getInt(args, 1);

    creature->giveGold(gp);

    return Variable::ofNull();
}

Variable setIsDestroyable(const vector<Variable> &args, const RoutineContext &ctx) {
    bool destroyabe = getBool(args, 0);
    bool raiseable = getBoolOrElse(args, 1, true);
    bool selectableWhenDead = getBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setLocked(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getDoor(args, 0, ctx);
    bool locked = getBool(args, 1);

    target->setLocked(locked);

    return Variable::ofNull();
}

Variable getLocked(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getDoor(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isLocked()));
}

Variable getClickingObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setAssociateListenPatterns(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastWeaponUsed(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastUsedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getAbilityModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIdentified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(item->isIdentified()));
}

Variable setIdentified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    bool identified = getBool(args, 1);

    item->setIdentified(identified);

    return Variable::ofNull();
}

Variable getDistanceBetweenLocations2D(const vector<Variable> &args, const RoutineContext &ctx) {
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);
    float result = glm::distance(glm::vec2(locationA->position()), glm::vec2(locationB->position()));

    return Variable::ofFloat(result);
}

Variable getDistanceToObject2D(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto caller = getCallerAsSpatial(ctx);
        auto object = getSpatialObject(args, 0, ctx);
        return Variable::ofFloat(caller->getDistanceTo(glm::vec2(object->position())));
    } catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getBlockingDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsDoorActionPossible(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable doDoorAction(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstItemInInventory(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    auto item = target->getFirstItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getNextItemInInventory(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    auto item = target->getNextItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getClassByPosition(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        int position = getInt(args, 0);
        auto creature = getCreatureOrCaller(args, 1, ctx);
        ClassType clazz = creature->attributes().getClassByPosition(position);

        return Variable::ofInt(static_cast<int>(clazz));
    } catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(ClassType::Invalid));
    }
}

Variable getLevelByPosition(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreatureOrCaller(args, 1, ctx);
    int position = getInt(args, 0);
    int level = creature->attributes().getLevelByPosition(position);

    return Variable::ofInt(level);
}

Variable getLevelByClass(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto clazz = getEnum<ClassType>(args, 0);

    return Variable::ofInt(creature->attributes().getClassLevel(clazz));
}

Variable getDamageDealtByType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTotalDamageDealt(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastDamager(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastDisarmed(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastDisturbed(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastLocked(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastUnlocked(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getInventoryDisturbType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getInventoryDisturbItem(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showUpgradeScreen(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getGender(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->gender()));
}

Variable getAttemptedAttackTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto target = caller->getAttemptedAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable playPazaak(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastPazaakResult(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable displayFeedBackText(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable addJournalQuestEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable removeJournalQuestEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getJournalEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
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

Variable sendMessageToPC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
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
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto spell = getEnum<ForcePower>(args, 0);

    // TODO: Force Powers, aka spells, are not supported at the moment

    return Variable::ofInt(static_cast<int>(false));
}

Variable openStore(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstFactionMember(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNextFactionMember(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getJournalQuestExperience(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable jumpToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getBoolOrElse(args, 1, true);

    auto action = ctx.actionFactory.newJumpToObject(move(jumpTo));
    getCaller(ctx)->addActionOnTop(move(action));

    return Variable::ofNull();
}

Variable setMapPinEnabled(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable popUpGUIPanel(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable addMultiClass(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsLinkImmune(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable giveXPToCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->giveXP(xpAmount);

    return Variable::ofNull();
}

Variable setXP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->setXP(xpAmount);

    return Variable::ofNull();
}

Variable getXP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    return Variable::ofInt(creature->xp());
}

Variable getBaseItemType(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    return Variable::ofInt(item->baseItemType());
}

Variable getItemHasItemProperty(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemACValue(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable exploreAreaForPlayer(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsEncounterCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreatureOrCaller(args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getLastPlayerDying(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getStartingLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    const ModuleInfo &info = ctx.game.module()->info();
    auto location = make_shared<Location>(info.entryPosition, info.entryFacing);
    return Variable::ofLocation(move(location));
}

Variable changeToStandardFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creatureToChange = getCreature(args, 0, ctx);
    auto faction = getEnum<Faction>(args, 1);

    creatureToChange->setFaction(faction);

    return Variable::ofNull();
}

Variable speakOneLinerConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getGold(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getCreatureOrCaller(args, 0, ctx);
    return Variable::ofInt(target->gold());
}

Variable getLastRespawnButtonPresser(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setLightsaberPowered(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsWeaponEffective(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastSpellHarmful(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastKiller(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellCastItem(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivated(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivator(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivatedTargetLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivatedTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsOpen(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getSpatialObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isOpen()));
}

Variable takeGoldFromCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    auto amount = getInt(args, 0);
    auto creatureToTakeFrom = getCreature(args, 1, ctx);
    auto destroy = getBoolOrElse(args, 2, false);

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
    throw NotImplementedException();
}

Variable getPlotFlag(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObjectOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->plotFlag()));
}

Variable setPlotFlag(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);
    bool plotFlag = getBool(args, 1);

    target->setPlotFlag(plotFlag);

    return Variable::ofNull();
}

Variable setDialogPlaceableCamera(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSoloMode(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(ctx.party.isSoloMode()));
}

Variable getNumStackedItems(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable surrenderToEnemies(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCreatureSize(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastTrapDetected(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNearestTrapToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getAttemptedMovementTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getBlockingCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFortitudeSavingThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getWillSavingThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getReflexSavingThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getChallengeRating(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFoundEnemyCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getMovementRate(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSubRace(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto creature = getCreature(args, 0, ctx);
        return Variable::ofInt(static_cast<int>(creature->subrace()));
    } catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(Subrace::None));
    }
}

Variable duplicateHeadAppearance(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable cutsceneAttack(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto target = getSpatialObject(args, 0, ctx);
    int animation = getInt(args, 1);
    auto attackResult = getEnum<AttackResultType>(args, 2);
    int damage = getInt(args, 3);

    ctx.combat.addAttack(caller, target, nullptr, attackResult, damage);

    return Variable::ofNull();
}

Variable setCameraMode(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setLockOrientationInDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setLockHeadFollowInDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable cutsceneMove(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable enableVideoEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable startNewModule(const vector<Variable> &args, const RoutineContext &ctx) {
    string moduleName(boost::to_lower_copy(getString(args, 0)));
    string waypoint(boost::to_lower_copy(getStringOrElse(args, 1, "")));
    string movie1(boost::to_lower_copy(getStringOrElse(args, 2, "")));
    string movie2(boost::to_lower_copy(getStringOrElse(args, 3, "")));
    string movie3(boost::to_lower_copy(getStringOrElse(args, 4, "")));
    string movie4(boost::to_lower_copy(getStringOrElse(args, 5, "")));
    string movie5(boost::to_lower_copy(getStringOrElse(args, 6, "")));
    string movie6(boost::to_lower_copy(getStringOrElse(args, 7, "")));

    ctx.game.scheduleModuleTransition(moduleName, waypoint);

    return Variable::ofNull();
}

Variable disableVideoEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getWeaponRanged(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable doSinglePlayerAutoSave(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getGameDifficulty(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getUserActionsPending(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    return Variable::ofInt(static_cast<int>(caller->hasUserActionsPending()));
}

Variable revealMap(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setTutorialWindowsEnabled(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showTutorialWindow(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable startCreditSequence(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable isCreditSequenceInProgress(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCurrentAction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    shared_ptr<Action> action(object->getCurrentAction());
    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

Variable getDifficultyModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getAppearanceType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable floatingTextStrRefOnCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable floatingTextStringOnCreature(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDisarmable(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDetectable(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDetectedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapFlagged(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapBaseType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapOneShot(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapCreator(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapKeyTag(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDisarmDC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDetectDC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLockKeyRequired(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLockKeyTag(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLockLockable(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLockUnlockDC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLockLockDC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getPCLevellingUp(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getHasFeatEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setPlaceableIllumination(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getPlaceableIllumination(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPlaceableObjectActionPossible(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable doPlaceableObjectAction(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstPC(const vector<Variable> &args, const RoutineContext &ctx) {
    auto player = ctx.party.player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getNextPC(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setTrapDetectedBy(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsTrapped(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable faceObjectAwayFromObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto facer = getSpatialObject(args, 0, ctx);
    auto objectToFaceAwayFrom = getSpatialObject(args, 1, ctx);

    facer->faceAwayFrom(*objectToFaceAwayFrom);

    return Variable::ofNull();
}

Variable popUpDeathGUIPanel(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setTrapDisabled(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastHostileActor(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable exportAllCharacters(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleName(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable endGame(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getRunScriptVar(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(ctx.execution.scriptVar);
}

Variable getCreatureMovmentType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getHasInventory(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getStrRefSoundDuration(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getGlobalBoolean(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    bool value = ctx.game.getGlobalBoolean(id);

    return Variable::ofInt(static_cast<int>(value));
}

Variable setGlobalBoolean(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    bool value = getBool(args, 1);

    ctx.game.setGlobalBoolean(id, value);

    return Variable::ofNull();
}

Variable getGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    int value = ctx.game.getGlobalNumber(id);

    return Variable::ofInt(value);
}

Variable setGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    int value = getInt(args, 1);

    ctx.game.setGlobalNumber(id, value);

    return Variable::ofNull();
}

Variable addJournalWorldEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable addJournalWorldEntryStrref(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable barkString(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable deleteJournalWorldAllEntries(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable deleteJournalWorldEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable deleteJournalWorldEntryStrref(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable playVisualAreaEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setJournalQuestEntryPicture(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
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
    bool value = getBool(args, 2);

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

Variable getGlobalLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    auto value = ctx.game.getGlobalLocation(id);

    return Variable::ofLocation(move(value));
}

Variable setGlobalLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    string id(getString(args, 0));
    auto value = getLocationEngineType(args, 1);

    ctx.game.setGlobalLocation(id, value);

    return Variable::ofNull();
}

Variable getIsConversationActive(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNPCAIStyle(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto creature = getCreature(args, 0, ctx);
        return Variable::ofInt(static_cast<int>(creature->aiStyle()));
    } catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(NPCAIStyle::DefaultAttack));
    }
}

Variable setNPCAIStyle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    auto style = getEnum<NPCAIStyle>(args, 1);

    creature->setAIStyle(style);

    return Variable::ofNull();
}

Variable setNPCSelectability(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNPCSelectability(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable clearAllEffects(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    caller->clearAllEffects();
    return Variable::ofNull();
}

Variable getLastConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showPartySelectionGUI(const vector<Variable> &args, const RoutineContext &ctx) {
    string exitScript(boost::to_lower_copy(getStringOrElse(args, 0, "")));
    int forceNpc1 = getIntOrElse(args, 1, -1);
    int forceNpc2 = getIntOrElse(args, 2, -1);

    PartySelectionContext partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    ctx.game.openPartySelection(partyCtx);

    return Variable::ofNull();
}

Variable getStandardFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto object = getCreature(args, 0, ctx);
        return Variable::ofInt(static_cast<int>(object->faction()));
    } catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(Faction::Invalid));
    }
}

Variable givePlotXP(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getMinOneHP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isMinOneHP()));
}

Variable setMinOneHP(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool minOneHP = getBool(args, 1);

    object->setMinOneHP(minOneHP);

    return Variable::ofNull();
}

Variable setGlobalFadeIn(const vector<Variable> &args, const RoutineContext &ctx) {
    float wait = getFloatOrElse(args, 0, 0.0f);
    float length = getFloatOrElse(args, 1, 0.0f);
    float r = getFloatOrElse(args, 0, 2.0f);
    float g = getFloatOrElse(args, 0, 3.0f);
    float b = getFloatOrElse(args, 0, 4.0f);

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
    auto attacker = getCreatureOrCaller(args, 0, ctx);
    // TODO: implement
    return Variable::ofObject(kObjectInvalid);
}

Variable getLastAttackAction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attacker = getCreatureOrCaller(args, 0, ctx);
    // TODO: implement
    return Variable::ofInt(static_cast<int>(ActionType::QueueEmpty));
}

Variable getLastForcePowerUsed(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastCombatFeatUsed(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastAttackResult(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getWasForcePowerSuccessful(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstAttacker(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getNextAttacker(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setFormation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setForcePowerUnsuccessful(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsDebilitated(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto creature = getCreatureOrCaller(args, 0, ctx);
        bool debilitated = creature->isDebilitated();
        return Variable::ofInt(static_cast<int>(debilitated));
    } catch (const ArgumentException &ex) {
        return Variable::ofInt(0);
    }
}

Variable surrenderByFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable changeFactionByFaction(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable playRoomAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showGalaxyMap(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setAreaFogColor(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable changeItemCost(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsLiveContentAvailable(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable resetDialogState(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setGoodEvilValue(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPoisoned(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    auto creature = getCreatureOrCaller(args, 0, ctx);
    // TODO: implement
    return Variable::ofObject(kObjectInvalid);
}

Variable setSoloMode(const vector<Variable> &args, const RoutineContext &ctx) {
    auto activate = getBool(args, 0);
    ctx.party.setSoloMode(activate);
    return Variable::ofNull();
}

Variable cancelPostDialogCharacterSwitch(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setMaxHitPoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int maxHP = getInt(args, 1);

    object->setMaxHitPoints(maxHP);

    return Variable::ofNull();
}

Variable noClicksFor(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable holdWorldFadeInForDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable shipBuild(const vector<Variable> &args, const RoutineContext &ctx) {
    return Variable::ofInt(static_cast<int>(kShipBuild));
}

Variable surrenderRetainBuffs(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable suppressStatusSummaryEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCheatCode(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable createItemOnFloor(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getScriptParameter(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setFadeUntilScript(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemComponent(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getItemComponentPieceValue(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showChemicalUpgradeScreen(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getChemicals(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getChemicalPieceValue(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellForcePointCost(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getFeatAcquired(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellAcquired(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showSwoopUpgradeScreen(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable grantFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable grantSpell(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable spawnMine(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setFakeCombatState(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getOwnerDemolitionsSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setOrientOnClick(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getInfluence(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setInfluence(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable modifyInfluence(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getRacialSubType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable incrementGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable decrementGlobalNumber(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setBonusForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable addBonusForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getBonusForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable yavinHackCloseDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable isStealthed(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable isMeditating(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable isInTotalDefense(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setHealTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getHealTarget(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getRandomDestination(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable isFormActive(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellFormMask(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellBaseForcePointCost(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setKeepStealthInDialog(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable hasLineOfSight(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable showDemoScreen(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable forceHeartbeat(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable isRunning(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setForfeitConditions(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getLastForfeitViolation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable modifyReflexSavingThrowBase(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable modifyFortitudeSavingThrowBase(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable modifyWillSavingThrowBase(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getScriptStringParameter(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectPersonalSpace(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable adjustCreatureAttributes(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setCreatureAILevel(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable resetCreatureAILevel(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable creatureFlourishWeapon(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable changeObjectAppearance(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsXBox(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable playOverlayAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable unlockAllSongs(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable disableMap(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable detonateMine(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable disableHealthRegen(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setCurrentForm(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setDisableTransit(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setInputClass(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setForceAlwaysUpdate(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable enableRain(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable displayMessageBox(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable displayDatapad(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable removeHeartbeat(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable removeEffectByID(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable removeEffectByExactMatch(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable adjustCreatureSkills(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getSkillRankBase(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable enableRendering(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCombatActionsPending(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPlayerMadeCharacter(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
