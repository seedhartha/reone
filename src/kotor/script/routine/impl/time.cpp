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
 *  Implementation of time-related routines.
 */

#include "../declarations.h"

#include "../../../../game/game.h"
#include "../../../../script/types.h"

#include "../argutil.h"
#include "../context.h"
#include "../objectutil.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

Variable getTimeHour(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().hour;
    return Variable::ofInt(result);
}

Variable getTimeMinute(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().minute;
    return Variable::ofInt(result);
}

Variable getTimeSecond(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().second;
    return Variable::ofInt(result);
}

Variable getTimeMillisecond(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().millisecond;
    return Variable::ofInt(result);
}

} // namespace routine

} // namespace kotor

} // namespace reone
