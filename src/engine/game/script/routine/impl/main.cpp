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

constexpr bool g_shipBuild = true;

Variable assignCommand(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto subject = getObject(game, args, 0, ctx);
    auto action = getAction(args, 1);

    if (subject) {
        auto objectAction = game.services().actionFactory().newDoCommand(move(action));
        subject->addAction(move(objectAction));
    } else {
        debug("Script: assignCommand: subject is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable delayCommand(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float seconds = getFloat(args, 0);
    auto action = getAction(args, 1);

    auto objectAction = game.services().actionFactory().newDoCommand(move(action));
    getCaller(game, ctx)->delayAction(move(objectAction), seconds);

    return Variable();
}

Variable executeScript(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string script(getString(args, 0));
    auto target = getObject(game, args, 1, ctx);
    int scriptVar = getInt(args, 2, -1);

    if (target) {
        game.services().scriptRunner().run(script, target->id(), kObjectInvalid, kObjectInvalid, scriptVar);
    } else {
        debug("Script: executeScript: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable clearAllActions(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    getCaller(game, ctx)->clearAllActions();
    return Variable();
}

Variable setFacing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    float direction = getFloat(args, 0);

    if (caller) {
        caller->setFacing(glm::radians(direction));
    } else {
        debug("Script: setFacing: caller is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable switchPlayerCharacter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setAreaUnescapable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool unescapable = getBool(args, 0);
    game.module()->area()->setUnescapable(unescapable);
    return Variable();
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
    glm::vec3 result(0.0f);

    auto target = getSpatialObject(game, args, 0, ctx);
    if (target) {
        result = target->position();
    } else {
        debug("Script: getPosition: target is invalid", 1, DebugChannels::script);
        return Variable::ofVector(glm::vec3(0.0f));
    }

    return Variable::ofVector(move(result));
}

Variable getFacing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = -1.0f;

    auto target = getSpatialObject(game, args, 0, ctx);
    if (target) {
        result = glm::degrees(target->getFacing());
    } else {
        debug("Script: getFacing: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable getItemPossessor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemPossessedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;
    auto creature = getCreature(game, args, 0, ctx);
    auto itemTag = boost::to_lower_copy(getString(args, 1));

    if (creature && !itemTag.empty()) {
        item = creature->getItemByTag(itemTag);
    } else if (!creature) {
        debug("Script: getItemPossessedBy: creature is invalid", 1, DebugChannels::script);
    } else if (itemTag.empty()) {
        debug("Script: getItemPossessedBy: itemTag is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable createItemOnObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;
    string itemTemplate(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(game, args, 1, ctx);
    int stackSize = getInt(args, 2, 1);

    if (!itemTemplate.empty() && target) {
        item = target->addItem(itemTemplate, stackSize, true);
    } else if (itemTemplate.empty()) {
        debug("Script: createItemOnObject: itemTemplate is invalid", 1, DebugChannels::script);
    } else if (!target) {
        debug("Script: createItemOnObject: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getLastAttacker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNearestCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto target = getSpatialObjectOrCaller(game, args, 2, ctx);
    int nth = getInt(args, 3, 1);
    int secondCriteriaType = getInt(args, 4, -1);
    int secondCriteriaValue = getInt(args, 5, -1);
    int thirdCriteriaType = getInt(args, 6, -1);
    int thirdCriteriaValue = getInt(args, 7, -1);

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
    float result = -1.0f;
    auto caller = getCallerAsSpatial(game, ctx);
    auto object = getSpatialObject(game, args, 0, ctx);

    if (caller && object) {
        result = caller->getDistanceTo(*object);
    } else if (!caller) {
        debug("Script: getDistanceToObject: caller is invalid", 1, DebugChannels::script);
    } else if (!object) {
        debug("Script: getDistanceToObject: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable getIsObjectValid(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(static_cast<bool>(object)));
}

Variable setCameraFacing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable playSound(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellTargetObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCurrentHitPoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;

    auto object = getObjectOrCaller(game, args, 0, ctx);
    if (object) {
        result = object->currentHitPoints();
    } else {
        debug("Script: getCurrentHitPoints: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getMaxHitPoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;

    auto object = getObjectOrCaller(game, args, 0, ctx);
    if (object) {
        result = object->maxHitPoints();
    } else {
        debug("Script: getMaxHitPoints: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getLastItemEquipped(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSubScreenID(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable cancelCombat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCurrentForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getMaxForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable pauseGame(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setPlayerRestrictMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool restrict = getBool(args, 0);
    game.module()->player().setRestrictMode(restrict);
    return Variable();
}

Variable getPlayerRestrictMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: why is this object necessary?
    auto object = getCreatureOrCaller(game, args, 0, ctx);
    return Variable::ofInt(static_cast<int>(game.module()->player().isRestrictMode()));
}

Variable getCasterLevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNextEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable removeEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstObjectInArea(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNextObjectInArea(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getMetaMagicFeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getObjectType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = ObjectType::Invalid;

    auto target = getObject(game, args, 0, ctx);
    if (target) {
        result = target->type();
    } else {
        debug("Script: getObjectType: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getRacialType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = RacialType::Invalid;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature->racialType();
    } else {
        debug("Script: getRacialType: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable fortitudeSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable reflexSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable willSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellSaveDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getAC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGoodEvilValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getAlignmentGoodEvil(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstObjectInShape(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNextObjectInShape(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemStackSize(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;

    auto item = getItem(game, args, 0, ctx);
    if (item) {
        result = item->stackSize();
    } else {
        debug("Script: getItemStackSize: item is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getAbilityScore(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreature(game, args, 0, ctx);
    auto type = getEnum<Ability>(args, 1);

    if (creature) {
        result = creature->attributes().getAbilityScore(type);
    } else {
        debug("Script: getAbilityScore: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getIsDead(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature->isDead();
    } else {
        debug("Script: getIsDead: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable setFacingPoint(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    if (caller) {
        glm::vec3 target(getVector(args, 0));
        caller->face(target);
    } else {
        debug("Script: setFacingPoint: caller is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable touchAttackMelee(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable touchAttackRanged(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setItemStackSize(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    int stackSize = getInt(args, 1);

    if (item) {
        item->setStackSize(stackSize);
    } else {
        debug("Script: setItemStackSize: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getDistanceBetween(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto objectA = getSpatialObject(game, args, 0, ctx);
    auto objectB = getSpatialObject(game, args, 1, ctx);

    if (objectA && objectB) {
        result = objectA->getDistanceTo(*objectB);
    } else if (!objectA) {
        debug("Script: getDistanceBetween: objectA is invalid", 1, DebugChannels::script);
    } else if (!objectB) {
        debug("Script: getDistanceBetween: objectB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable setReturnStrref(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemInSlot(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    int slot = getInt(args, 0);

    if (creature) {
        item = creature->getEquippedItem(slot);
    } else {
        debug("Script: getItemInSlot: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable setGlobalString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    string value(getString(args, 1));

    game.setGlobalString(id, value);

    return Variable();
}

Variable setCommandable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool commandable = getBool(args, 0);
    auto target = getObjectOrCaller(game, args, 1, ctx);

    if (target) {
        target->setCommandable(commandable);
    } else {
        debug("Script: setCommandable: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getCommandable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto target = getObjectOrCaller(game, args, 0, ctx);
    if (target) {
        result = target->isCommandable();
    } else {
        debug("Script: getCommandable: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getHitDice(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature->attributes().getAggregateLevel();
    } else {
        debug("Script: getGender: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string result;

    auto object = getObject(game, args, 0, ctx);
    if (object) {
        result = object->tag();
    } else {
        debug("Script: getTag: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable resistForce(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionEqual(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto firstObject = getCreature(game, args, 0, ctx);
    auto secondObject = getCreatureOrCaller(game, args, 1, ctx);

    if (firstObject && secondObject) {
        result = firstObject->faction() == secondObject->faction();
    } else if (!firstObject) {
        debug("Script: getStandardFaction: firstObject is invalid", 1, DebugChannels::script);
    } else if (!secondObject) {
        debug("Script: getStandardFaction: secondObject is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable changeFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectToChangeFaction = getCreature(game, args, 0, ctx);
    auto memberOfFactionToJoin = getCreature(game, args, 1, ctx);

    if (objectToChangeFaction && memberOfFactionToJoin) {
        objectToChangeFaction->setFaction(memberOfFactionToJoin->faction());
    } else if (!objectToChangeFaction) {
        debug("Script: changeFaction: objectToChangeFaction is invalid", 1, DebugChannels::script);
    } else if (!memberOfFactionToJoin) {
        debug("Script: changeFaction: memberOfFactionToJoin is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getIsListening(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setListening(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setListenPattern(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionWeakestMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionStrongestMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionMostDamagedMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionLeastDamagedMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionGold(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionAverageReputation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionAverageGoodEvilAlignment(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionAverageLevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionAverageXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionMostFrequentClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionWorstAC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionBestAC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGlobalString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofString(game.getGlobalString(id));
}

Variable getListenPatternNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofString(game.getGlobalString(id));
}

Variable getWaypointByTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> object;
    string tag(boost::to_lower_copy(getString(args, 0)));

    for (auto &waypoint : game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (waypoint->tag() == tag) {
            object = waypoint;
            break;
        }
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getTransitionTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getObjectByTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object;
    string tag(boost::to_lower_copy(getString(args, 0)));
    int nth = getInt(args, 1, 0);

    if (!tag.empty()) {
        object = game.module()->area()->getObjectByTag(tag, nth);
    } else {
        // Apparently, empty tag in this context stands for the player
        object = game.services().party().player();
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable adjustAlignment(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setAreaTransitionBMP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getReputation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable adjustReputation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getModuleFileName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGoingToBeAttackedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Location> result;

    auto object = getSpatialObject(game, args, 0, ctx);
    if (object) {
        glm::vec3 position(object->position());
        float facing = object->getFacing();
        result = make_shared<Location>(move(position), facing);
    } else {
        debug("Script: getLocation: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofLocation(move(result));
}

Variable location(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));
    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable applyEffectAtLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));
    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable applyEffectToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto durationType = getEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto target = getSpatialObject(game, args, 2, ctx);
    float duration = getFloat(args, 3, 0.0f);

    if (target) {
        target->applyEffect(effect, durationType, duration);
    } else {
        debug("Script: applyEffectToObject: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getIsPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature == game.services().party().player();
    } else {
        debug("Script: getIsPC: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable speakString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellTargetLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getPositionFromLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    glm::vec3 result(0.0f);

    auto location = getLocationEngineType(args, 0);
    if (location) {
        result = location->position();
    } else {
        debug("Script: getPositionFromLocation: location is invalid", 1, DebugChannels::script);
    }

    return Variable::ofVector(move(result));
}

Variable getFacingFromLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = -1.0f;

    auto location = getLocationEngineType(args, 0);
    if (location) {
        result = glm::degrees(location->facing());
    } else {
        debug("Script: getFacingFromLocation: location is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable getNearestCreatureToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto location = getLocationEngineType(args, 2);
    int nth = getInt(args, 3, 1);
    int secondCriteriaType = getInt(args, 4, -1);
    int secondCriteriaValue = getInt(args, 5, -1);
    int thirdCriteriaType = getInt(args, 6, -1);
    int thirdCriteriaValue = getInt(args, 7, -1);

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
    auto objectType = getEnum(args, 0, ObjectType::All);
    auto target = getSpatialObjectOrCaller(game, args, 1, ctx);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(game.module()->area()->getNearestObject(target->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    auto location = getLocationEngineType(args, 1);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(game.module()->area()->getNearestObject(location->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getNearestObjectByTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string tag(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(game, args, 1, ctx);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(game.module()->area()->getNearestObject(target->position(), nth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getIsEnemy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    if (target && source) {
        result = game.services().reputes().getIsEnemy(*target, *source);
    } else if (!target) {
        debug("Script: getIsEnemy: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsEnemy: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsFriend(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    if (target && source) {
        result = game.services().reputes().getIsFriend(*target, *source);
    } else if (!target) {
        debug("Script: getIsFriend: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsFriend: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsNeutral(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(game, args, 0, ctx);
    auto source = getCreatureOrCaller(game, args, 1, ctx);

    if (target && source) {
        result = game.services().reputes().getIsNeutral(*target, *source);
    } else if (!target) {
        debug("Script: getIsNeutral: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsNeutral: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getPCSpeaker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto player = game.services().party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getStringByStrRef(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    return Variable::ofString(game.services().resource().strings().get(strRef));
}

Variable destroyObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto destroy = getSpatialObject(game, args, 0, ctx);
    if (destroy) {
        game.module()->area()->destroyObject(*destroy);
    } else {
        debug("Script: destroyObject: destroy is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable getModule(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto module = game.module();
    return Variable::ofObject(getObjectIdOrInvalid(module));
}

Variable createObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    string blueprintResRef(boost::to_lower_copy(getString(args, 1)));
    auto location = getLocationEngineType(args, 2);
    bool useAppearAnimation = getBool(args, 3, false);

    auto object = game.module()->area()->createObject(objectType, blueprintResRef, location);

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable getLastSpellCaster(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastSpell(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getUserDefinedEventNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.userDefinedEventNumber);
}

Variable getSpellId(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable randomName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLoadFromSaveGame(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(game.isLoadFromSaveGame()));
}

Variable getName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string result;

    auto object = getObject(game, args, 0, ctx);
    if (object) {
        result = object->name();
    } else {
        debug("Script: getName: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable getLastSpeaker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable beginConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastClosedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstInPersistentObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNextInPersistentObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getAreaOfEffectCreator(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showLevelUpGUI(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setItemNonEquippable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getButtonMashCheck(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setButtonMashCheck(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable giveItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable objectToString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    string result;

    if (object) {
        result = str(boost::format("%x") % object->id());
    } else {
        debug("Script: objectToString: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable getIsImmune(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getModuleItemAcquired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getModuleItemAcquiredFrom(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setCustomToken(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getHasFeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto feat = getEnum<FeatType>(args, 0);
    auto creature = getCreatureOrCaller(game, args, 1, ctx);

    if (creature) {
        result = creature->attributes().hasFeat(feat);
    } else {
        debug("Script: getHasFeat: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getHasSkill(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    if (creature) {
        result = creature->attributes().hasSkill(skill);
    } else {
        debug("Script: getHasSkill: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getLastPlayerDied(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getModuleItemLost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getModuleItemLostBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getDistanceBetweenLocations(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);

    if (locationA && locationB) {
        result = glm::distance(locationA->position(), locationB->position());
    } else if (!locationA) {
        debug("Script: getDistanceBetweenLocations: locationA is invalid", 1, DebugChannels::script);
    } else if (!locationB) {
        debug("Script: getDistanceBetweenLocations: locationB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable getReflexAdjustedDamage(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable playAnimation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    auto animType = getEnum<AnimationType>(args, 0);
    float speed = getFloat(args, 1, 1.0f);
    float seconds = getFloat(args, 2, 0.0f); // TODO: handle duration

    if (caller) {
        AnimationProperties properties;
        properties.speed = speed;
        caller->playAnimation(animType, move(properties));
    } else {
        debug("Script: playAnimation: caller is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getHasSpellEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCreatureHasTalent(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCreatureTalentRandom(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCreatureTalentBest(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGoldPieceValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsPlayableRacialType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable jumpToLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    if (destination) {
        auto action = game.services().actionFactory().newJumpToLocation(move(destination));
        getCaller(game, ctx)->addActionOnTop(move(action));
    } else {
        debug("Script: jumpToLocation: destination is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable getSkillRank(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;
    auto object = getCreatureOrCaller(game, args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    if (object) {
        result = object->attributes().getSkillRank(skill);
    } else {
        debug("Script: getSkillRank: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getAttackTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto creature = getCreatureOrCaller(game, args, 0, ctx);
    if (creature) {
        target = creature->getAttackTarget();
    } else {
        debug("Script: getAttackTarget: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getLastAttackType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastAttackMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getDistanceBetween2D(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto objectA = getSpatialObject(game, args, 0, ctx);
    auto objectB = getSpatialObject(game, args, 1, ctx);

    if (objectA && objectB) {
        result = objectA->getDistanceTo(glm::vec2(objectB->position()));
    } else if (!objectA) {
        debug("Script: getDistanceBetween2D: objectA is invalid", 1, DebugChannels::script);
    } else if (!objectB) {
        debug("Script: getDistanceBetween2D: objectB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable getIsInCombat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreatureOrCaller(game, args, 0, ctx);
    if (creature) {
        result = creature->isInCombat();
    } else {
        debug("Script: getIsInCombat: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getLastAssociateCommand(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable giveGoldToCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    auto gp = getInt(args, 1);
    if (creature) {
        creature->giveGold(gp);
    } else {
        debug("Script: giveGoldToCreature: creature is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable setIsDestroyable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setLocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getDoor(game, args, 0, ctx);
    bool locked = getBool(args, 1);

    if (target) {
        target->setLocked(locked);
    } else {
        debug("Script: setLocked: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getLocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto target = getDoor(game, args, 0, ctx);
    if (target) {
        result = target->isLocked();
    } else {
        debug("Script: getLocked: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getClickingObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setAssociateListenPatterns(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastWeaponUsed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastUsedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getAbilityModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIdentified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto item = getItem(game, args, 0, ctx);
    if (item) {
        result = item->isIdentified();
    } else {
        debug("Script: getIdentified: item is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable setIdentified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto item = getItem(game, args, 0, ctx);
    bool identified = getBool(args, 1);

    if (item) {
        item->setIdentified(identified);
    } else {
        debug("Script: setIdentified: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getDistanceBetweenLocations2D(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);

    if (locationA && locationB) {
        result = glm::distance(glm::vec2(locationA->position()), glm::vec2(locationB->position()));
    } else if (!locationA) {
        debug("Script: getDistanceBetweenLocations2D: locationA is invalid", 1, DebugChannels::script);
    } else if (!locationB) {
        debug("Script: getDistanceBetweenLocations2D: locationB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable getDistanceToObject2D(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = -1.0f;
    auto caller = getCallerAsSpatial(game, ctx);
    auto object = getSpatialObject(game, args, 0, ctx);

    if (caller && object) {
        result = caller->getDistanceTo(glm::vec2(object->position()));
    } else if (!caller) {
        debug("Script: getDistanceToObject2D: caller is invalid", 1, DebugChannels::script);
    } else if (!object) {
        debug("Script: getDistanceToObject2D: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable getBlockingDoor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsDoorActionPossible(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable doDoorAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstItemInInventory(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;

    auto target = getSpatialObjectOrCaller(game, args, 0, ctx);
    if (target) {
        item = target->getFirstItem();
    } else {
        debug("Script: getFirstItemInInventory: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getNextItemInInventory(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;

    auto target = getSpatialObjectOrCaller(game, args, 0, ctx);
    if (target) {
        item = target->getNextItem();
    } else {
        debug("Script: getNextItemInInventory: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable getClassByPosition(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = ClassType::Invalid;
    int position = getInt(args, 0);
    auto creature = getCreatureOrCaller(game, args, 1, ctx);

    if (creature) {
        result = creature->attributes().getClassByPosition(position);
    } else {
        debug("Script: getClassByPosition: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getLevelByPosition(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    int position = getInt(args, 0);

    if (creature) {
        result = creature->attributes().getLevelByPosition(position);
    } else {
        debug("Script: getLevelByPosition: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getLevelByClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    auto clazz = getEnum<ClassType>(args, 0);

    if (creature) {
        result = creature->attributes().getClassLevel(clazz);
    } else {
        debug("Script: getLevelByClass: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getDamageDealtByType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTotalDamageDealt(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastDamager(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastDisarmed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastDisturbed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastLocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastUnlocked(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getInventoryDisturbType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getInventoryDisturbItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showUpgradeScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGender(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = Gender::None;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature->gender();
    } else {
        debug("Script: getGender: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getAttemptedAttackTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto caller = getCallerAsCreature(game, ctx);
    if (caller) {
        target = caller->getAttemptedAttackTarget();
    } else {
        debug("Script: getAttemptedAttackTarget: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable playPazaak(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastPazaakResult(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable displayFeedBackText(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable addJournalQuestEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable removeJournalQuestEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getJournalEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable playRumblePattern(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable stopRumblePattern(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable sendMessageToPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getAttemptedSpellTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto caller = getCallerAsCreature(game, ctx);
    if (caller) {
        // TODO: implement
    } else {
        debug("Script: getAttemptedSpellTarget: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable getLastOpenedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto triggerrer = getTriggerrer(game, ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable getHasSpell(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto creature = getCreatureOrCaller(game, args, 1, ctx);
    auto spell = getEnum<ForcePower>(args, 0);

    if (creature) {
        // TODO: Force Powers, aka spells, are not supported at the moment
    } else {
        debug("Script: getHasSpell: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable openStore(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstFactionMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNextFactionMember(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getJournalQuestExperience(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable jumpToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(game, args, 0, ctx);
    bool walkStraightLine = getBool(args, 1, true);

    if (jumpTo) {
        auto action = game.services().actionFactory().newJumpToObject(move(jumpTo));
        getCaller(game, ctx)->addActionOnTop(move(action));
    } else {
        debug("Script: jumpToObject: jumpTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable setMapPinEnabled(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable popUpGUIPanel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable addMultiClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsLinkImmune(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable giveXPToCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    int xpAmount = getInt(args, 1);

    if (creature) {
        creature->giveXP(xpAmount);
    } else {
        debug("Script: giveXPToCreature: creature is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable setXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    int xpAmount = getInt(args, 1);

    if (creature) {
        creature->setXP(xpAmount);
    } else {
        debug("Script: setXP: creature is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature->xp();
    } else {
        debug("Script: getXP: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getBaseItemType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;

    auto item = getItem(game, args, 0, ctx);
    if (item) {
        result = item->baseItemType();
    } else {
        debug("Script: getBaseItemType: item is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable getItemHasItemProperty(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemACValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable exploreAreaForPlayer(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsEncounterCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastPlayerDying(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getStartingLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    const ModuleInfo &info = game.module()->info();
    auto location = make_shared<Location>(info.entryPosition, info.entryFacing);
    return Variable::ofLocation(move(location));
}

Variable changeToStandardFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creatureToChange = getCreature(game, args, 0, ctx);
    auto faction = getEnum<Faction>(args, 1);

    if (creatureToChange) {
        creatureToChange->setFaction(faction);
    } else {
        debug("Script: changeToStandardFaction: creatureToChange is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable speakOneLinerConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGold(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;
    auto target = getCreatureOrCaller(game, args, 0, ctx);
    if (target) {
        result = target->gold();
    } else {
        debug("Script: getGold: target is invalid", 1, DebugChannels::script);
    }
    return Variable::ofInt(result);
}

Variable getLastRespawnButtonPresser(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setLightsaberPowered(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsWeaponEffective(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastSpellHarmful(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastKiller(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellCastItem(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemActivated(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemActivator(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemActivatedTargetLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemActivatedTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsOpen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto object = getSpatialObject(game, args, 0, ctx);
    if (object) {
        result = object->isOpen();
    } else {
        debug("Script: getIsOpen: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable takeGoldFromCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto amount = getInt(args, 0);
    auto creatureToTakeFrom = getCreature(game, args, 1, ctx);
    auto destroy = getBool(args, 2);
    if (creatureToTakeFrom) {
        creatureToTakeFrom->takeGold(amount);
    } else {
        debug("Script: takeGoldFromCreature: creatureToTakeFrom is invalid", 1, DebugChannels::script);
    }
    if (!destroy) {
        auto caller = getCallerAsCreature(game, ctx);
        if (caller) {
            caller->giveGold(amount);
        } else {
            debug("Script: takeGoldFromCreature: caller is invalid", 1, DebugChannels::script);
        }
    }
    return Variable();
}

Variable getIsInConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getPlotFlag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto target = getObjectOrCaller(game, args, 0, ctx);
    if (target) {
        result = target->plotFlag();
    } else {
        debug("Script: getPlotFlag: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable setPlotFlag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto target = getObject(game, args, 0, ctx);
    bool plotFlag = getBool(args, 1);

    if (target) {
        target->setPlotFlag(plotFlag);
    } else {
        debug("Script: setPlotFlag: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable setDialogPlaceableCamera(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSoloMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(game.services().party().isSoloMode()));
}

Variable getNumStackedItems(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable surrenderToEnemies(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCreatureSize(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastTrapDetected(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNearestTrapToObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getAttemptedMovementTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getBlockingCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFortitudeSavingThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getWillSavingThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getReflexSavingThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getChallengeRating(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFoundEnemyCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getMovementRate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSubRace(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = Subrace::None;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature->subrace();
    } else {
        debug("Script: getSubRace: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable duplicateHeadAppearance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable cutsceneAttack(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(game, ctx);
    auto target = getSpatialObject(game, args, 0, ctx);
    int animation = getInt(args, 1);
    auto attackResult = getEnum<AttackResultType>(args, 2);
    int damage = getInt(args, 3);

    if (caller && target) {
        game.services().combat().addAttack(caller, target, nullptr, attackResult, damage);
    } else if (!caller) {
        debug("Script: cutsceneAttack: caller is invalid", 1, DebugChannels::script);
    } else if (!target) {
        debug("Script: cutsceneAttack: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable setCameraMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setLockOrientationInDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setLockHeadFollowInDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable cutsceneMove(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable enableVideoEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable startNewModule(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string moduleName(boost::to_lower_copy(getString(args, 0)));
    string waypoint(boost::to_lower_copy(getString(args, 1, "")));

    game.scheduleModuleTransition(moduleName, waypoint);

    return Variable();
}

Variable disableVideoEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getWeaponRanged(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable doSinglePlayerAutoSave(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGameDifficulty(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getUserActionsPending(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = 0;

    auto caller = getCallerAsCreature(game, ctx);
    if (caller) {
        result = caller->hasUserActionsPending();
    } else {
        debug("Script: getUserActionsPending: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable revealMap(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setTutorialWindowsEnabled(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showTutorialWindow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable startCreditSequence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable isCreditSequenceInProgress(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCurrentAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(game, args, 0, ctx);
    shared_ptr<Action> action(object->getCurrentAction());
    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

Variable getDifficultyModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getAppearanceType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable floatingTextStrRefOnCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable floatingTextStringOnCreature(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapDisarmable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapDetectable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapDetectedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapFlagged(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapBaseType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapOneShot(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapCreator(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapKeyTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapDisarmDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getTrapDetectDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLockKeyRequired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLockKeyTag(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLockLockable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLockUnlockDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLockLockDC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getPCLevellingUp(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getHasFeatEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setPlaceableIllumination(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getPlaceableIllumination(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsPlaceableObjectActionPossible(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable doPlaceableObjectAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto player = game.services().party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable getNextPC(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setTrapDetectedBy(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsTrapped(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable faceObjectAwayFromObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto facer = getSpatialObject(game, args, 0, ctx);
    auto objectToFaceAwayFrom = getSpatialObject(game, args, 1, ctx);

    if (facer && objectToFaceAwayFrom) {
        facer->faceAwayFrom(*objectToFaceAwayFrom);
    } else if (!facer) {
        debug("Script: faceObjectAwayFromObject: facer is invalid", 1, DebugChannels::script);
    } else if (!objectToFaceAwayFrom) {
        debug("Script: faceObjectAwayFromObject: objectToFaceAwayFrom is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable popUpDeathGUIPanel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setTrapDisabled(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastHostileActor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable exportAllCharacters(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getModuleName(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFactionLeader(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable endGame(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getRunScriptVar(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.scriptVar);
}

Variable getCreatureMovmentType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getHasInventory(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getStrRefSoundDuration(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getGlobalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofInt(static_cast<int>(game.getGlobalBoolean(id)));
}

Variable setGlobalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    bool value = getBool(args, 1);

    game.setGlobalBoolean(id, value);

    return Variable();
}

Variable getGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofInt(game.getGlobalNumber(id));
}

Variable setGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    int value = getInt(args, 1);

    game.setGlobalNumber(id, value);

    return Variable();
}

Variable addJournalWorldEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable addJournalWorldEntryStrref(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable barkString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable deleteJournalWorldAllEntries(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable deleteJournalWorldEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable deleteJournalWorldEntryStrref(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable playVisualAreaEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setJournalQuestEntryPicture(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLocalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);

    if (object) {
        result = object->getLocalBoolean(index);
    } else {
        debug("Script: getLocalBoolean: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable setLocalBoolean(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);
    bool value = getBool(args, 2);

    if (object) {
        object->setLocalBoolean(index, value);
    } else {
        debug("Script: setLocalBoolean: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getLocalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);

    if (object) {
        result = object->getLocalNumber(index);
    } else {
        debug("Script: getLocalNumber: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable setLocalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int index = getInt(args, 1);
    int value = getInt(args, 2);

    if (object) {
        object->setLocalNumber(index, value);
    } else {
        debug("Script: setLocalNumber: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getGlobalLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofLocation(game.getGlobalLocation(id));
}

Variable setGlobalLocation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    auto value = getLocationEngineType(args, 1);

    if (value) {
        game.setGlobalLocation(id, value);
    } else {
        debug("Script: setGlobalLocation: value is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable getIsConversationActive(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNPCAIStyle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = NPCAIStyle::DefaultAttack;

    auto creature = getCreature(game, args, 0, ctx);
    if (creature) {
        result = creature->aiStyle();
    } else {
        debug("Script: getNPCAIStyle: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable setNPCAIStyle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto creature = getCreature(game, args, 0, ctx);
    auto style = getEnum<NPCAIStyle>(args, 1);

    if (creature) {
        creature->setAIStyle(style);
    } else {
        debug("Script: setNPCAIStyle: creature is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable setNPCSelectability(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNPCSelectability(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable clearAllEffects(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(game, ctx);
    if (caller) {
        caller->clearAllEffects();
    } else {
        debug("Script: clearAllEffects: caller is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable getLastConversation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showPartySelectionGUI(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string exitScript(boost::to_lower_copy(getString(args, 0, "")));
    int forceNpc1 = getInt(args, 1);
    int forceNpc2 = getInt(args, 2);

    PartySelection::Context partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    game.openPartySelection(partyCtx);

    return Variable();
}

Variable getStandardFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = Faction::Invalid;

    auto object = getCreature(game, args, 0, ctx);
    if (object) {
        result = object->faction();
    } else {
        debug("Script: getStandardFaction: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable givePlotXP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getMinOneHP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto object = getObject(game, args, 0, ctx);
    if (object) {
        result = object->isMinOneHP();
    } else {
        debug("Script: getMinOneHP: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable setMinOneHP(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    bool minOneHP = getBool(args, 1);

    if (object) {
        object->setMinOneHP(minOneHP);
    } else {
        debug("Script: setMinOneHP: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable setGlobalFadeIn(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setGlobalFadeOut(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastHostileTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    uint32_t result = kObjectInvalid;

    auto attacker = getCreatureOrCaller(game, args, 0, ctx);
    if (attacker) {
        // TODO: implement
    } else {
        debug("Script: getLastHostileTarget: attacker is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(result);
}

Variable getLastAttackAction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto result = ActionType::QueueEmpty;

    auto attacker = getCreatureOrCaller(game, args, 0, ctx);
    if (attacker) {
        // TODO: implement
    } else {
        debug("Script: getLastAttackAction: attacker is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable getLastForcePowerUsed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastCombatFeatUsed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastAttackResult(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getWasForcePowerSuccessful(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFirstAttacker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getNextAttacker(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setFormation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setForcePowerUnsuccessful(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsDebilitated(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreatureOrCaller(game, args, 0, ctx);
    if (creature) {
        result = creature->isDebilitated();
    } else {
        debug("Script: getIsDebilitated: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable surrenderByFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable changeFactionByFaction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable playRoomAnimation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showGalaxyMap(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setAreaFogColor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable changeItemCost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsLiveContentAvailable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable resetDialogState(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setGoodEvilValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsPoisoned(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto creature = getCreatureOrCaller(game, args, 0, ctx);
    if (creature) {
        // TODO: implement
    } else {
        debug("Script: getSpellTarget: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable setSoloMode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto activate = getBool(args, 0);
    game.services().party().setSoloMode(activate);
    return Variable();
}

Variable cancelPostDialogCharacterSwitch(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setMaxHitPoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    int maxHP = getInt(args, 1);

    if (object) {
        object->setMaxHitPoints(maxHP);
    } else {
        debug("Script: setMaxHitPoints: object is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable noClicksFor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable holdWorldFadeInForDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable shipBuild(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(g_shipBuild));
}

Variable surrenderRetainBuffs(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable suppressStatusSummaryEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCheatCode(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable createItemOnFloor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getScriptParameter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setFadeUntilScript(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemComponent(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getItemComponentPieceValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showChemicalUpgradeScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getChemicals(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getChemicalPieceValue(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellForcePointCost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getFeatAcquired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellAcquired(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showSwoopUpgradeScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable grantFeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable grantSpell(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable spawnMine(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setFakeCombatState(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getOwnerDemolitionsSkill(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setOrientOnClick(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getInfluence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setInfluence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable modifyInfluence(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getRacialSubType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable incrementGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable decrementGlobalNumber(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setBonusForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable addBonusForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getBonusForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable yavinHackCloseDoor(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable isStealthed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable isMeditating(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable isInTotalDefense(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setHealTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getHealTarget(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getRandomDestination(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable isFormActive(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellFormMask(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSpellBaseForcePointCost(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setKeepStealthInDialog(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable hasLineOfSight(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable showDemoScreen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable forceHeartbeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable isRunning(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setForfeitConditions(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getLastForfeitViolation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable modifyReflexSavingThrowBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable modifyFortitudeSavingThrowBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable modifyWillSavingThrowBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getScriptStringParameter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getObjectPersonalSpace(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable adjustCreatureAttributes(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setCreatureAILevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable resetCreatureAILevel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable creatureFlourishWeapon(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable changeObjectAppearance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsXBox(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable playOverlayAnimation(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable unlockAllSongs(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable disableMap(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable detonateMine(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable disableHealthRegen(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setCurrentForm(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setDisableTransit(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setInputClass(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable setForceAlwaysUpdate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable enableRain(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable displayMessageBox(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable displayDatapad(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable removeHeartbeat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable removeEffectByID(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable removeEffectByExactMatch(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable adjustCreatureSkills(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getSkillRankBase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable enableRendering(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getCombatActionsPending(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable getIsPlayerMadeCharacter(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

} // namespace routine

} // namespace game

} // namespace reone
