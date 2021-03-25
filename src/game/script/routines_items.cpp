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
 *  Implementation of routines related to Item objects.
 */

#include "routines.h"

#include <boost/algorithm/string.hpp>

#include "../../common/log.h"

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getItemInSlot(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> result;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    int slot = getInt(args, 0);

    if (creature) {
        result = creature->getEquippedItem(slot);
    } else {
        debug("Script: getItemInSlot: creature is invalid");
    }

    return Variable::ofObject(move(result));
}

Variable Routines::createItemOnObject(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> result;
    string itemTemplate(boost::to_lower_copy(getString(args, 0)));
    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    int stackSize = getInt(args, 2, 1);

    if (!itemTemplate.empty() && target) {
        result = target->addItem(itemTemplate, stackSize, true);
    } else if (itemTemplate.empty()) {
        debug("Script: createItemOnObject: itemTemplate is invalid");
    } else if (!target) {
        debug("Script: createItemOnObject: target is invalid");
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getFirstItemInInventory(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> result;

    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    if (target) {
        result = target->getFirstItem();
    } else {
        debug("Script: getFirstItemInInventory: target is invalid");
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getNextItemInInventory(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> result;

    auto target = getSpatialObjectOrCaller(args, 0, ctx);
    if (target) {
        result = target->getNextItem();
    } else {
        debug("Script: getNextItemInInventory: target is invalid");
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getItemStackSize(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto item = getItem(args, 0);
    if (item) {
        result = item->stackSize();
    } else {
        debug("Script: getItemStackSize: item is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::setItemStackSize(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0);
    int stackSize = getInt(args, 1);

    if (item) {
        item->setStackSize(stackSize);
    } else {
        debug("Script: setItemStackSize: item is invalid");
    }

    return Variable();
}

Variable Routines::getIdentified(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto item = getItem(args, 0);
    if (item) {
        result = item->isIdentified();
    } else {
        debug("Script: getIdentified: item is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setIdentified(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0);
    bool identified = getBool(args, 1);

    if (item) {
        item->setIdentified(identified);
    } else {
        debug("Script: setIdentified: item is invalid");
    }

    return Variable();
}

Variable Routines::getItemPossessedBy(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Item> result;
    auto creature = getCreature(args, 0);
    auto itemTag = boost::to_lower_copy(getString(args, 1));

    if (creature && !itemTag.empty()) {
        result = creature->getItemByTag(itemTag);
    } else if (!creature) {
        debug("Script: getItemPossessedBy: creature is invalid");
    } else if (itemTag.empty()) {
        debug("Script: getItemPossessedBy: itemTag is invalid");
    }

    return Variable::ofObject(move(result));
}

Variable Routines::getBaseItemType(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto item = getItem(args, 0);
    if (item) {
        result = item->baseItemType();
    } else {
        debug("Script: getBaseItemType: item is invalid");
    }

    return Variable::ofInt(result);
}

} // namespace game

} // namespace reone
