/*
 * Copyright (c) 2020 The reone project contributors
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

#include "routines.h"

#include <boost/algorithm/string.hpp>

#include "../../common/log.h"

#include "../blueprint/blueprints.h"
#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::destroyObject(const VariablesList &args, ExecutionContext &ctx) {
    auto destroy = getSpatialObject(args, 0);
    if (destroy) {
        _game->module()->area()->destroyObject(*destroy);
    } else {
        warn("Routines: destroyObject: destroy is invalid");
    }
    return Variable();
}

Variable Routines::getEnteringObject(const VariablesList &args, ExecutionContext &ctx) {
    return getTriggerrer(ctx);
}

Variable Routines::getIsObjectValid(const VariablesList &args, ExecutionContext &ctx) {
    return static_cast<bool>(getObject(args, 0));
}

Variable Routines::getObjectByTag(const VariablesList &args, ExecutionContext &ctx) {
    string tag(getString(args, 0));
    boost::to_lower(tag);

    // Apparently, empty string in this context stands for the player
    if (tag.empty()) {
        return _game->party().player();
    }
    int nth = getInt(args, 1, 0);

    return _game->module()->area()->find(tag, nth);
}

Variable Routines::getWaypointByTag(const VariablesList &args, ExecutionContext &ctx) {
    string tag(getString(args, 0));
    boost::to_lower(tag);

    return _game->module()->area()->find(tag);
}

Variable Routines::getArea(const VariablesList &args, ExecutionContext &ctx) {
    return _game->module()->area();
}

Variable Routines::getItemInSlot(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto creature = getCreatureOrCaller(args, 1, ctx);
    if (creature) {
        InventorySlot slot = static_cast<InventorySlot>(getInt(args, 0));
        result.object = creature->getEquippedItem(slot);
    } else {
        warn("Routines: getItemInSlot: creature is invalid");
    }

    return move(result);
}

Variable Routines::setLocked(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getDoor(args, 0);
    if (target) {
        bool locked = getBool(args, 1);
        target->setLocked(locked);
    } else {
        warn("Routines: setLocked: target is invalid");
    }
    return Variable();
}

Variable Routines::getLocked(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getDoor(args, 0);
    if (!target) {
        warn("Routines: getLocked: target is invalid");
        return 0;
    }
    return target->isLocked() ? 1 : 0;
}

Variable Routines::createItemOnObject(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    if (target) {
        string itemTemplate(getString(args, 0));
        boost::to_lower(itemTemplate);

        if (!itemTemplate.empty()) {
            int count = getInt(args, 2);
            result.object = target->addItem(itemTemplate);
        } else {
            warn("Routines: createItemOnObject: itemTemplate is invalid");
        }
    } else {
        warn("Routines: createItemOnObject: target is invalid");
    }

    return move(result);
}

Variable Routines::getModule(const VariablesList &args, ExecutionContext &ctx) {
    return _game->module();
}

Variable Routines::getTag(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (!object) {
        warn("Routines: getTag: object is invalid");
        return "";
    }
    return object->tag();
}

Variable Routines::getDistanceToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    if (!caller) {
        warn("Routines: getDistanceToObject: caller is invalid");
        return -1.0f;
    }
    auto object = getSpatialObject(args, 0);
    if (!object) {
        warn("Routines: getDistanceToObject: object is invalid");
        return -1.0f;
    }

    return caller->distanceTo(*object);
}

Variable Routines::getDistanceToObject2D(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    if (!caller) {
        warn("Routines: getDistanceToObject2D: caller is invalid");
        return -1.0f;
    }
    auto object = getSpatialObject(args, 0);
    if (!object) {
        warn("Routines: getDistanceToObject2D: object is invalid");
        return -1.0f;
    }

    return caller->distanceTo(glm::vec2(object->position()));
}

Variable Routines::getExitingObject(const VariablesList &args, ExecutionContext &ctx) {
    return getTriggerrer(ctx);
}

Variable Routines::getFacing(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getSpatialObject(args, 0);
    if (!target) {
        warn("Routines: getFacing: target is invalid");
        return -1.0f;
    }
    return glm::degrees(target->heading());
}

Variable Routines::getPosition(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getSpatialObject(args, 0);
    if (!target) {
        warn("Routines: getPosition: target is invalid");
        return glm::vec3(0.0f);
    }
    return target->position();
}

Variable Routines::soundObjectPlay(const VariablesList &args, ExecutionContext &ctx) {
    auto sound = getSound(args, 0);
    if (sound) {
        sound->play();
    } else {
        warn("Routines: soundObjectPlay: sound is invalid");
    }
    return Variable();
}

Variable Routines::soundObjectStop(const VariablesList &args, ExecutionContext &ctx) {
    auto sound = getSound(args, 0);
    if (sound) {
        sound->stop();
    } else {
        warn("Routines: soundObjectStop: sound is invalid");
    }
    return Variable();
}

Variable Routines::getDistanceBetween(const VariablesList &args, ExecutionContext &ctx) {
    auto objectA = getSpatialObject(args, 0);
    if (!objectA) {
        warn("Routines: getDistanceBetween: objectA is invalid");
        return -1.0f;
    }
    auto objectB = getSpatialObject(args, 1);
    if (!objectB) {
        warn("Routines: getDistanceBetween: objectB is invalid");
        return -1.0f;
    }

    return objectA->distanceTo(*objectB);
}

Variable Routines::getFirstItemInInventory(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    if (target) {
        auto item = target->getFirstItem();
        if (item) {
            result.object = move(item);
        }
    } else {
        warn("Routines: getFirstItemInInventory: target is invalid");
    }

    return move(result);
}

Variable Routines::getNextItemInInventory(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    if (target) {
        auto item = target->getNextItem();
        if (item) {
            result.object = move(item);
        }
    } else {
        warn("Routines: getNextItemInInventory: target is invalid");
    }

    return move(result);
}

} // namespace game

} // namespace reone
