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
 *  Implementation of routines related to object management.
 */

#include "routines.h"

#include <boost/algorithm/string.hpp>

#include "../../common/log.h"

#include "../enginetype/location.h"
#include "../game.h"

using namespace std;

using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::destroyObject(const VariablesList &args, ExecutionContext &ctx) {
    auto destroy = getSpatialObject(args, 0);
    if (destroy) {
        _game->module()->area()->destroyObject(*destroy);
    } else {
        debug("Script: destroyObject: destroy is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::getEnteringObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(getTriggerrer(ctx));
}

Variable Routines::getIsObjectValid(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    return Variable::ofInt(static_cast<int>(static_cast<bool>(object)));
}

Variable Routines::getObjectByTag(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Object> result;
    string tag(boost::to_lower_copy(getString(args, 0)));
    int nth = getInt(args, 1, 0);

    if (!tag.empty()) {
        result = _game->module()->area()->getObjectByTag(tag, nth);
    } else {
        // Apparently, empty tag in this context stands for the player
        result = _game->party().player();
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getWaypointByTag(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> result;
    string tag(boost::to_lower_copy(getString(args, 0)));

    for (auto &waypoint : _game->module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (waypoint->tag() == tag) {
            result = waypoint;
            break;
        }
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getArea(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(_game->module()->area());
}

Variable Routines::setLocked(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getDoor(args, 0);
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

    auto target = getDoor(args, 0);
    if (target) {
        result = target->isLocked();
    } else {
        debug("Script: getLocked: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getModule(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(_game->module());
}

Variable Routines::getTag(const VariablesList &args, ExecutionContext &ctx) {
    string result;

    auto object = getObject(args, 0);
    if (object) {
        result = object->tag();
    } else {
        debug("Script: getTag: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable Routines::getDistanceToObject(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;
    auto caller = getCallerAsSpatial(ctx);
    auto object = getSpatialObject(args, 0);

    if (caller && object) {
        result = caller->getDistanceTo(*object);
    } else if (!caller) {
        debug("Script: getDistanceToObject: caller is invalid", 1, DebugChannels::script);
    } else if (!object) {
        debug("Script: getDistanceToObject: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getDistanceToObject2D(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;
    auto caller = getCallerAsSpatial(ctx);
    auto object = getSpatialObject(args, 0);

    if (caller && object) {
        result = caller->getDistanceTo(glm::vec2(object->position()));
    } else if (!caller) {
        debug("Script: getDistanceToObject2D: caller is invalid", 1, DebugChannels::script);
    } else if (!object) {
        debug("Script: getDistanceToObject2D: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getExitingObject(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(getTriggerrer(ctx));
}

Variable Routines::getFacing(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;

    auto target = getSpatialObject(args, 0);
    if (target) {
        result = glm::degrees(target->facing());
    } else {
        debug("Script: getFacing: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getPosition(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 result(0.0f);

    auto target = getSpatialObject(args, 0);
    if (target) {
        result = target->position();
    } else {
        debug("Script: getPosition: target is invalid", 1, DebugChannels::script);
        return Variable::ofVector(glm::vec3(0.0f));
    }

    return Variable::ofVector(move(result));
}

Variable Routines::soundObjectPlay(const VariablesList &args, ExecutionContext &ctx) {
    auto sound = getSound(args, 0);
    if (sound) {
        sound->play();
    } else {
        debug("Script: soundObjectPlay: sound is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::soundObjectStop(const VariablesList &args, ExecutionContext &ctx) {
    auto sound = getSound(args, 0);
    if (sound) {
        sound->stop();
    } else {
        debug("Script: soundObjectStop: sound is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::getDistanceBetween(const VariablesList &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto objectA = getSpatialObject(args, 0);
    auto objectB = getSpatialObject(args, 1);

    if (objectA && objectB) {
        result = objectA->getDistanceTo(*objectB);
    } else if (!objectA) {
        debug("Script: getDistanceBetween: objectA is invalid", 1, DebugChannels::script);
    } else if (!objectB) {
        debug("Script: getDistanceBetween: objectB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getDistanceBetween2D(const VariablesList &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto objectA = getSpatialObject(args, 0);
    auto objectB = getSpatialObject(args, 1);

    if (objectA && objectB) {
        result = objectA->getDistanceTo(glm::vec2(objectB->position()));
    } else if (!objectA) {
        debug("Script: getDistanceBetween2D: objectA is invalid", 1, DebugChannels::script);
    } else if (!objectB) {
        debug("Script: getDistanceBetween2D: objectB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getIsDead(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature->isDead();
    } else {
        debug("Script: getIsDead: creature is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
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

Variable Routines::getIsOpen(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto object = getSpatialObject(args, 0);
    if (object) {
        result = object->isOpen();
    } else {
        debug("Script: getIsOpen: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
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

Variable Routines::getName(const VariablesList &args, ExecutionContext &ctx) {
    string result;

    auto object = getObject(args, 0);
    if (object) {
        result = object->name();
    } else {
        debug("Script: getName: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

Variable Routines::getObjectType(const VariablesList &args, ExecutionContext &ctx) {
    auto result = ObjectType::Invalid;

    auto target = getObject(args, 0);
    if (target) {
        result = target->type();
    } else {
        debug("Script: getObjectType: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getPlotFlag(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto target = getObjectOrCaller(args, 0, ctx);
    if (target) {
        result = target->plotFlag();
    } else {
        debug("Script: getPlotFlag: target is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::setPlotFlag(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getObject(args, 0);
    int plotFlag = getInt(args, 1);

    if (target) {
        target->setPlotFlag(plotFlag);
    } else {
        debug("Script: setPlotFlag: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::faceObjectAwayFromObject(const VariablesList &args, ExecutionContext &ctx) {
    auto facer = getSpatialObject(args, 0);
    auto objectToFaceAwayFrom = getSpatialObject(args, 1);

    if (facer && objectToFaceAwayFrom) {
        facer->faceAwayFrom(*objectToFaceAwayFrom);
    } else if (!facer) {
        debug("Script: faceObjectAwayFromObject: facer is invalid", 1, DebugChannels::script);
    } else if (!objectToFaceAwayFrom) {
        debug("Script: faceObjectAwayFromObject: objectToFaceAwayFrom is invalid", 1, DebugChannels::script);
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

Variable Routines::getLastOpenedBy(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofObject(getTriggerrer(ctx));
}

Variable Routines::getAreaUnescapable(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(_game->module()->area()->isUnescapable()));
}

Variable Routines::setAreaUnescapable(const VariablesList &args, ExecutionContext &ctx) {
    bool unescapable = getBool(args, 0);
    _game->module()->area()->setUnescapable(unescapable);
    return Variable();
}

Variable Routines::cutsceneAttack(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto target = getSpatialObject(args, 0);
    int animation = getInt(args, 1);
    auto attackResult = getEnum<AttackResultType>(args, 2);
    int damage = getInt(args, 3);

    if (caller && target) {
        _game->combat().addAttack(caller, target, nullptr, attackResult, damage);
    } else if (!caller) {
        debug("Script: cutsceneAttack: caller is invalid", 1, DebugChannels::script);
    } else if (!target) {
        debug("Script: cutsceneAttack: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::createObject(const VariablesList &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    string blueprintResRef(boost::to_lower_copy(getString(args, 1)));
    auto location = getLocationEngineType(args, 2);
    bool useAppearAnimation = getBool(args, 3, false);

    return Variable::ofObject(_game->module()->area()->createObject(objectType, blueprintResRef, location));
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

    CreatureFinder::CriteriaList criterias;
    criterias.push_back(make_pair(static_cast<CreatureType>(firstCriteriaType), firstCriteriaValue));
    if (secondCriteriaType != -1) {
        criterias.push_back(make_pair(static_cast<CreatureType>(secondCriteriaType), secondCriteriaValue));
    }
    if (thirdCriteriaType != -1) {
        criterias.push_back(make_pair(static_cast<CreatureType>(thirdCriteriaType), thirdCriteriaValue));
    }

    shared_ptr<Creature> creature(_game->module()->area()->creatureFinder().getNearestCreature(target, criterias, nth - 1));

    return Variable::ofObject(move(creature));
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

    CreatureFinder::CriteriaList criterias;
    criterias.push_back(make_pair(static_cast<CreatureType>(firstCriteriaType), firstCriteriaValue));
    if (secondCriteriaType != -1) {
        criterias.push_back(make_pair(static_cast<CreatureType>(secondCriteriaType), secondCriteriaValue));
    }
    if (thirdCriteriaType != -1) {
        criterias.push_back(make_pair(static_cast<CreatureType>(thirdCriteriaType), thirdCriteriaValue));
    }

    shared_ptr<Creature> creature(_game->module()->area()->creatureFinder().getNearestCreatureToLocation(*location, criterias, nth - 1));

    return Variable::ofObject(move(creature));
}

Variable Routines::getNearestObject(const VariablesList &args, ExecutionContext &ctx) {
    auto objectType = getEnum(args, 0, ObjectType::All);
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(_game->module()->area()->getNearestObject(target->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(move(object));
}

Variable Routines::getNearestObjectToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto objectType = getEnum<ObjectType>(args, 0);
    auto location = getLocationEngineType(args, 1);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(_game->module()->area()->getNearestObject(location->position(), nth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    }));

    return Variable::ofObject(move(object));
}

Variable Routines::getNearestObjectByTag(const VariablesList &args, ExecutionContext &ctx) {
    string tag(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int nth = getInt(args, 2, 1);

    shared_ptr<SpatialObject> object(_game->module()->area()->getNearestObject(target->position(), nth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    }));

    return Variable::ofObject(move(object));
}

Variable Routines::objectToString(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    string result;

    if (object) {
        result = str(boost::format("%x") % object->id());
    } else {
        debug("Script: objectToString: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofString(move(result));
}

} // namespace game

} // namespace reone
