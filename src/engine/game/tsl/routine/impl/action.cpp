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
 *  Implementation of action-related routines.
 */

#include "declarations.h"

#include "../../../../script/exception/argument.h"

#include "../../../core/action/factory.h"
#include "../../../core/game.h"
#include "../../../core/object/creature.h"
#include "../../../core/script/routine/argutil.h"
#include "../../../core/script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable actionFollowOwner(const vector<Variable> &args, const RoutineContext &ctx) {
    float range = getFloatOrElse(args, 0, 2.5f);

    auto action = ctx.actionFactory.newFollowOwner();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSwitchWeapons(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newSwitchWeapons();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

} // namespace routine

} // namespace game

} // namespace reone
