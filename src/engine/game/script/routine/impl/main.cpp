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

#include "../../../../common/log.h"
#include "../../../../script/exception/argument.h"
#include "../../../../script/exception/notimpl.h"

#include "../../../game.h"
#include "../../../location.h"
#include "../../../object/spatial.h"

#include "argutil.h"
#include "objectutil.h"

using namespace std;

using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

static constexpr bool kShipBuild = true;

Variable assignCommand(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto subject = getObject(game, args, 0, ctx);
    auto action = getAction(args, 1);

    auto objectAction = game.services().actionFactory().newDoCommand(move(action));
    subject->addAction(move(objectAction));

    return Variable::ofNull();
}

Variable delayCommand(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float seconds = getFloat(args, 0);
    auto action = getAction(args, 1);

    auto objectAction = game.services().actionFactory().newDoCommand(move(action));
    getCaller(game, ctx)->delayAction(move(objectAction), seconds);

    return Variable::ofNull();
}

Variable executeScript(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string script(getString(args, 0));
    auto target = getObject(game, args, 1, ctx);
    int scriptVar = getIntOrElse(args, 2, -1);

    game.services().scriptRunner().run(script, target->id(), kObjectInvalid, kObjectInvalid, scriptVar);

    return Variable::ofNull();
}

Variable clearAllActions(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    getCaller(game, ctx)->clearAllActions();
    return Variable::ofNull();
}

Variable setFacing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    float direction = getFloat(args, 0);

    caller->setFacing(glm::radians(direction));

    return Variable::ofNull();
}

Variable switchPlayerCharacter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setAreaUnescapable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool unescapable = getBool(args, 0);
    game.module()->area()->setUnescapable(unescapable);
    return Variable::ofNull();
}

Variable getAreaUnescapable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(game.module()->area()->isUnescapable()));
}

Variable getArea(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto area = game.module()->area();
    return Variable::ofObject(getObjectIdOrInvalid(area));
}

Variable getEnteringObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto triggerrer = getTriggerrer(game, ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getExitingObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto triggerrer = getTriggerrer(game, ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getPosition(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getSpatialObject(game, args, 0, ctx);
    return Variable::ofVector(target->position());
}

Variable getFacing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto target = getSpatialObject(game, args, 0, ctx);
        return Variable::ofFloat(glm::degrees(target->getFacing()));
    }
    catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getItemPossessor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemPossessedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    auto itemTag = boost::to_lower_copy(getString(args, 1));
    if (itemTag.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }

    auto item = creature->getItemByTag(itemTag);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable createItemOnObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string itemTemplate(boost::to_lower_copy(getString(args, 0)));
    if (itemTemplate.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }

    auto target = getSpatialObjectOrCaller(game, args, 1, ctx);
    int stackSize = getIntOrElse(args, 2, 1);
    int hideMessage = getIntOrElse(args, 3, 0);

    auto item = target->addItem(itemTemplate, stackSize, true);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getLastAttacker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNearestCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto target = getSpatialObjectOrCaller(game, args, 2, ctx);
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

    shared_ptr<Creature> creature(game.module()->area()->getNearestCreature(target, criterias, nth - 1));

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable getDistanceToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto caller = getCallerAsSpatial(game, ctx);
        auto object = getSpatialObject(game, args, 0, ctx);
        return Variable::ofFloat(caller->getDistanceTo(*object));
    }
    catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getIsObjectValid(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto object = getObject(game, args, 0, ctx);
        return Variable::ofInt(1);
    }
    catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable setCameraFacing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable playSound(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellTargetObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCurrentHitPoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(game, args, 0, ctx);
    return Variable::ofInt(object->currentHitPoints());
}

Variable getMaxHitPoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(game, args, 0, ctx);
    return Variable::ofInt(object->maxHitPoints());
}

Variable getLastItemEquipped(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSubScreenID(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable cancelCombat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCurrentForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getMaxForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable pauseGame(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setPlayerRestrictMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool restrict = getBool(args, 0);
    game.module()->player().setRestrictMode(restrict);
    return Variable::ofNull();
}

Variable getPlayerRestrictMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: why is this object necessary?
    auto object = getCreatureOrCaller(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(game.module()->player().isRestrictMode()));
}

Variable getCasterLevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNextEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable removeEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstObjectInArea(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNextObjectInArea(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getMetaMagicFeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getObject(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->type()));
}

Variable getRacialType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto creature = getCreature(game, args, 0, ctx);
        return Variable::ofInt(static_cast<int>(creature->racialType()));
    }
    catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(RacialType::Invalid));
    }
}

Variable fortitudeSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable reflexSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable willSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellSaveDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getAC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGoodEvilValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getAlignmentGoodEvil(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstObjectInShape(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNextObjectInShape(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemStackSize(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    return Variable::ofInt(item->stackSize());
}

Variable getAbilityScore(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    auto type = getEnum<Ability>(args, 1);

    int result = creature->attributes().getAbilityScore(type);

    return Variable::ofInt(result);
}

Variable getIsDead(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->isDead()));
}

Variable setFacingPoint(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    glm::vec3 target(getVector(args, 0));

    caller->face(target);

    return Variable::ofNull();
}

Variable touchAttackMelee(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable touchAttackRanged(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setItemStackSize(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    int stackSize = getInt(args, 1);

    item->setStackSize(stackSize);

    return Variable::ofNull();
}

Variable getDistanceBetween(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectA = getSpatialObject(game, args, 0, ctx);
    auto objectB = getSpatialObject(game, args, 1, ctx);

    return Variable::ofFloat(objectA->getDistanceTo(*objectB));
}

Variable setReturnStrref(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool show = getBool(args, 0);
    int strRef = getIntOrElse(args, 1, 0);
    int returnQuerystrRef = getIntOrElse(args, 1, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getItemInSlot(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int slot = getInt(args, 0);
    auto creature = getCreatureOrCaller(game, args, 1, ctx);

    auto item = creature->getEquippedItem(slot);

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable setGlobalString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    string value(getString(args, 1));

    game.setGlobalString(id, value);

    return Variable::ofNull();
}

Variable setCommandable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool commandable = getBool(args, 0);
    auto target = getObjectOrCaller(game, args, 1, ctx);

    target->setCommandable(commandable);

    return Variable::ofNull();
}

Variable getCommandable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getObjectOrCaller(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isCommandable()));
}

Variable getHitDice(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    return Variable::ofInt(creature->attributes().getAggregateLevel());
}

Variable getTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    return Variable::ofString(object->tag());
}

Variable resistForce(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionEqual(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto firstObject = getCreature(game, args, 0, ctx);
    auto secondObject = getCreatureOrCaller(game, args, 1, ctx);

    return Variable::ofInt(static_cast<int>(firstObject->faction() == secondObject->faction()));
}

Variable changeFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectToChangeFaction = getCreature(game, args, 0, ctx);
    auto memberOfFactionToJoin = getCreature(game, args, 1, ctx);

    objectToChangeFaction->setFaction(memberOfFactionToJoin->faction());

    return Variable::ofNull();
}

Variable getIsListening(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setListening(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    bool value = getBool(args, 1);

    // TODO: implement

    return Variable::ofNull();
}

Variable setListenPattern(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    string pattern(getString(args, 1));
    int number = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable getFactionWeakestMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionStrongestMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionMostDamagedMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionLeastDamagedMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionGold(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageReputation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageGoodEvilAlignment(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageLevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionAverageXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionMostFrequentClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionWorstAC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionBestAC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGlobalString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofString(game.getGlobalString(id));
}

Variable getListenPatternNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: implement
    return Variable::ofInt(-1);
}

Variable getWaypointByTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string tag(boost::to_lower_copy(getString(args, 0)));

    shared_ptr<SpatialObject> object;
    for (auto &waypoint : game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (waypoint->tag() == tag) {
            object = waypoint;
            break;
        }
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getTransitionTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectByTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object;
    string tag(boost::to_lower_copy(getString(args, 0)));
    int nth = getIntOrElse(args, 1, 0);

    if (!tag.empty()) {
        object = game.module()->area()->getObjectByTag(tag, nth);
    } else {
        // Apparently, empty tag in this context stands for the player
        object = game.services().party().player();
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable adjustAlignment(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setAreaTransitionBMP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getReputation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable adjustReputation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleFileName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGoingToBeAttackedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getSpatialObject(game, args, 0, ctx);

    glm::vec3 position(object->position());
    float facing = object->getFacing();
    auto result = make_shared<Location>(move(position), facing);

    return Variable::ofLocation(move(result));
}

Variable location(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));

    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable applyEffectAtLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto durationType = getEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto location = getLocationEngineType(args, 2);
    float duration = getFloatOrElse(args, 3, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable applyEffectToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto durationType = getEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto target = getSpatialObject(game, args, 2, ctx);
    float duration = getFloatOrElse(args, 3, 0.0f);

    target->applyEffect(effect, durationType, duration);

    return Variable::ofNull();
}

Variable getIsPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature == game.services().party().player()));
}

Variable speakString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string stringToSpeak(getString(args, 0));
    auto talkVolume = getEnumOrElse<TalkVolume>(args, 1, TalkVolume::Talk);

    // TODO: implement

    return Variable::ofNull();
}

Variable getSpellTargetLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPositionFromLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    return Variable::ofVector(location->position());
}

Variable getFacingFromLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto location = getLocationEngineType(args, 0);
        return Variable::ofFloat(glm::degrees(location->facing()));
    }
    catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getNearestCreatureToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
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

    shared_ptr<Creature> creature(game.module()->area()->getNearestCreatureToLocation(*location, criterias, nth - 1));

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable getNearestObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectType = getEnumOrElse(args, 0, ObjectType::All);
    auto target = getSpatialObjectOrCaller(game, args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    shared_ptr<SpatialObject> object(game.module()->area()->getNearestObject(target->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    auto location = getLocationEngineType(args, 1);
    int nth = getIntOrElse(args, 2, 1);

    shared_ptr<SpatialObject> object(game.module()->area()->getNearestObject(location->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectByTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string tag(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(game, args, 1, ctx);
    int nth = getIntOrElse(args, 2, 1);

    shared_ptr<SpatialObject> object(game.module()->area()->getNearestObject(target->position(), nth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getIsEnemy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    return Variable::ofInt(static_cast<int>(game.services().reputes().getIsEnemy(*target, *source)));
}

Variable getIsFriend(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    return Variable::ofInt(static_cast<int>(game.services().reputes().getIsFriend(*target, *source)));
}

Variable getIsNeutral(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    return Variable::ofInt(static_cast<int>(game.services().reputes().getIsNeutral(*target, *source)));
}

Variable getPCSpeaker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto player = game.services().party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getStringByStrRef(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    string str(game.services().resource().strings().get(strRef));

    return Variable::ofString(move(str));
}

Variable destroyObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto destroy = getSpatialObject(game, args, 0, ctx);
    game.module()->area()->destroyObject(*destroy);

    return Variable::ofNull();
}

Variable getModule(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto module = game.module();
    return Variable::ofObject(getObjectIdOrInvalid(module));
}

Variable createObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    string blueprint(boost::to_lower_copy(getString(args, 1)));
    auto location = getLocationEngineType(args, 2);
    bool useAppearAnimation = getBoolOrElse(args, 3, false);

    auto object = game.module()->area()->createObject(objectType, blueprint, location);

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getLastSpellCaster(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastSpell(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getUserDefinedEventNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.userDefinedEventNumber);
}

Variable getSpellId(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable randomName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLoadFromSaveGame(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(game.isLoadFromSaveGame()));
}

Variable getName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    return Variable::ofString(object->name());
}

Variable getLastSpeaker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable beginConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastClosedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstInPersistentObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNextInPersistentObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getAreaOfEffectCreator(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showLevelUpGUI(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setItemNonEquippable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getButtonMashCheck(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setButtonMashCheck(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable giveItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable objectToString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    return Variable::ofString(str(boost::format("%x") % object->id()));
}

Variable getIsImmune(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemAcquired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemAcquiredFrom(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setCustomToken(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getHasFeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto feat = getEnum<FeatType>(args, 0);
    auto creature = getCreatureOrCaller(game, args, 1, ctx);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasFeat(feat)));
}

Variable getHasSkill(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    return Variable::ofInt(static_cast<int>(creature->attributes().hasSkill(skill)));
}

Variable getLastPlayerDied(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemLost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleItemLostBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getDistanceBetweenLocations(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);

    float result = glm::distance(locationA->position(), locationB->position());

    return Variable::ofFloat(result);
}

Variable getReflexAdjustedDamage(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable playAnimation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    auto animType = getEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float seconds = getFloatOrElse(args, 2, 0.0f); // TODO: handle duration

    AnimationProperties properties;
    properties.speed = speed;
    caller->playAnimation(animType, move(properties));

    return Variable::ofNull();
}

Variable getHasSpellEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCreatureHasTalent(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCreatureTalentRandom(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCreatureTalentBest(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGoldPieceValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPlayableRacialType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable jumpToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);

    auto action = game.services().actionFactory().newJumpToLocation(move(destination));
    getCaller(game, ctx)->addActionOnTop(move(action));

    return Variable::ofNull();
}

Variable getSkillRank(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getCreatureOrCaller(game, args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    return Variable::ofInt(object->attributes().getSkillRank(skill));
}

Variable getAttackTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 0, ctx);
    auto target = creature->getAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getLastAttackType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastAttackMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getDistanceBetween2D(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectA = getSpatialObject(game, args, 0, ctx);
    auto objectB = getSpatialObject(game, args, 1, ctx);
    float result = objectA->getDistanceTo(glm::vec2(objectB->position()));

    return Variable::ofFloat(result);
}

Variable getIsInCombat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->isInCombat()));
}

Variable getLastAssociateCommand(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable giveGoldToCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    auto gp = getInt(args, 1);

    creature->giveGold(gp);

    return Variable::ofNull();
}

Variable setIsDestroyable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool destroyabe = getBool(args, 0);
    bool raiseable = getBoolOrElse(args, 1, true);
    bool selectableWhenDead = getBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofNull();
}

Variable setLocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getDoor(game, args, 0, ctx);
    bool locked = getBool(args, 1);

    target->setLocked(locked);

    return Variable::ofNull();
}

Variable getLocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getDoor(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->isLocked()));
}

Variable getClickingObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setAssociateListenPatterns(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastWeaponUsed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastUsedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getAbilityModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIdentified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(item->isIdentified()));
}

Variable setIdentified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    bool identified = getBool(args, 1);

    item->setIdentified(identified);

    return Variable::ofNull();
}

Variable getDistanceBetweenLocations2D(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);
    float result = glm::distance(glm::vec2(locationA->position()), glm::vec2(locationB->position()));

    return Variable::ofFloat(result);
}

Variable getDistanceToObject2D(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto caller = getCallerAsSpatial(game, ctx);
        auto object = getSpatialObject(game, args, 0, ctx);
        return Variable::ofFloat(caller->getDistanceTo(glm::vec2(object->position())));
    }
    catch (const ArgumentException &) {
        return Variable::ofFloat(-1.0f);
    }
}

Variable getBlockingDoor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsDoorActionPossible(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable doDoorAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstItemInInventory(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getSpatialObjectOrCaller(game, args, 0, ctx);
    auto item = target->getFirstItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getNextItemInInventory(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getSpatialObjectOrCaller(game, args, 0, ctx);
    auto item = target->getNextItem();

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getClassByPosition(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        int position = getInt(args, 0);
        auto creature = getCreatureOrCaller(game, args, 1, ctx);
        ClassType clazz = creature->attributes().getClassByPosition(position);

        return Variable::ofInt(static_cast<int>(clazz));
    }
    catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(ClassType::Invalid));
    }
}

Variable getLevelByPosition(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    int position = getInt(args, 0);
    int level = creature->attributes().getLevelByPosition(position);

    return Variable::ofInt(level);
}

Variable getLevelByClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    auto clazz = getEnum<ClassType>(args, 0);

    return Variable::ofInt(creature->attributes().getClassLevel(clazz));
}

Variable getDamageDealtByType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTotalDamageDealt(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastDamager(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastDisarmed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastDisturbed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastLocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastUnlocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getInventoryDisturbType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getInventoryDisturbItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showUpgradeScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGender(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(creature->gender()));
}

Variable getAttemptedAttackTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    auto target = caller->getAttemptedAttackTarget();

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable playPazaak(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastPazaakResult(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable displayFeedBackText(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addJournalQuestEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable removeJournalQuestEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getJournalEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable playRumblePattern(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int pattern = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable stopRumblePattern(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int pattern = getInt(args, 0);

    // TODO: implement

    return Variable::ofNull();
}

Variable sendMessageToPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getAttemptedSpellTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);

    // TODO: implement

    return Variable::ofObject(kObjectInvalid);
}

Variable getLastOpenedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto triggerrer = getTriggerrer(game, ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getHasSpell(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    auto spell = getEnum<ForcePower>(args, 0);

    // TODO: Force Powers, aka spells, are not supported at the moment

    return Variable::ofInt(static_cast<int>(false));
}

Variable openStore(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstFactionMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNextFactionMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getJournalQuestExperience(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable jumpToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(game, args, 0, ctx);
    bool walkStraightLine = getBoolOrElse(args, 1, true);

    auto action = game.services().actionFactory().newJumpToObject(move(jumpTo));
    getCaller(game, ctx)->addActionOnTop(move(action));

    return Variable::ofNull();
}

Variable setMapPinEnabled(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable popUpGUIPanel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addMultiClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsLinkImmune(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable giveXPToCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->giveXP(xpAmount);

    return Variable::ofNull();
}

Variable setXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    int xpAmount = getInt(args, 1);

    creature->setXP(xpAmount);

    return Variable::ofNull();
}

Variable getXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    return Variable::ofInt(creature->xp());
}

Variable getBaseItemType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    return Variable::ofInt(item->baseItemType());
}

Variable getItemHasItemProperty(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemACValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable exploreAreaForPlayer(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsEncounterCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 0, ctx);

    // TODO: implement

    return Variable::ofInt(0);
}

Variable getLastPlayerDying(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getStartingLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    const ModuleInfo &info = game.module()->info();
    auto location = make_shared<Location>(info.entryPosition, info.entryFacing);
    return Variable::ofLocation(move(location));
}

Variable changeToStandardFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creatureToChange = getCreature(game, args, 0, ctx);
    auto faction = getEnum<Faction>(args, 1);

    creatureToChange->setFaction(faction);

    return Variable::ofNull();
}

Variable speakOneLinerConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGold(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getCreatureOrCaller(game, args, 0, ctx);
    return Variable::ofInt(target->gold());
}

Variable getLastRespawnButtonPresser(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setLightsaberPowered(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsWeaponEffective(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastSpellHarmful(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastKiller(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellCastItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivated(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivator(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivatedTargetLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemActivatedTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsOpen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getSpatialObject(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isOpen()));
}

Variable takeGoldFromCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto amount = getInt(args, 0);
    auto creatureToTakeFrom = getCreature(game, args, 1, ctx);
    auto destroy = getBoolOrElse(args, 2, false);

    if (creatureToTakeFrom) {
        creatureToTakeFrom->takeGold(amount);
    }
    if (!destroy) {
        auto caller = getCallerAsCreature(game, ctx);
        caller->giveGold(amount);
    }

    return Variable::ofNull();
}

Variable getIsInConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPlotFlag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getObjectOrCaller(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(target->plotFlag()));
}

Variable setPlotFlag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getObject(game, args, 0, ctx);
    bool plotFlag = getBool(args, 1);

    target->setPlotFlag(plotFlag);

    return Variable::ofNull();
}

Variable setDialogPlaceableCamera(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSoloMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(game.services().party().isSoloMode()));
}

Variable getNumStackedItems(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable surrenderToEnemies(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCreatureSize(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastTrapDetected(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNearestTrapToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getAttemptedMovementTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getBlockingCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFortitudeSavingThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getWillSavingThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getReflexSavingThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getChallengeRating(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFoundEnemyCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getMovementRate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSubRace(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto creature = getCreature(game, args, 0, ctx);
        return Variable::ofInt(static_cast<int>(creature->subrace()));
    }
    catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(Subrace::None));
    }
}

Variable duplicateHeadAppearance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable cutsceneAttack(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    auto target = getSpatialObject(game, args, 0, ctx);
    int animation = getInt(args, 1);
    auto attackResult = getEnum<AttackResultType>(args, 2);
    int damage = getInt(args, 3);

    game.services().combat().addAttack(caller, target, nullptr, attackResult, damage);

    return Variable::ofNull();
}

Variable setCameraMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setLockOrientationInDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setLockHeadFollowInDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable cutsceneMove(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable enableVideoEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable startNewModule(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string moduleName(boost::to_lower_copy(getString(args, 0)));
    string waypoint(boost::to_lower_copy(getStringOrElse(args, 1, "")));
    string movie1(boost::to_lower_copy(getStringOrElse(args, 2, "")));
    string movie2(boost::to_lower_copy(getStringOrElse(args, 3, "")));
    string movie3(boost::to_lower_copy(getStringOrElse(args, 4, "")));
    string movie4(boost::to_lower_copy(getStringOrElse(args, 5, "")));
    string movie5(boost::to_lower_copy(getStringOrElse(args, 6, "")));
    string movie6(boost::to_lower_copy(getStringOrElse(args, 7, "")));

    game.scheduleModuleTransition(moduleName, waypoint);

    return Variable::ofNull();
}

Variable disableVideoEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getWeaponRanged(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable doSinglePlayerAutoSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGameDifficulty(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getUserActionsPending(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    return Variable::ofInt(static_cast<int>(caller->hasUserActionsPending()));
}

Variable revealMap(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setTutorialWindowsEnabled(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showTutorialWindow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable startCreditSequence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable isCreditSequenceInProgress(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCurrentAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(game, args, 0, ctx);
    shared_ptr<Action> action(object->getCurrentAction());
    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

Variable getDifficultyModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getAppearanceType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable floatingTextStrRefOnCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable floatingTextStringOnCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDisarmable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDetectable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDetectedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapFlagged(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapBaseType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapOneShot(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapCreator(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapKeyTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDisarmDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getTrapDetectDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLockKeyRequired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLockKeyTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLockLockable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLockUnlockDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLockLockDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPCLevellingUp(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getHasFeatEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setPlaceableIllumination(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPlaceableIllumination(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPlaceableObjectActionPossible(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable doPlaceableObjectAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto player = game.services().party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getNextPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setTrapDetectedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsTrapped(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable faceObjectAwayFromObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto facer = getSpatialObject(game, args, 0, ctx);
    auto objectToFaceAwayFrom = getSpatialObject(game, args, 1, ctx);

    facer->faceAwayFrom(*objectToFaceAwayFrom);

    return Variable::ofNull();
}

Variable popUpDeathGUIPanel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setTrapDisabled(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastHostileActor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable exportAllCharacters(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getModuleName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFactionLeader(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable endGame(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getRunScriptVar(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.scriptVar);
}

Variable getCreatureMovmentType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getHasInventory(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getStrRefSoundDuration(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getGlobalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    bool value = game.getGlobalBoolean(id);

    return Variable::ofInt(static_cast<int>(value));
}

Variable setGlobalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    bool value = getBool(args, 1);

    game.setGlobalBoolean(id, value);

    return Variable::ofNull();
}

Variable getGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    int value = game.getGlobalNumber(id);

    return Variable::ofInt(value);
}

Variable setGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    int value = getInt(args, 1);

    game.setGlobalNumber(id, value);

    return Variable::ofNull();
}

Variable addJournalWorldEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addJournalWorldEntryStrref(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable barkString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable deleteJournalWorldAllEntries(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable deleteJournalWorldEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable deleteJournalWorldEntryStrref(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable playVisualAreaEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setJournalQuestEntryPicture(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLocalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);
    bool value = object->getLocalBoolean(index);

    return Variable::ofInt(static_cast<int>(value));
}

Variable setLocalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);
    bool value = getBool(args, 2);

    object->setLocalBoolean(index, value);

    return Variable::ofNull();
}

Variable getLocalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);

    return Variable::ofInt(object->getLocalNumber(index));
}

Variable setLocalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);
    int value = getInt(args, 2);

    object->setLocalNumber(index, value);

    return Variable::ofNull();
}

Variable getGlobalLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    auto value = game.getGlobalLocation(id);

    return Variable::ofLocation(move(value));
}

Variable setGlobalLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    auto value = getLocationEngineType(args, 1);

    game.setGlobalLocation(id, value);

    return Variable::ofNull();
}

Variable getIsConversationActive(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNPCAIStyle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto creature = getCreature(game, args, 0, ctx);
        return Variable::ofInt(static_cast<int>(creature->aiStyle()));
    }
    catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(NPCAIStyle::DefaultAttack));
    }
}

Variable setNPCAIStyle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    auto style = getEnum<NPCAIStyle>(args, 1);

    creature->setAIStyle(style);

    return Variable::ofNull();
}

Variable setNPCSelectability(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNPCSelectability(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable clearAllEffects(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    caller->clearAllEffects();
    return Variable::ofNull();
}

Variable getLastConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showPartySelectionGUI(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string exitScript(boost::to_lower_copy(getStringOrElse(args, 0, "")));
    int forceNpc1 = getIntOrElse(args, 1, -1);
    int forceNpc2 = getIntOrElse(args, 2, -1);

    PartySelection::Context partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    game.openPartySelection(partyCtx);

    return Variable::ofNull();
}

Variable getStandardFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto object = getCreature(game, args, 0, ctx);
        return Variable::ofInt(static_cast<int>(object->faction()));
    }
    catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(Faction::Invalid));
    }
}

Variable givePlotXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getMinOneHP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(object->isMinOneHP()));
}

Variable setMinOneHP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    bool minOneHP = getBool(args, 1);

    object->setMinOneHP(minOneHP);

    return Variable::ofNull();
}

Variable setGlobalFadeIn(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float wait = getFloatOrElse(args, 0, 0.0f);
    float length = getFloatOrElse(args, 0, 0.0f);
    float r = getFloatOrElse(args, 0, 0.0f);
    float g = getFloatOrElse(args, 0, 0.0f);
    float b = getFloatOrElse(args, 0, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable setGlobalFadeOut(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float wait = getFloatOrElse(args, 0, 0.0f);
    float length = getFloatOrElse(args, 0, 0.0f);
    float r = getFloatOrElse(args, 0, 0.0f);
    float g = getFloatOrElse(args, 0, 0.0f);
    float b = getFloatOrElse(args, 0, 0.0f);

    // TODO: implement

    return Variable::ofNull();
}

Variable getLastHostileTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto attacker = getCreatureOrCaller(game, args, 0, ctx);
    // TODO: implement
    return Variable::ofObject(kObjectInvalid);
}

Variable getLastAttackAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto attacker = getCreatureOrCaller(game, args, 0, ctx);
    // TODO: implement
    return Variable::ofInt(static_cast<int>(ActionType::QueueEmpty));
}

Variable getLastForcePowerUsed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastCombatFeatUsed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastAttackResult(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getWasForcePowerSuccessful(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFirstAttacker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getNextAttacker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setFormation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setForcePowerUnsuccessful(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsDebilitated(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto creature = getCreatureOrCaller(game, args, 0, ctx);
        bool debilitated = creature->isDebilitated();
        return Variable::ofInt(static_cast<int>(debilitated));
    }
    catch (const ArgumentException &ex) {
        return Variable::ofInt(0);
    }
}

Variable surrenderByFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable changeFactionByFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable playRoomAnimation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showGalaxyMap(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setAreaFogColor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable changeItemCost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsLiveContentAvailable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable resetDialogState(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setGoodEvilValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPoisoned(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(game, args, 0, ctx);
    // TODO: implement
    return Variable::ofObject(kObjectInvalid);
}

Variable setSoloMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto activate = getBool(args, 0);
    game.services().party().setSoloMode(activate);
    return Variable::ofNull();
}

Variable cancelPostDialogCharacterSwitch(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setMaxHitPoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int maxHP = getInt(args, 1);

    object->setMaxHitPoints(maxHP);

    return Variable::ofNull();
}

Variable noClicksFor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable holdWorldFadeInForDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable shipBuild(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(kShipBuild));
}

Variable surrenderRetainBuffs(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable suppressStatusSummaryEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCheatCode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable createItemOnFloor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getScriptParameter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setFadeUntilScript(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemComponent(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getItemComponentPieceValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showChemicalUpgradeScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getChemicals(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getChemicalPieceValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellForcePointCost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getFeatAcquired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellAcquired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showSwoopUpgradeScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable grantFeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable grantSpell(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable spawnMine(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setFakeCombatState(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getOwnerDemolitionsSkill(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setOrientOnClick(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getInfluence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setInfluence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable modifyInfluence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getRacialSubType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable incrementGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable decrementGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setBonusForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable addBonusForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getBonusForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable yavinHackCloseDoor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable isStealthed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable isMeditating(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable isInTotalDefense(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setHealTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getHealTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getRandomDestination(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable isFormActive(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellFormMask(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSpellBaseForcePointCost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setKeepStealthInDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable hasLineOfSight(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable showDemoScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable forceHeartbeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable isRunning(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setForfeitConditions(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getLastForfeitViolation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable modifyReflexSavingThrowBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable modifyFortitudeSavingThrowBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable modifyWillSavingThrowBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getScriptStringParameter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getObjectPersonalSpace(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable adjustCreatureAttributes(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setCreatureAILevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable resetCreatureAILevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable creatureFlourishWeapon(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable changeObjectAppearance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsXBox(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable playOverlayAnimation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable unlockAllSongs(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable disableMap(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable detonateMine(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable disableHealthRegen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setCurrentForm(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setDisableTransit(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setInputClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setForceAlwaysUpdate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable enableRain(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable displayMessageBox(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable displayDatapad(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable removeHeartbeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable removeEffectByID(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable removeEffectByExactMatch(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable adjustCreatureSkills(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSkillRankBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable enableRendering(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getCombatActionsPending(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getIsPlayerMadeCharacter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
