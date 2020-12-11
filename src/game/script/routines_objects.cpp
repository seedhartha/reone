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

Variable Routines::destroyObject(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<SpatialObject> spatial(dynamic_pointer_cast<SpatialObject>(object));
        if (spatial) {
            _game->module()->area()->destroyObject(*spatial);
        } else {
            warn("Routines: destroyObject: not a spatial object: " + to_string(object->id()));
        }
    }
    return Variable();
}

Variable Routines::getEnteringObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.triggererId;
    return move(result);
}

Variable Routines::getIsObjectValid(const vector<Variable> &args, ExecutionContext &ctx) {
    return args[0].objectId != kObjectInvalid;
}

Variable Routines::getObjectByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);

    string tag(args[0].strValue);
    boost::to_lower(tag);

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
    string tag(args[0].strValue);
    boost::to_lower(tag);

    shared_ptr<Object> object(_game->module()->area()->find(tag));

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
    shared_ptr<Object> item;

    InventorySlot slot = static_cast<InventorySlot>(args[0].intValue);
    shared_ptr<Object> object(getObjectById(args.size() >= 2 ? args[1].objectId : kObjectSelf, ctx));

    if (object) {
        shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
        if (creature) {
            item = creature->getEquippedItem(slot);
        } else {
            warn("Routines: getItemInSlot: not a creature: " + to_string(object->id()));
        }
    }

    Variable result(VariableType::Object);
    result.objectId = item ? item->id() : kObjectInvalid;

    return move(result);
}

Variable Routines::setLocked(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    bool locked = args[1].intValue != 0;

    if (object) {
        shared_ptr<Door> door(dynamic_pointer_cast<Door>(object));
        if (door) {
            door->setLocked(locked);
        } else {
            warn("Routines: setLocked: not a door: " + to_string(object->id()));
        }
    }

    return Variable();
}

Variable Routines::getLocked(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);

    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<Door> door(dynamic_pointer_cast<Door>(object));
        if (door) {
            result.intValue = door->isLocked() ? 1 : 0;
        } else {
            warn("Routines: getLocked: not a door: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::createItemOnObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = kObjectInvalid;

    string itemBlueprint(args[0].strValue);
    boost::to_lower(itemBlueprint);

    shared_ptr<Object> target(getObjectById(args.size() >= 2 ? args[1].objectId : kObjectSelf, ctx));
    int count = args.size() >= 3 ? args[2].intValue : 1;

    if (target) {
        if (!itemBlueprint.empty()) {
            shared_ptr<ItemBlueprint> blueprint(Blueprints::instance().getItem(itemBlueprint));
            if (blueprint) {
                shared_ptr<Item> item(_game->objectFactory().newItem());
                item->load(blueprint);

                shared_ptr<SpatialObject> spatialTarget(dynamic_pointer_cast<SpatialObject>(target));
                if (spatialTarget) {
                    spatialTarget->addItem(item);
                } else {
                    warn("Routines: createItemOnObject: not a spatial object: " + to_string(target->id()));
                }
                result.objectId = item->id();

            } else {
                warn("Routines: createItemOnObject: item blueprint not found: " + itemBlueprint);
            }
        } else {
            warn("Routines: createItemOnObject: item blueprint is empty");
        }
    }

    return move(result);
}

Variable Routines::getModule(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = _game->module()->id();
    return move(result);
}

Variable Routines::getTag(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    return object ? object->tag() : "";
}

Variable Routines::getDistanceToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(-1.0f);

    shared_ptr<Object> left(getObjectById(kObjectSelf, ctx));
    shared_ptr<Object> right(getObjectById(args[0].objectId, ctx));

    if (left && right) {
        shared_ptr<SpatialObject> spatialLeft(dynamic_pointer_cast<SpatialObject>(left));
        shared_ptr<SpatialObject> spatialRight(dynamic_pointer_cast<SpatialObject>(right));

        if (spatialLeft && spatialRight) {
            result.floatValue = spatialLeft->distanceTo(spatialRight->position());
        } else {
            warn("Routines: getDistanceToObject: objects are not spatial: " + to_string(left->id()) + " " + to_string(right->id()));
        }
    }

    return move(result);
}

Variable Routines::getDistanceToObject2D(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(-1.0f);

    shared_ptr<Object> left(getObjectById(kObjectSelf, ctx));
    shared_ptr<Object> right(getObjectById(args[0].objectId, ctx));

    if (left && right) {
        shared_ptr<SpatialObject> spatialLeft(dynamic_pointer_cast<SpatialObject>(left));
        shared_ptr<SpatialObject> spatialRight(dynamic_pointer_cast<SpatialObject>(right));

        if (spatialLeft && spatialRight) {
            result.floatValue = spatialLeft->distanceTo(glm::vec2(spatialRight->position()));
        } else {
            warn("Routines: getDistanceToObject2D: objects are not spatial: " + to_string(left->id()) + " " + to_string(right->id()));
        }
    }

    return move(result);
}

Variable Routines::getExitingObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.triggererId;
    return move(result);
}

Variable Routines::getFacing(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(-1.0f);

    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<SpatialObject> spatial(dynamic_pointer_cast<SpatialObject>(object));
        if (spatial) {
            result.floatValue = spatial->heading();
        } else {
            warn("Routines: getFacing: not a spatial object: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::getPosition(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(glm::vec3(0.0f));

    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<SpatialObject> spatial(dynamic_pointer_cast<SpatialObject>(object));
        if (spatial) {
            result.vecValue = spatial->position();
        } else {
            warn("Routines: getPosition: not a spatial object: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::soundObjectPlay(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<Sound> sound(dynamic_pointer_cast<Sound>(object));
        if (sound) {
            sound->play();
        } else {
            warn("Routines: soundObjectPlay: not a sound: " + to_string(object->id()));
        }
    }

    return Variable();
}

Variable Routines::soundObjectStop(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<Sound> sound(dynamic_pointer_cast<Sound>(object));
        if (sound) {
            sound->stop();
        } else {
            warn("Routines: soundObjectStop: not a sound: " + to_string(object->id()));
        }
    }

    return Variable();
}

Variable Routines::getDistanceBetween(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0.0f);
    auto left = getObjectById(args[0].objectId, ctx);
    auto right = getObjectById(args[1].objectId, ctx);

    if (left && right) {
        auto spatialLeft = dynamic_pointer_cast<SpatialObject>(left);
        auto spatialRight = dynamic_pointer_cast<SpatialObject>(right);

        if (spatialLeft && spatialRight) {
            result.floatValue = spatialLeft->distanceTo(*spatialRight);
        } else {
            warn(boost::format("Routines: getDistanceBetween: objects are not spatial: '%s', '%s'") % left->tag() % right->tag());
        }
    }

    return move(result);
}

Variable Routines::getFirstItemInInventory(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = kObjectInvalid;

    auto object = getObjectById(args.size() > 0 ? args[0].objectId : kObjectSelf, ctx);
    if (object) {
        auto spatial = dynamic_pointer_cast<SpatialObject>(object);
        if (spatial) {
            auto item = spatial->getFirstItem();
            if (item) {
                result.objectId = item->id();
            }
        } else {
            warn(boost::format("Routines: getFirstItemInInventory: object '%s' is not spatial") % object->tag());
        }
    }

    return move(result);
}

Variable Routines::getNextItemInInventory(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = kObjectInvalid;

    auto object = getObjectById(args.size() > 0 ? args[0].objectId : kObjectSelf, ctx);
    if (object) {
        auto spatial = dynamic_pointer_cast<SpatialObject>(object);
        if (spatial) {
            auto item = spatial->getNextItem();
            if (item) {
                result.objectId = item->id();
            }
        } else {
            warn(boost::format("Routines: getNextItemInInventory: object '%s' is not spatial") % object->tag());
        }
    }

    return move(result);
}

} // namespace game

} // namespace reone
