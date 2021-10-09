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
 *  Implementation of stealth-related routines.
 */

#include "declarations.h"

#include "../../../../script/exception/notimpl.h"
#include "../../../../script/types.h"

#include "../../../core/game.h"
#include "../../../core/script/routine/context.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable getMaxStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->area()->maxStealthXP();
    return Variable::ofInt(result);
}

Variable setMaxStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    int max = getInt(args, 0);
    ctx.game.module()->area()->setMaxStealthXP(max);

    return Variable::ofNull();
}

Variable getCurrentStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->area()->currentStealthXP();
    return Variable::ofInt(result);
}

Variable setCurrentStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    int current = getInt(args, 0);
    ctx.game.module()->area()->setCurrentStealthXP(current);

    return Variable::ofNull();
}

Variable awardStealthXP(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getStealthXPEnabled(const vector<Variable> &args, const RoutineContext &ctx) {
    bool result = ctx.game.module()->area()->isStealthXPEnabled();
    return Variable::ofInt(static_cast<int>(result));
}

Variable setStealthXPEnabled(const vector<Variable> &args, const RoutineContext &ctx) {
    bool enabled = getBool(args, 0);
    ctx.game.module()->area()->setStealthXPEnabled(enabled);

    return Variable::ofNull();
}

Variable getStealthXPDecrement(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->area()->stealthXPDecrement();
    return Variable::ofInt(result);
}

Variable setStealthXPDecrement(const vector<Variable> &args, const RoutineContext &ctx) {
    int decrement = getInt(args, 0);
    ctx.game.module()->area()->setStealthXPDecrement(decrement);

    return Variable::ofNull();
}

} // namespace routine

} // namespace game

} // namespace reone
