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

#include "routines.h"

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getMaxStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game.module()->area()->maxStealthXP());
}

Variable Routines::setMaxStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    int max = getInt(args, 0);
    _game.module()->area()->setMaxStealthXP(max);
    return Variable();
}

Variable Routines::getCurrentStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game.module()->area()->currentStealthXP());
}

Variable Routines::setCurrentStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    int current = getInt(args, 0);
    _game.module()->area()->setCurrentStealthXP(current);
    return Variable();
}

Variable Routines::getStealthXPEnabled(const VariablesList &args, ExecutionContext &ctx) {
    bool result = _game.module()->area()->isStealthXPEnabled();
    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setStealthXPEnabled(const VariablesList &args, ExecutionContext &ctx) {
    bool enabled = getBool(args, 0);
    _game.module()->area()->setStealthXPEnabled(enabled);
    return Variable();
}

Variable Routines::getStealthXPDecrement(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game.module()->area()->stealthXPDecrement());
}

Variable Routines::setStealthXPDecrement(const VariablesList &args, ExecutionContext &ctx) {
    int decrement = getInt(args, 0);
    _game.module()->area()->setStealthXPDecrement(decrement);
    return Variable();
}

} // namespace game

} // namespace reone
