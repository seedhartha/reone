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
 *  Implementation of routines related to string manipulation.
 */

#include "routines.h"

#include "../../common/log.h"
#include "../../resource/strings.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getStringLength(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(getString(args, 0).length()));
}

Variable Routines::getStringUpperCase(const VariablesList &args, ExecutionContext &ctx) {
    string result(boost::to_upper_copy(getString(args, 0)));
    return Variable::ofString(move(result));
}

Variable Routines::getStringLowerCase(const VariablesList &args, ExecutionContext &ctx) {
    string result(boost::to_lower_copy(getString(args, 0)));
    return Variable::ofString(move(result));
}

Variable Routines::getStringRight(const VariablesList &args, ExecutionContext &ctx) {
    string result;
    string str(getString(args, 0));
    int count = getInt(args, 1);

    if (str.size() >= count) {
        result = str.substr(str.length() - count, count);
    }

    return Variable::ofString(move(result));
}

Variable Routines::getStringLeft(const VariablesList &args, ExecutionContext &ctx) {
    string result;
    string str(getString(args, 0));
    int count = getInt(args, 1);

    if (str.size() >= count) {
        result = str.substr(0, count);
    }

    return Variable::ofString(move(result));
}

Variable Routines::insertString(const VariablesList &args, ExecutionContext &ctx) {
    string dest(getString(args, 0));
    string str(getString(args, 1));
    int pos = getInt(args, 2);
    return Variable::ofString(dest.insert(pos, str));
}

Variable Routines::getSubString(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    int start = getInt(args, 1);
    int count = getInt(args, 2);
    return Variable::ofString(str.substr(start, count));
}

Variable Routines::findSubString(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    string substr(getString(args, 1));
    size_t pos = str.find(substr);
    return Variable::ofInt(pos != string::npos ? static_cast<int>(pos) : -1);
}

} // namespace game

} // namespace reone
