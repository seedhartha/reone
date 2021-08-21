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

#include "../../routines.h"

#include "../../../../common/log.h"
#include "../../../../script/executioncontext.h"

#include "../../../game.h"
#include "../../../location.h"

#include "objectutil.h"

using namespace std;

using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

constexpr bool g_shipBuild = true;

Variable Routines::assignCommand(const VariablesList &args, ExecutionContext &ctx) {
    auto subject = getObject(args, 0, ctx);
    auto action = getAction(args, 1);

    if (subject) {
        auto objectAction = _game.services().actionFactory().newDoCommand(move(action));
        subject->addAction(move(objectAction));
    } else {
        debug("Script: assignCommand: subject is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::delayCommand(const VariablesList &args, ExecutionContext &ctx) {
    float seconds = getFloat(args, 0);
    auto action = getAction(args, 1);

    auto objectAction = _game.services().actionFactory().newDoCommand(move(action));
    getCaller(ctx)->delayAction(move(objectAction), seconds);

    return Variable();
}

Variable Routines::executeScript(const VariablesList &args, ExecutionContext &ctx) {
    string script(getString(args, 0));
    auto target = getObject(args, 1, ctx);
    int scriptVar = getInt(args, 2, -1);

    if (target) {
        _game.services().scriptRunner().run(script, target->id(), kObjectInvalid, kObjectInvalid, scriptVar);
    } else {
        debug("Script: executeScript: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::clearAllActions(const VariablesList &args, ExecutionContext &ctx) {
    getCaller(ctx)->clearAllActions();
    return Variable();
}

Variable Routines::setFacing(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    float direction = getFloat(args, 0);

    if (caller) {
        caller->setFacing(glm::radians(direction));
    } else {
        debug("Script: setFacing: caller is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::switchPlayerCharacter(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setAreaUnescapable(const VariablesList &args, ExecutionContext &ctx) {
    bool unescapable = getBool(args, 0);
    _game.module()->area()->setUnescapable(unescapable);
    return Variable();
}

Variable Routines::getAreaUnescapable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(_game.module()->area()->isUnescapable()));
}

Variable Routines::getArea(const VariablesList &args, ExecutionContext &ctx) {
    auto area = _game.module()->area();
    return Variable::ofObject(getObjectIdOrInvalid(area));
}

Variable Routines::getEnteringObject(const VariablesList &args, ExecutionContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable Routines::getExitingObject(const VariablesList &args, ExecutionContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable Routines::getPosition(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 result(0.0f);

    auto target = getSpatialObject(args, 0, ctx);
    if (target) {
        result = target->position();
    } else {
        debug("Script: getPosition: target is invalid", 1, DebugChannels::script);
        return Variable::ofVector(glm::vec3(0.0f));
    }

    return Variable::ofVector(move(result));
}

Variable Routines::getFacing(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;

    auto target = getSpatialObject(args, 0, ctx);
    if (target) {
        result = glm::degrees(target->getFacing());
    } else {
        debug("Script: getFacing: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getItemPossessor(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemPossessedBy(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;
    auto creature = getCreature(args, 0, ctx);
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

Variable Routines::createItemOnObject(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;
    string itemTemplate(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
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

Variable Routines::getLastAttacker(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNearestCreature(const VariablesList &args, ExecutionContext &ctx) {
    int firstCriteriaType = getInt(args, 0);
    int firstCriteriaValue = getInt(args, 1);
    auto target = getSpatialObjectOrCaller(args, 2, ctx);
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

    shared_ptr<Creature> creature(_game.module()->area()->getNearestCreature(target, criterias, nth - 1));

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable Routines::getDistanceToObject(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;
    auto caller = getCallerAsSpatial(ctx);
    auto object = getSpatialObject(args, 0, ctx);

    if (caller && object) {
        result = caller->getDistanceTo(*object);
    } else if (!caller) {
        debug("Script: getDistanceToObject: caller is invalid", 1, DebugChannels::script);
    } else if (!object) {
        debug("Script: getDistanceToObject: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getIsObjectValid(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(static_cast<bool>(object)));
}

Variable Routines::setCameraFacing(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::playSound(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellTargetObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCurrentHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto object = getObjectOrCaller(args, 0, ctx);
    if (object) {
        result = object->currentHitPoints();
    } else {
        debug("Script: getCurrentHitPoints: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getMaxHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto object = getObjectOrCaller(args, 0, ctx);
    if (object) {
        result = object->maxHitPoints();
    } else {
        debug("Script: getMaxHitPoints: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getLastItemEquipped(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSubScreenID(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::cancelCombat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCurrentForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getMaxForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::pauseGame(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setPlayerRestrictMode(const VariablesList &args, ExecutionContext &ctx) {
    bool restrict = getBool(args, 0);
    _game.module()->player().setRestrictMode(restrict);
    return Variable();
}

Variable Routines::getPlayerRestrictMode(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: why is this object necessary?
    auto object = getCreatureOrCaller(args, 0, ctx);
    return Variable::ofInt(static_cast<int>(_game.module()->player().isRestrictMode()));
}

Variable Routines::getCasterLevel(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNextEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::removeEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstObjectInArea(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNextObjectInArea(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getMetaMagicFeat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getObjectType(const VariablesList &args, ExecutionContext &ctx) {
    auto result = ObjectType::Invalid;

    auto target = getObject(args, 0, ctx);
    if (target) {
        result = target->type();
    } else {
        debug("Script: getObjectType: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getRacialType(const VariablesList &args, ExecutionContext &ctx) {
    auto result = RacialType::Invalid;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature->racialType();
    } else {
        debug("Script: getRacialType: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::fortitudeSave(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::reflexSave(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::willSave(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellSaveDC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getAC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGoodEvilValue(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getAlignmentGoodEvil(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstObjectInShape(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNextObjectInShape(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemStackSize(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto item = getItem(args, 0, ctx);
    if (item) {
        result = item->stackSize();
    } else {
        debug("Script: getItemStackSize: item is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getAbilityScore(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreature(args, 0, ctx);
    auto type = getEnum<Ability>(args, 1);

    if (creature) {
        result = creature->attributes().getAbilityScore(type);
    } else {
        debug("Script: getAbilityScore: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getIsDead(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature->isDead();
    } else {
        debug("Script: getIsDead: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setFacingPoint(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    if (caller) {
        glm::vec3 target(getVector(args, 0));
        caller->face(target);
    } else {
        debug("Script: setFacingPoint: caller is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::touchAttackMelee(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::touchAttackRanged(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setItemStackSize(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    int stackSize = getInt(args, 1);

    if (item) {
        item->setStackSize(stackSize);
    } else {
        debug("Script: setItemStackSize: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getDistanceBetween(const VariablesList &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto objectA = getSpatialObject(args, 0, ctx);
    auto objectB = getSpatialObject(args, 1, ctx);

    if (objectA && objectB) {
        result = objectA->getDistanceTo(*objectB);
    } else if (!objectA) {
        debug("Script: getDistanceBetween: objectA is invalid", 1, DebugChannels::script);
    } else if (!objectB) {
        debug("Script: getDistanceBetween: objectB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::setReturnStrref(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemInSlot(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    int slot = getInt(args, 0);

    if (creature) {
        item = creature->getEquippedItem(slot);
    } else {
        debug("Script: getItemInSlot: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable Routines::setGlobalString(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    string value(getString(args, 1));

    _game.setGlobalString(id, value);

    return Variable();
}

Variable Routines::setCommandable(const VariablesList &args, ExecutionContext &ctx) {
    bool commandable = getBool(args, 0);
    auto target = getObjectOrCaller(args, 1, ctx);

    if (target) {
        target->setCommandable(commandable);
    } else {
        debug("Script: setCommandable: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getCommandable(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto target = getObjectOrCaller(args, 0, ctx);
    if (target) {
        result = target->isCommandable();
    } else {
        debug("Script: getCommandable: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getHitDice(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature->attributes().getAggregateLevel();
    } else {
        debug("Script: getGender: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getTag(const VariablesList &args, ExecutionContext &ctx) {
    string result;

    auto object = getObject(args, 0, ctx);
    if (object) {
        result = object->tag();
    } else {
        debug("Script: getTag: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable Routines::resistForce(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionEqual(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto firstObject = getCreature(args, 0, ctx);
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

Variable Routines::changeFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto objectToChangeFaction = getCreature(args, 0, ctx);
    auto memberOfFactionToJoin = getCreature(args, 1, ctx);

    if (objectToChangeFaction && memberOfFactionToJoin) {
        objectToChangeFaction->setFaction(memberOfFactionToJoin->faction());
    } else if (!objectToChangeFaction) {
        debug("Script: changeFaction: objectToChangeFaction is invalid", 1, DebugChannels::script);
    } else if (!memberOfFactionToJoin) {
        debug("Script: changeFaction: memberOfFactionToJoin is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getIsListening(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setListening(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setListenPattern(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionWeakestMember(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionStrongestMember(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionMostDamagedMember(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionLeastDamagedMember(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionGold(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionAverageReputation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionAverageGoodEvilAlignment(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionAverageLevel(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionAverageXP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionMostFrequentClass(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionWorstAC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionBestAC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGlobalString(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofString(_game.getGlobalString(id));
}

Variable Routines::getListenPatternNumber(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofString(_game.getGlobalString(id));
}

Variable Routines::getWaypointByTag(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> object;
    string tag(boost::to_lower_copy(getString(args, 0)));

    for (auto &waypoint : _game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (waypoint->tag() == tag) {
            object = waypoint;
            break;
        }
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable Routines::getTransitionTarget(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getObjectByTag(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Object> object;
    string tag(boost::to_lower_copy(getString(args, 0)));
    int nth = getInt(args, 1, 0);

    if (!tag.empty()) {
        object = _game.module()->area()->getObjectByTag(tag, nth);
    } else {
        // Apparently, empty tag in this context stands for the player
        object = _game.services().party().player();
    }

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable Routines::adjustAlignment(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setAreaTransitionBMP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getReputation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::adjustReputation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getModuleFileName(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGoingToBeAttackedBy(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLocation(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Location> result;

    auto object = getSpatialObject(args, 0, ctx);
    if (object) {
        glm::vec3 position(object->position());
        float facing = object->getFacing();
        result = make_shared<Location>(move(position), facing);
    } else {
        debug("Script: getLocation: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofLocation(move(result));
}

Variable Routines::location(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));
    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable Routines::applyEffectAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));
    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable Routines::applyEffectToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto durationType = getEnum<DurationType>(args, 0);
    auto effect = getEffect(args, 1);
    auto target = getSpatialObject(args, 2, ctx);
    float duration = getFloat(args, 3, 0.0f);

    if (target) {
        target->applyEffect(effect, durationType, duration);
    } else {
        debug("Script: applyEffectToObject: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getIsPC(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature == _game.services().party().player();
    } else {
        debug("Script: getIsPC: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::speakString(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellTargetLocation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getPositionFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 result(0.0f);

    auto location = getLocationEngineType(args, 0);
    if (location) {
        result = location->position();
    } else {
        debug("Script: getPositionFromLocation: location is invalid", 1, DebugChannels::script);
    }

    return Variable::ofVector(move(result));
}

Variable Routines::getFacingFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;

    auto location = getLocationEngineType(args, 0);
    if (location) {
        result = glm::degrees(location->facing());
    } else {
        debug("Script: getFacingFromLocation: location is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getNearestCreatureToLocation(const VariablesList &args, ExecutionContext &ctx) {
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

    shared_ptr<Creature> creature(_game.module()->area()->getNearestCreatureToLocation(*location, criterias, nth - 1));

    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

Variable Routines::getNearestObject(const VariablesList &args, ExecutionContext &ctx) {
    auto objectType = getEnum(args, 0, ObjectType::All);
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(_game.module()->area()->getNearestObject(target->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable Routines::getNearestObjectToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    auto location = getLocationEngineType(args, 1);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(_game.module()->area()->getNearestObject(location->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable Routines::getNearestObjectByTag(const VariablesList &args, ExecutionContext &ctx) {
    string tag(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(_game.module()->area()->getNearestObject(target->position(), nth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    }));

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable Routines::getIsEnemy(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0, ctx);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = _game.services().reputes().getIsEnemy(*target, *source);
    } else if (!target) {
        debug("Script: getIsEnemy: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsEnemy: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getIsFriend(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0, ctx);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = _game.services().reputes().getIsFriend(*target, *source);
    } else if (!target) {
        debug("Script: getIsFriend: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsFriend: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getIsNeutral(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto target = getCreature(args, 0, ctx);
    auto source = getCreatureOrCaller(args, 1, ctx);

    if (target && source) {
        result = _game.services().reputes().getIsNeutral(*target, *source);
    } else if (!target) {
        debug("Script: getIsNeutral: target is invalid", 1, DebugChannels::script);
    } else if (!source) {
        debug("Script: getIsNeutral: source is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getPCSpeaker(const VariablesList &args, ExecutionContext &ctx) {
    auto player = _game.services().party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable Routines::getStringByStrRef(const VariablesList &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    return Variable::ofString(_game.services().resource().strings().get(strRef));
}

Variable Routines::destroyObject(const VariablesList &args, ExecutionContext &ctx) {
    auto destroy = getSpatialObject(args, 0, ctx);
    if (destroy) {
        _game.module()->area()->destroyObject(*destroy);
    } else {
        debug("Script: destroyObject: destroy is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::getModule(const VariablesList &args, ExecutionContext &ctx) {
    auto module = _game.module();
    return Variable::ofObject(getObjectIdOrInvalid(module));
}

Variable Routines::createObject(const VariablesList &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    string blueprintResRef(boost::to_lower_copy(getString(args, 1)));
    auto location = getLocationEngineType(args, 2);
    bool useAppearAnimation = getBool(args, 3, false);

    auto object = _game.module()->area()->createObject(objectType, blueprintResRef, location);

    return Variable::ofObject(getObjectIdOrInvalid(object));
}

Variable Routines::getLastSpellCaster(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastSpell(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getUserDefinedEventNumber(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.userDefinedEventNumber);
}

Variable Routines::getSpellId(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::randomName(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLoadFromSaveGame(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(_game.isLoadFromSaveGame()));
}

Variable Routines::getName(const VariablesList &args, ExecutionContext &ctx) {
    string result;

    auto object = getObject(args, 0, ctx);
    if (object) {
        result = object->name();
    } else {
        debug("Script: getName: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable Routines::getLastSpeaker(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::beginConversation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastClosedBy(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstInPersistentObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNextInPersistentObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getAreaOfEffectCreator(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showLevelUpGUI(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setItemNonEquippable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getButtonMashCheck(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setButtonMashCheck(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::giveItem(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::objectToString(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    string result;

    if (object) {
        result = str(boost::format("%x") % object->id());
    } else {
        debug("Script: objectToString: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable Routines::getIsImmune(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getModuleItemAcquired(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getModuleItemAcquiredFrom(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setCustomToken(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getHasFeat(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto feat = getEnum<FeatType>(args, 0);
    auto creature = getCreatureOrCaller(args, 1, ctx);

    if (creature) {
        result = creature->attributes().hasFeat(feat);
    } else {
        debug("Script: getHasFeat: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getHasSkill(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    if (creature) {
        result = creature->attributes().hasSkill(skill);
    } else {
        debug("Script: getHasSkill: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLastPlayerDied(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getModuleItemLost(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getModuleItemLostBy(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getDistanceBetweenLocations(const VariablesList &args, ExecutionContext &ctx) {
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

Variable Routines::getReflexAdjustedDamage(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::playAnimation(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
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

Variable Routines::getHasSpellEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCreatureHasTalent(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCreatureTalentRandom(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCreatureTalentBest(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGoldPieceValue(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsPlayableRacialType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::jumpToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    if (destination) {
        auto action = _game.services().actionFactory().newJumpToLocation(move(destination));
        getCaller(ctx)->addActionOnTop(move(action));
    } else {
        debug("Script: jumpToLocation: destination is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::getSkillRank(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto object = getCreatureOrCaller(args, 1, ctx);
    auto skill = getEnum<SkillType>(args, 0);

    if (object) {
        result = object->attributes().getSkillRank(skill);
    } else {
        debug("Script: getSkillRank: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getAttackTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        target = creature->getAttackTarget();
    } else {
        debug("Script: getAttackTarget: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable Routines::getLastAttackType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastAttackMode(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getDistanceBetween2D(const VariablesList &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto objectA = getSpatialObject(args, 0, ctx);
    auto objectB = getSpatialObject(args, 1, ctx);

    if (objectA && objectB) {
        result = objectA->getDistanceTo(glm::vec2(objectB->position()));
    } else if (!objectA) {
        debug("Script: getDistanceBetween2D: objectA is invalid", 1, DebugChannels::script);
    } else if (!objectB) {
        debug("Script: getDistanceBetween2D: objectB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getIsInCombat(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        result = creature->isInCombat();
    } else {
        debug("Script: getIsInCombat: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLastAssociateCommand(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::giveGoldToCreature(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    auto gp = getInt(args, 1);
    if (creature) {
        creature->giveGold(gp);
    } else {
        debug("Script: giveGoldToCreature: creature is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::setIsDestroyable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setLocked(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getDoor(args, 0, ctx);
    bool locked = getBool(args, 1);

    if (target) {
        target->setLocked(locked);
    } else {
        debug("Script: setLocked: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getLocked(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto target = getDoor(args, 0, ctx);
    if (target) {
        result = target->isLocked();
    } else {
        debug("Script: getLocked: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getClickingObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setAssociateListenPatterns(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastWeaponUsed(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastUsedBy(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getAbilityModifier(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIdentified(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto item = getItem(args, 0, ctx);
    if (item) {
        result = item->isIdentified();
    } else {
        debug("Script: getIdentified: item is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setIdentified(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    bool identified = getBool(args, 1);

    if (item) {
        item->setIdentified(identified);
    } else {
        debug("Script: setIdentified: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getDistanceBetweenLocations2D(const VariablesList &args, ExecutionContext &ctx) {
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

Variable Routines::getDistanceToObject2D(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;
    auto caller = getCallerAsSpatial(ctx);
    auto object = getSpatialObject(args, 0, ctx);

    if (caller && object) {
        result = caller->getDistanceTo(glm::vec2(object->position()));
    } else if (!caller) {
        debug("Script: getDistanceToObject2D: caller is invalid", 1, DebugChannels::script);
    } else if (!object) {
        debug("Script: getDistanceToObject2D: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getBlockingDoor(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsDoorActionPossible(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::doDoorAction(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstItemInInventory(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;

    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    if (target) {
        item = target->getFirstItem();
    } else {
        debug("Script: getFirstItemInInventory: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable Routines::getNextItemInInventory(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> item;

    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    if (target) {
        item = target->getNextItem();
    } else {
        debug("Script: getNextItemInInventory: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(item));
}

Variable Routines::getClassByPosition(const VariablesList &args, ExecutionContext &ctx) {
    auto result = ClassType::Invalid;
    int position = getInt(args, 0);
    auto creature = getCreatureOrCaller(args, 1, ctx);

    if (creature) {
        result = creature->attributes().getClassByPosition(position);
    } else {
        debug("Script: getClassByPosition: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLevelByPosition(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    int position = getInt(args, 0);

    if (creature) {
        result = creature->attributes().getLevelByPosition(position);
    } else {
        debug("Script: getLevelByPosition: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getLevelByClass(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto clazz = getEnum<ClassType>(args, 0);

    if (creature) {
        result = creature->attributes().getClassLevel(clazz);
    } else {
        debug("Script: getLevelByClass: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getDamageDealtByType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTotalDamageDealt(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastDamager(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastDisarmed(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastDisturbed(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastLocked(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastUnlocked(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getInventoryDisturbType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getInventoryDisturbItem(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showUpgradeScreen(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGender(const VariablesList &args, ExecutionContext &ctx) {
    auto result = Gender::None;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature->gender();
    } else {
        debug("Script: getGender: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getAttemptedAttackTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        target = caller->getAttemptedAttackTarget();
    } else {
        debug("Script: getAttemptedAttackTarget: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable Routines::playPazaak(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastPazaakResult(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::displayFeedBackText(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addJournalQuestEntry(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::removeJournalQuestEntry(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getJournalEntry(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::playRumblePattern(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::stopRumblePattern(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::sendMessageToPC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getAttemptedSpellTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        // TODO: implement
    } else {
        debug("Script: getAttemptedSpellTarget: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable Routines::getLastOpenedBy(const VariablesList &args, ExecutionContext &ctx) {
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

Variable Routines::getHasSpell(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto spell = getEnum<ForcePower>(args, 0);

    if (creature) {
        // TODO: Force Powers, aka spells, are not supported at the moment
    } else {
        debug("Script: getHasSpell: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::openStore(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstFactionMember(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNextFactionMember(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getJournalQuestExperience(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::jumpToObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getBool(args, 1, true);

    if (jumpTo) {
        auto action = _game.services().actionFactory().newJumpToObject(move(jumpTo));
        getCaller(ctx)->addActionOnTop(move(action));
    } else {
        debug("Script: jumpToObject: jumpTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::setMapPinEnabled(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::popUpGUIPanel(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addMultiClass(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsLinkImmune(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::giveXPToCreature(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    if (creature) {
        creature->giveXP(xpAmount);
    } else {
        debug("Script: giveXPToCreature: creature is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::setXP(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    int xpAmount = getInt(args, 1);

    if (creature) {
        creature->setXP(xpAmount);
    } else {
        debug("Script: setXP: creature is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getXP(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature->xp();
    } else {
        debug("Script: getXP: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getBaseItemType(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto item = getItem(args, 0, ctx);
    if (item) {
        result = item->baseItemType();
    } else {
        debug("Script: getBaseItemType: item is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::getItemHasItemProperty(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemACValue(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::exploreAreaForPlayer(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsEncounterCreature(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastPlayerDying(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getStartingLocation(const VariablesList &args, ExecutionContext &ctx) {
    const ModuleInfo &info = _game.module()->info();
    auto location = make_shared<Location>(info.entryPosition, info.entryFacing);
    return Variable::ofLocation(move(location));
}

Variable Routines::changeToStandardFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto creatureToChange = getCreature(args, 0, ctx);
    auto faction = getEnum<Faction>(args, 1);

    if (creatureToChange) {
        creatureToChange->setFaction(faction);
    } else {
        debug("Script: changeToStandardFaction: creatureToChange is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::speakOneLinerConversation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGold(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto target = getCreatureOrCaller(args, 0, ctx);
    if (target) {
        result = target->gold();
    } else {
        debug("Script: getGold: target is invalid", 1, DebugChannels::script);
    }
    return Variable::ofInt(result);
}

Variable Routines::getLastRespawnButtonPresser(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setLightsaberPowered(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsWeaponEffective(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastSpellHarmful(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastKiller(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellCastItem(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemActivated(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemActivator(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemActivatedTargetLocation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemActivatedTarget(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsOpen(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto object = getSpatialObject(args, 0, ctx);
    if (object) {
        result = object->isOpen();
    } else {
        debug("Script: getIsOpen: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::takeGoldFromCreature(const VariablesList &args, ExecutionContext &ctx) {
    auto amount = getInt(args, 0);
    auto creatureToTakeFrom = getCreature(args, 1, ctx);
    auto destroy = getBool(args, 2);
    if (creatureToTakeFrom) {
        creatureToTakeFrom->takeGold(amount);
    } else {
        debug("Script: takeGoldFromCreature: creatureToTakeFrom is invalid", 1, DebugChannels::script);
    }
    if (!destroy) {
        auto caller = getCallerAsCreature(ctx);
        if (caller) {
            caller->giveGold(amount);
        } else {
            debug("Script: takeGoldFromCreature: caller is invalid", 1, DebugChannels::script);
        }
    }
    return Variable();
}

Variable Routines::getIsInConversation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getPlotFlag(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto target = getObjectOrCaller(args, 0, ctx);
    if (target) {
        result = target->plotFlag();
    } else {
        debug("Script: getPlotFlag: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setPlotFlag(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getObject(args, 0, ctx);
    bool plotFlag = getBool(args, 1);

    if (target) {
        target->setPlotFlag(plotFlag);
    } else {
        debug("Script: setPlotFlag: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::setDialogPlaceableCamera(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSoloMode(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(_game.services().party().isSoloMode()));
}

Variable Routines::getNumStackedItems(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::surrenderToEnemies(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCreatureSize(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastTrapDetected(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNearestTrapToObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getAttemptedMovementTarget(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getBlockingCreature(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFortitudeSavingThrow(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getWillSavingThrow(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getReflexSavingThrow(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getChallengeRating(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFoundEnemyCreature(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getMovementRate(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSubRace(const VariablesList &args, ExecutionContext &ctx) {
    auto result = Subrace::None;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature->subrace();
    } else {
        debug("Script: getSubRace: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::duplicateHeadAppearance(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::cutsceneAttack(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto target = getSpatialObject(args, 0, ctx);
    int animation = getInt(args, 1);
    auto attackResult = getEnum<AttackResultType>(args, 2);
    int damage = getInt(args, 3);

    if (caller && target) {
        _game.services().combat().addAttack(caller, target, nullptr, attackResult, damage);
    } else if (!caller) {
        debug("Script: cutsceneAttack: caller is invalid", 1, DebugChannels::script);
    } else if (!target) {
        debug("Script: cutsceneAttack: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::setCameraMode(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setLockOrientationInDialog(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setLockHeadFollowInDialog(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::cutsceneMove(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::enableVideoEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::startNewModule(const VariablesList &args, ExecutionContext &ctx) {
    string moduleName(boost::to_lower_copy(getString(args, 0)));
    string waypoint(boost::to_lower_copy(getString(args, 1, "")));

    _game.scheduleModuleTransition(moduleName, waypoint);

    return Variable();
}

Variable Routines::disableVideoEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getWeaponRanged(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::doSinglePlayerAutoSave(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGameDifficulty(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getUserActionsPending(const VariablesList &args, ExecutionContext &ctx) {
    bool result = 0;

    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        result = caller->hasUserActionsPending();
    } else {
        debug("Script: getUserActionsPending: caller is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::revealMap(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setTutorialWindowsEnabled(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showTutorialWindow(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::startCreditSequence(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::isCreditSequenceInProgress(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCurrentAction(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    shared_ptr<Action> action(object->getCurrentAction());
    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

Variable Routines::getDifficultyModifier(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getAppearanceType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::floatingTextStrRefOnCreature(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::floatingTextStringOnCreature(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapDisarmable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapDetectable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapDetectedBy(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapFlagged(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapBaseType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapOneShot(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapCreator(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapKeyTag(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapDisarmDC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getTrapDetectDC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLockKeyRequired(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLockKeyTag(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLockLockable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLockUnlockDC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLockLockDC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getPCLevellingUp(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getHasFeatEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setPlaceableIllumination(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getPlaceableIllumination(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsPlaceableObjectActionPossible(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::doPlaceableObjectAction(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstPC(const VariablesList &args, ExecutionContext &ctx) {
    auto player = _game.services().party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

Variable Routines::getNextPC(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setTrapDetectedBy(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsTrapped(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::faceObjectAwayFromObject(const VariablesList &args, ExecutionContext &ctx) {
    auto facer = getSpatialObject(args, 0, ctx);
    auto objectToFaceAwayFrom = getSpatialObject(args, 1, ctx);

    if (facer && objectToFaceAwayFrom) {
        facer->faceAwayFrom(*objectToFaceAwayFrom);
    } else if (!facer) {
        debug("Script: faceObjectAwayFromObject: facer is invalid", 1, DebugChannels::script);
    } else if (!objectToFaceAwayFrom) {
        debug("Script: faceObjectAwayFromObject: objectToFaceAwayFrom is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::popUpDeathGUIPanel(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setTrapDisabled(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastHostileActor(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::exportAllCharacters(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getModuleName(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFactionLeader(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::endGame(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getRunScriptVar(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.scriptVar);
}

Variable Routines::getCreatureMovmentType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getHasInventory(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getStrRefSoundDuration(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getGlobalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofInt(static_cast<int>(_game.getGlobalBoolean(id)));
}

Variable Routines::setGlobalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    bool value = getBool(args, 1);

    _game.setGlobalBoolean(id, value);

    return Variable();
}

Variable Routines::getGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofInt(_game.getGlobalNumber(id));
}

Variable Routines::setGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    int value = getInt(args, 1);

    _game.setGlobalNumber(id, value);

    return Variable();
}

Variable Routines::addJournalWorldEntry(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addJournalWorldEntryStrref(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::barkString(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::deleteJournalWorldAllEntries(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::deleteJournalWorldEntry(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::deleteJournalWorldEntryStrref(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::playVisualAreaEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setJournalQuestEntryPicture(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLocalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);

    if (object) {
        result = object->getLocalBoolean(index);
    } else {
        debug("Script: getLocalBoolean: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setLocalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    bool value = getBool(args, 2);

    if (object) {
        object->setLocalBoolean(index, value);
    } else {
        debug("Script: setLocalBoolean: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getLocalNumber(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);

    if (object) {
        result = object->getLocalNumber(index);
    } else {
        debug("Script: getLocalNumber: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::setLocalNumber(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    int value = getInt(args, 2);

    if (object) {
        object->setLocalNumber(index, value);
    } else {
        debug("Script: setLocalNumber: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getGlobalLocation(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofLocation(_game.getGlobalLocation(id));
}

Variable Routines::setGlobalLocation(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    auto value = getLocationEngineType(args, 1);

    if (value) {
        _game.setGlobalLocation(id, value);
    } else {
        debug("Script: setGlobalLocation: value is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getIsConversationActive(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNPCAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    auto result = NPCAIStyle::DefaultAttack;

    auto creature = getCreature(args, 0, ctx);
    if (creature) {
        result = creature->aiStyle();
    } else {
        debug("Script: getNPCAIStyle: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setNPCAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0, ctx);
    auto style = getEnum<NPCAIStyle>(args, 1);

    if (creature) {
        creature->setAIStyle(style);
    } else {
        debug("Script: setNPCAIStyle: creature is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::setNPCSelectability(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNPCSelectability(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::clearAllEffects(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    if (caller) {
        caller->clearAllEffects();
    } else {
        debug("Script: clearAllEffects: caller is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::getLastConversation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showPartySelectionGUI(const VariablesList &args, ExecutionContext &ctx) {
    string exitScript(boost::to_lower_copy(getString(args, 0, "")));
    int forceNpc1 = getInt(args, 1);
    int forceNpc2 = getInt(args, 2);

    PartySelection::Context partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    _game.openPartySelection(partyCtx);

    return Variable();
}

Variable Routines::getStandardFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto result = Faction::Invalid;

    auto object = getCreature(args, 0, ctx);
    if (object) {
        result = object->faction();
    } else {
        debug("Script: getStandardFaction: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::givePlotXP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getMinOneHP(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto object = getObject(args, 0, ctx);
    if (object) {
        result = object->isMinOneHP();
    } else {
        debug("Script: getMinOneHP: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setMinOneHP(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    bool minOneHP = getBool(args, 1);

    if (object) {
        object->setMinOneHP(minOneHP);
    } else {
        debug("Script: setMinOneHP: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::setGlobalFadeIn(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setGlobalFadeOut(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastHostileTarget(const VariablesList &args, ExecutionContext &ctx) {
    uint32_t result = kObjectInvalid;

    auto attacker = getCreatureOrCaller(args, 0, ctx);
    if (attacker) {
        // TODO: implement
    } else {
        debug("Script: getLastHostileTarget: attacker is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(result);
}

Variable Routines::getLastAttackAction(const VariablesList &args, ExecutionContext &ctx) {
    auto result = ActionType::QueueEmpty;

    auto attacker = getCreatureOrCaller(args, 0, ctx);
    if (attacker) {
        // TODO: implement
    } else {
        debug("Script: getLastAttackAction: attacker is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLastForcePowerUsed(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastCombatFeatUsed(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastAttackResult(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getWasForcePowerSuccessful(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFirstAttacker(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getNextAttacker(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setFormation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setForcePowerUnsuccessful(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsDebilitated(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        result = creature->isDebilitated();
    } else {
        debug("Script: getIsDebilitated: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::surrenderByFaction(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::changeFactionByFaction(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::playRoomAnimation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showGalaxyMap(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setAreaFogColor(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::changeItemCost(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsLiveContentAvailable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::resetDialogState(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setGoodEvilValue(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsPoisoned(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellTarget(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> target;

    auto creature = getCreatureOrCaller(args, 0, ctx);
    if (creature) {
        // TODO: implement
    } else {
        debug("Script: getSpellTarget: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofObject(getObjectIdOrInvalid(target));
}

Variable Routines::setSoloMode(const VariablesList &args, ExecutionContext &ctx) {
    auto activate = getBool(args, 0);
    _game.services().party().setSoloMode(activate);
    return Variable();
}

Variable Routines::cancelPostDialogCharacterSwitch(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setMaxHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int maxHP = getInt(args, 1);

    if (object) {
        object->setMaxHitPoints(maxHP);
    } else {
        debug("Script: setMaxHitPoints: object is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::noClicksFor(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::holdWorldFadeInForDialog(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::shipBuild(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(g_shipBuild));
}

Variable Routines::surrenderRetainBuffs(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::suppressStatusSummaryEntry(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCheatCode(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::createItemOnFloor(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getScriptParameter(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setFadeUntilScript(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemComponent(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getItemComponentPieceValue(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showChemicalUpgradeScreen(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getChemicals(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getChemicalPieceValue(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellForcePointCost(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getFeatAcquired(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellAcquired(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showSwoopUpgradeScreen(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::grantFeat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::grantSpell(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::spawnMine(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setFakeCombatState(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getOwnerDemolitionsSkill(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setOrientOnClick(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getInfluence(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setInfluence(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::modifyInfluence(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getRacialSubType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::incrementGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::decrementGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setBonusForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::addBonusForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getBonusForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::yavinHackCloseDoor(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::isStealthed(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::isMeditating(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::isInTotalDefense(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setHealTarget(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getHealTarget(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getRandomDestination(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::isFormActive(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellFormMask(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSpellBaseForcePointCost(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setKeepStealthInDialog(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::hasLineOfSight(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::showDemoScreen(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::forceHeartbeat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::isRunning(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setForfeitConditions(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getLastForfeitViolation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::modifyReflexSavingThrowBase(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::modifyFortitudeSavingThrowBase(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::modifyWillSavingThrowBase(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getScriptStringParameter(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getObjectPersonalSpace(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::adjustCreatureAttributes(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setCreatureAILevel(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::resetCreatureAILevel(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::creatureFlourishWeapon(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::changeObjectAppearance(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsXBox(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::playOverlayAnimation(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::unlockAllSongs(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::disableMap(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::detonateMine(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::disableHealthRegen(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setCurrentForm(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setDisableTransit(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setInputClass(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::setForceAlwaysUpdate(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::enableRain(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::displayMessageBox(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::displayDatapad(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::removeHeartbeat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::removeEffectByID(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::removeEffectByExactMatch(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::adjustCreatureSkills(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getSkillRankBase(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::enableRendering(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getCombatActionsPending(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getIsPlayerMadeCharacter(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

} // namespace game

} // namespace reone
