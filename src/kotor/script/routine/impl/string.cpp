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

#include "../declarations.h"

#include "../../../../game/script/routine/argutil.h"
#include "../../../../game/script/routine/context.h"
#include "../../../../game/script/routine/objectutil.h"
#include "../../../../script/exception/notimpl.h"
#include "../../../../script/types.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

Variable getStringLength(const vector<Variable> &args, const RoutineContext &ctx) {
    string str(getString(args, 0));
    int result = str.length();

    return Variable::ofInt(static_cast<int>(result));
}

Variable getStringUpperCase(const vector<Variable> &args, const RoutineContext &ctx) {
    string result(boost::to_upper_copy(getString(args, 0)));
    return Variable::ofString(move(result));
}

Variable getStringLowerCase(const vector<Variable> &args, const RoutineContext &ctx) {
    string result(boost::to_lower_copy(getString(args, 0)));
    return Variable::ofString(move(result));
}

Variable getStringRight(const vector<Variable> &args, const RoutineContext &ctx) {
    string str(getString(args, 0));
    int count = getInt(args, 1);

    string result;
    if (str.size() >= count) {
        result = str.substr(str.length() - count, count);
    }

    return Variable::ofString(move(result));
}

Variable getStringLeft(const vector<Variable> &args, const RoutineContext &ctx) {
    string str(getString(args, 0));
    int count = getInt(args, 1);

    string result;
    if (str.size() >= count) {
        result = str.substr(0, count);
    }

    return Variable::ofString(move(result));
}

Variable insertString(const vector<Variable> &args, const RoutineContext &ctx) {
    string dest(getString(args, 0));
    string str(getString(args, 1));
    int pos = getInt(args, 2);

    return Variable::ofString(dest.insert(pos, str));
}

Variable getSubString(const vector<Variable> &args, const RoutineContext &ctx) {
    string str(getString(args, 0));
    int start = getInt(args, 1);
    int count = getInt(args, 2);

    return Variable::ofString(str.substr(start, count));
}

Variable findSubString(const vector<Variable> &args, const RoutineContext &ctx) {
    string str(getString(args, 0));
    string substr(getString(args, 1));
    size_t pos = str.find(substr);

    return Variable::ofInt(pos != string::npos ? static_cast<int>(pos) : -1);
}

Variable testStringAgainstPattern(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getMatchedSubstring(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getMatchedSubstringsCount(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace kotor

} // namespace reone
