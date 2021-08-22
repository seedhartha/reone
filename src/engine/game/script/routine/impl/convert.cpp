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
    float value = getFloat(args, 0);
    int width = getIntOrElse(args, 1, 18);
    int decimals = getIntOrElse(args, 2, 9);

    // TODO: handle optional arguments

    return Variable::ofString(to_string(value));
}

Variable intToString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int value = getInt(args, 0);
    return Variable::ofString(to_string(value));
}

Variable roundsToSeconds(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int rounds = getInt(args, 0);
    return Variable::ofFloat(rounds / 6.0f);
}

Variable hoursToSeconds(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int hours = getInt(args, 0);
    return Variable::ofInt(hours * 3600);
}

Variable turnsToSeconds(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int turns = getInt(args, 0);
    return Variable::ofFloat(turns / 60.0f);
}

Variable feetToMeters(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float feet = getFloat(args, 0);
    return Variable::ofFloat(feet * 0.3048f);
}

Variable yardsToMeters(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float yards = getFloat(args, 0);
    return Variable::ofFloat(yards * 0.9144f);
}

Variable intToFloat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int value = getInt(args, 0);
    return Variable::ofFloat(static_cast<float>(value));
}

Variable floatToInt(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofInt(static_cast<int>(value));
}

Variable stringToInt(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string number(getString(args, 0));

    int result = 0;
    if (!number.empty()) {
        result = stoi(number);
    }

    return Variable::ofInt(result);
}

Variable stringToFloat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string number(getString(args, 0));

    float result = 0;
    if (!number.empty()) {
        result = stof(number);
    }

    return Variable::ofFloat(result);
}

Variable intToHexString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int value = getInt(args, 0);
    return Variable::ofString(str(boost::format("%08x") % value));
}

} // namespace routine

} // namespace game

} // namespace reone
