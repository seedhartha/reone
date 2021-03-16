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

#include "routines.h"

#include <boost/algorithm/string.hpp>

#include "../../common/log.h"

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getItemInSlot(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto creature = getCreatureOrCaller(args, 1, ctx);
    if (creature) {
        int slot = getInt(args, 0);
        result.object = creature->getEquippedItem(slot);
    } else {
        warn("Routines: getItemInSlot: creature is invalid");
    }

    return move(result);
}

Variable Routines::createItemOnObject(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Object;

    auto target = getSpatialObjectOrCaller(args, 1, ctx);
    if (target) {
        string itemTemplate(getString(args, 0));
        boost::to_lower(itemTemplate);

        if (!itemTemplate.empty()) {
            int stackSize = getInt(args, 2, 1);
            result.object = target->addItem(itemTemplate, stackSize, true);
        } else {
            warn("Routines: createItemOnObject: itemTemplate is invalid");
        }
    } else {
        warn("Routines: createItemOnObject: target is invalid");
    }

    return move(result);
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

Variable Routines::getItemStackSize(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0);
    if (!item) {
        warn("Routines: getItemStackSize: item is invalid");
        return 0;
    }
    return item->stackSize();
}

Variable Routines::setItemStackSize(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0);
    if (item) {
        int stackSize = getInt(args, 1);
        item->setStackSize(stackSize);
    } else {
        warn("Routines: setItemStackSize: item is invalid");
    }
    return Variable();
}

Variable Routines::getIdentified(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0);
    if (!item) {
        warn("Routines: getIdentified: item is invalid");
        return 0;
    }
    return item->isIdentified() ? 1 : 0;
}

Variable Routines::setIdentified(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0);
    if (item) {
        bool identified = getBool(args, 1);
        item->setIdentified(identified);
    } else {
        warn("Routines: setIdentified: item is invalid");
    }
    return Variable();
}

Variable Routines::getItemPossessedBy(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (!creature) return shared_ptr<ScriptObject>();

    auto itemTag = boost::to_lower_copy(getString(args, 1));

    return static_pointer_cast<ScriptObject>(creature->getItemByTag(itemTag));
}

} // namespace game

} // namespace reone
