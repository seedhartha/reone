/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../common/random.h"

#include "../game.h"
#include "../script/util.h"

using namespace std;

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::d2(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return result;
}

Variable Routines::d3(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return result;
}

Variable Routines::d4(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return result;
}

Variable Routines::d6(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return result;
}

Variable Routines::d8(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return result;
}

Variable Routines::d10(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return result;
}

Variable Routines::d12(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return result;
}

Variable Routines::d20(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return result;
}

Variable Routines::d100(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return result;
}

Variable Routines::feetToMeters(const vector<Variable> &args, ExecutionContext &ctx) {
    return args[0].floatValue * 0.3048f;
}

Variable Routines::floatToInt(const vector<Variable> &args, ExecutionContext &ctx) {
    return static_cast<int>(args[0].floatValue);
}

Variable Routines::floatToString(const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: handle optional arguments
    return to_string(args[0].floatValue);
}

Variable Routines::hoursToSeconds(const vector<Variable> &args, ExecutionContext &ctx) {
    return args[0].intValue * 3600;
}

Variable Routines::intToFloat(const vector<Variable> &args, ExecutionContext &ctx) {
    return static_cast<float>(args[0].intValue);
}

Variable Routines::intToHexString(const vector<Variable> &args, ExecutionContext &ctx) {
    return str(boost::format("%08x") % args[0].intValue);
}

Variable Routines::intToString(const vector<Variable> &args, ExecutionContext &ctx) {
    return to_string(args[0].intValue);
}

Variable Routines::roundsToSeconds(const vector<Variable> &args, ExecutionContext &ctx) {
    return args[0].intValue / 6.0f;
}

Variable Routines::stringToFloat(const vector<Variable> &args, ExecutionContext &ctx) {
    return stof(args[0].strValue);
}

Variable Routines::stringToInt(const vector<Variable> &args, ExecutionContext &ctx) {
    return stoi(args[0].strValue);
}

Variable Routines::turnsToSeconds(const vector<Variable> &args, ExecutionContext &ctx) {
    return args[0].intValue / 60.0f;
}

Variable Routines::yardsToMeters(const vector<Variable> &args, ExecutionContext &ctx) {
    return args[0].floatValue * 0.9144f;
}

Variable Routines::getStringLength(const vector<Variable> &args, ExecutionContext &ctx) {
    return static_cast<int>(args[0].strValue.length());
}

Variable Routines::getStringUpperCase(const vector<Variable> &args, ExecutionContext &ctx) {
    string result(args[0].strValue);
    boost::to_upper(result);
    return move(result);
}

Variable Routines::getStringLowerCase(const vector<Variable> &args, ExecutionContext &ctx) {
    string result(args[0].strValue);
    boost::to_lower(result);
    return move(result);
}

Variable Routines::getStringRight(const vector<Variable> &args, ExecutionContext &ctx) {
    string str(args[0].strValue);
    int count = args[1].intValue;
    return str.substr(str.length() - count, count);
}

Variable Routines::getStringLeft(const vector<Variable> &args, ExecutionContext &ctx) {
    string str(args[0].strValue);
    int count = args[1].intValue;
    return str.substr(0, count);
}

Variable Routines::insertString(const vector<Variable> &args, ExecutionContext &ctx) {
    string dest(args[0].strValue);
    string str(args[1].strValue);
    int pos = args[2].intValue;
    return dest.insert(pos, str);
}

Variable Routines::getSubString(const vector<Variable> &args, ExecutionContext &ctx) {
    string str(args[0].strValue);
    int start = args[1].intValue;
    int count = args[2].intValue;
    return str.substr(start, count);
}

Variable Routines::findSubString(const vector<Variable> &args, ExecutionContext &ctx) {
    string str(args[0].strValue);
    string substr(args[1].strValue);
    size_t pos = str.find(substr);
    return pos != string::npos ? static_cast<int>(pos) : -1;
}

Variable Routines::shipBuild(const vector<Variable> &args, ExecutionContext &ctx) {
    return 1;
}

Variable Routines::executeScript(const vector<Variable> &args, ExecutionContext &ctx) {
    string script(args[0].strValue);
    int targetId = args[1].objectId;
    int scriptVar = args.size() >= 3 ? args[2].intValue : -1;

    shared_ptr<Object> target(getObjectById(targetId, ctx));
    if (target) {
        _game->setRunScriptVar(scriptVar);
        runScript(script, targetId, kObjectInvalid, -1);
    }

    return Variable();
}

Variable Routines::getRunScriptVar(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getRunScriptVar();
}

Variable Routines::random(const vector<Variable> &args, ExecutionContext &ctx) {
    return reone::random(0, args[0].intValue - 1);
}

Variable Routines::getLoadFromSaveGame(const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable(_game->isLoadFromSaveGame() ? 1 : 0);
}

} // namespace game

} // namespace reone
