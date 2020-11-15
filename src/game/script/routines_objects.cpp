/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

Variable Routines::destroyObject(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (object) {
        shared_ptr<SpatialObject> spatial(dynamic_pointer_cast<SpatialObject>(object));
        if (spatial) {
            _game->module()->area()->destroyObject(*spatial);
        }
    } else {
        warn("Routine: object not found by id: " + to_string(objectId));
    }

    return Variable();
}

Variable Routines::getEnteringObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.triggererId;
    return move(result);
}

Variable Routines::getIsObjectValid(const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable(args[0].objectId != kObjectInvalid);
}

Variable Routines::getObjectByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);

    string tag(args[0].strValue);
    if (tag.empty()) {
        result.objectId = _game->party().player()->id();
    } else {
        int nth = args.size() >= 2 ? args[1].intValue : 0;
        shared_ptr<Object> object(_game->module()->area()->find(tag, nth));
        result.objectId = object ? object->id() : kObjectInvalid;
    }

    return move(result);
}

Variable Routines::getWaypointByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(_game->module()->area()->find(args[0].strValue));

    Variable result(VariableType::Object);
    result.objectId = object ? object->id() : kObjectInvalid;

    return move(result);
}

Variable Routines::getArea(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = _game->module()->area()->id();

    return move(result);
}

Variable Routines::getItemInSlot(const vector<Variable> &args, ExecutionContext &ctx) {
    uint32_t objectId(args.size() > 1 ? args[1].objectId : kObjectSelf);
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    shared_ptr<Object> item;

    if (object) {
        Creature &creature = static_cast<Creature &>(*object);
        item = creature.getEquippedItem(static_cast<InventorySlot>(args[0].intValue));
    }
    Variable result(VariableType::Object);
    result.objectId = item ? item->id() : kObjectInvalid;

    return move(result);
}

Variable Routines::setLocked(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    bool locked = args[1].intValue != 0;

    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (object) {
        Door *door = dynamic_cast<Door *>(object.get());
        if (door) {
            door->setLocked(locked);
        } else {
            warn("Routine: object is not a door: " + to_string(objectId));
        }
    }

    return Variable();
}

Variable Routines::getLocked(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);

    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (object) {
        Door *door = dynamic_cast<Door *>(object.get());
        if (door) {
            result.intValue = door->isLocked() ? 1 : 0;
        } else {
            warn("Routine: object is not a door: " + to_string(objectId));
        }
    }

    return move(result);
}

Variable Routines::createItemOnObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = kObjectInvalid;

    string itemBlueprint(args[0].strValue);
    if (!itemBlueprint.empty()) {
        boost::to_lower(itemBlueprint);

        int targetId = args.size() >= 2 ? args[1].objectId : kObjectSelf;
        int count = args.size() >= 3 ? args[2].intValue : 1;

        shared_ptr<Object> target(getObjectById(targetId, ctx));
        if (target) {
            shared_ptr<ItemBlueprint> blueprint(Blueprints::instance().getItem(itemBlueprint));
            shared_ptr<Item> item(_game->objectFactory().newItem());
            item->load(blueprint);

            shared_ptr<SpatialObject> spatialTarget(dynamic_pointer_cast<SpatialObject>(target));
            spatialTarget->addItem(item);

            result.objectId = item->id();

        } else {
            warn("Routine: object not found: " + to_string(targetId));
        }
    }

    return move(result);
}

} // namespace game

} // namespace reone
