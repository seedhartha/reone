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

#include "declarations.h"

#include "../../../../script/types.h"

#include "../../../game.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable setTime(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int hour = getInt(args, 0);
    int minute = getInt(args, 1);
    int second = getInt(args, 2);
    int millisecond = getInt(args, 3);

    game.module()->setTime(hour, minute, second, millisecond);

    return Variable();
}

Variable getTimeHour(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(game.module()->time().hour);
}

Variable getTimeMinute(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(game.module()->time().minute);
}

Variable getTimeSecond(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(game.module()->time().second);
}

Variable getTimeMillisecond(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(game.module()->time().millisecond);
}

Variable getIsDay(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(game.module());
    const Module::Time &time = module->time();
    return Variable::ofInt(static_cast<int>(time.hour > module->info().dawnHour && time.hour < module->info().duskHour));
}

Variable getIsNight(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(game.module());
    const Module::Time &time = module->time();
    return Variable::ofInt(static_cast<int>(time.hour < module->info().dawnHour || time.hour > module->info().duskHour));
}

Variable getIsDawn(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(game.module());
    const Module::Time &time = module->time();
    return Variable::ofInt(static_cast<int>(time.hour == module->info().dawnHour));
}

Variable getIsDusk(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(game.module());
    const Module::Time &time = module->time();
    return Variable::ofInt(static_cast<int>(time.hour == module->info().duskHour));
}

} // namespace routine

} // namespace game

} // namespace reone
