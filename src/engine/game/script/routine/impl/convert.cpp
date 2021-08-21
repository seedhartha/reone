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
 *  Implementation of conversion-related routines.
 */

#include "../../routines.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::floatToString(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle optional arguments
    return Variable::ofString(to_string(getFloat(args, 0)));
}

Variable Routines::intToString(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofString(to_string(getInt(args, 0)));
}

Variable Routines::roundsToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getInt(args, 0) / 6.0f);
}

Variable Routines::hoursToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(getInt(args, 0) * 3600);
}

Variable Routines::turnsToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getInt(args, 0) / 60.0f);
}

Variable Routines::feetToMeters(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getFloat(args, 0) * 0.3048f);
}

Variable Routines::yardsToMeters(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getFloat(args, 0) * 0.9144f);
}

Variable Routines::intToFloat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(static_cast<float>(getInt(args, 0)));
}

Variable Routines::floatToInt(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(getFloat(args, 0)));
}

Variable Routines::stringToInt(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    string number(getString(args, 0));

    if (!number.empty()) {
        result = stoi(number);
    }

    return Variable::ofInt(result);
}

Variable Routines::stringToFloat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(stof(getString(args, 0)));
}

Variable Routines::intToHexString(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofString(str(boost::format("%08x") % getInt(args, 0)));
}

} // namespace game

} // namespace reone
