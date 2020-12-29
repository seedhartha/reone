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

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getIsDawn(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(_game->module());
    const Module::Time &time = module->time();
    return time.hour == module->info().dawnHour ? 1 : 0;
}

Variable Routines::getIsDay(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(_game->module());
    const Module::Time &time = module->time();
    return time.hour > module->info().dawnHour && time.hour < module->info().duskHour ? 1 : 0;
}

Variable Routines::getIsDusk(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(_game->module());
    const Module::Time &time = module->time();
    return time.hour == module->info().duskHour ? 1 : 0;
}

Variable Routines::getIsNight(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Module> module(_game->module());
    const Module::Time &time = module->time();
    return time.hour < module->info().dawnHour || time.hour > module->info().duskHour ? 1 : 0;
}

Variable Routines::getTimeHour(const VariablesList &args, ExecutionContext &ctx) {
    return _game->module()->time().hour;
}

Variable Routines::getTimeMillisecond(const VariablesList &args, ExecutionContext &ctx) {
    return _game->module()->time().millisecond;
}

Variable Routines::getTimeMinute(const VariablesList &args, ExecutionContext &ctx) {
    return _game->module()->time().minute;
}

Variable Routines::getTimeSecond(const VariablesList &args, ExecutionContext &ctx) {
    return _game->module()->time().second;
}

Variable Routines::setTime(const VariablesList &args, ExecutionContext &ctx) {
    int hour = getInt(args, 0);
    int minute = getInt(args, 1);
    int second = getInt(args, 2);
    int millisecond = getInt(args, 3);

    _game->module()->setTime(hour, minute, second, millisecond);

    return Variable();
}

} // namespace game

} // namespace reone
