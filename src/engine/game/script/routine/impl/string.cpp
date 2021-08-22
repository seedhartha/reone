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
 *  Implementation of string-related routines.
 */

#include "declarations.h"

#include "../../../../script/exception/notimpl.h"
#include "../../../../script/types.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable getStringLength(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(getStringOrElse(args, 0).length()));
}

Variable getStringUpperCase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string result(boost::to_upper_copy(getStringOrElse(args, 0)));
    return Variable::ofString(move(result));
}

Variable getStringLowerCase(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string result(boost::to_lower_copy(getStringOrElse(args, 0)));
    return Variable::ofString(move(result));
}

Variable getStringRight(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string result;
    string str(getStringOrElse(args, 0));
    int count = getIntOrElse(args, 1);

    if (str.size() >= count) {
        result = str.substr(str.length() - count, count);
    }

    return Variable::ofString(move(result));
}

Variable getStringLeft(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string result;
    string str(getStringOrElse(args, 0));
    int count = getIntOrElse(args, 1);

    if (str.size() >= count) {
        result = str.substr(0, count);
    }

    return Variable::ofString(move(result));
}

Variable insertString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string dest(getStringOrElse(args, 0));
    string str(getStringOrElse(args, 1));
    int pos = getIntOrElse(args, 2);
    return Variable::ofString(dest.insert(pos, str));
}

Variable getSubString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string str(getStringOrElse(args, 0));
    int start = getIntOrElse(args, 1);
    int count = getIntOrElse(args, 2);
    return Variable::ofString(str.substr(start, count));
}

Variable findSubString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string str(getStringOrElse(args, 0));
    string substr(getStringOrElse(args, 1));
    size_t pos = str.find(substr);
    return Variable::ofInt(pos != string::npos ? static_cast<int>(pos) : -1);
}

Variable testStringAgainstPattern(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getMatchedSubstring(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getMatchedSubstringsCount(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
