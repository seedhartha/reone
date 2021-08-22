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

#include "declarations.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable floatToString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: handle optional arguments
    return Variable::ofString(to_string(getFloatOrElse(args, 0)));
}

Variable intToString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofString(to_string(getIntOrElse(args, 0)));
}

Variable roundsToSeconds(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getIntOrElse(args, 0) / 6.0f);
}

Variable hoursToSeconds(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(getIntOrElse(args, 0) * 3600);
}

Variable turnsToSeconds(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getIntOrElse(args, 0) / 60.0f);
}

Variable feetToMeters(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getFloatOrElse(args, 0) * 0.3048f);
}

Variable yardsToMeters(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getFloatOrElse(args, 0) * 0.9144f);
}

Variable intToFloat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofFloat(static_cast<float>(getIntOrElse(args, 0)));
}

Variable floatToInt(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(getFloatOrElse(args, 0)));
}

Variable stringToInt(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int result = 0;
    string number(getStringOrElse(args, 0));

    if (!number.empty()) {
        result = stoi(number);
    }

    return Variable::ofInt(result);
}

Variable stringToFloat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofFloat(stof(getStringOrElse(args, 0)));
}

Variable intToHexString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofString(str(boost::format("%08x") % getIntOrElse(args, 0)));
}

} // namespace routine

} // namespace game

} // namespace reone
