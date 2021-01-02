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

#include "routines.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../common/log.h"
#include "../../common/random.h"
#include "../../resource/resources.h"

#include "../game.h"

using namespace std;

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::d2(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return result;
}

Variable Routines::d3(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return result;
}

Variable Routines::d4(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return result;
}

Variable Routines::d6(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return result;
}

Variable Routines::d8(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return result;
}

Variable Routines::d10(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return result;
}

Variable Routines::d12(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return result;
}

Variable Routines::d20(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return result;
}

Variable Routines::d100(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = getInt(args, 0, 1);
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return result;
}

Variable Routines::feetToMeters(const VariablesList &args, ExecutionContext &ctx) {
    return getFloat(args, 0) * 0.3048f;
}

Variable Routines::floatToInt(const VariablesList &args, ExecutionContext &ctx) {
    return static_cast<int>(getFloat(args, 0));
}

Variable Routines::floatToString(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle optional arguments
    return to_string(getFloat(args, 0));
}

Variable Routines::hoursToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return getInt(args, 0) * 3600;
}

Variable Routines::intToFloat(const VariablesList &args, ExecutionContext &ctx) {
    return static_cast<float>(getInt(args, 0));
}

Variable Routines::intToHexString(const VariablesList &args, ExecutionContext &ctx) {
    return str(boost::format("%08x") % getInt(args, 0));
}

Variable Routines::intToString(const VariablesList &args, ExecutionContext &ctx) {
    return to_string(getInt(args, 0));
}

Variable Routines::roundsToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return getInt(args, 0) / 6.0f;
}

Variable Routines::stringToFloat(const VariablesList &args, ExecutionContext &ctx) {
    return stof(getString(args, 0));
}

Variable Routines::stringToInt(const VariablesList &args, ExecutionContext &ctx) {
    return stoi(getString(args, 0));
}

Variable Routines::turnsToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return getInt(args, 0) / 60.0f;
}

Variable Routines::yardsToMeters(const VariablesList &args, ExecutionContext &ctx) {
    return getFloat(args, 0) * 0.9144f;
}

Variable Routines::getStringLength(const VariablesList &args, ExecutionContext &ctx) {
    return static_cast<int>(getString(args, 0).length());
}

Variable Routines::getStringUpperCase(const VariablesList &args, ExecutionContext &ctx) {
    string result(getString(args, 0));
    boost::to_upper(result);
    return move(result);
}

Variable Routines::getStringLowerCase(const VariablesList &args, ExecutionContext &ctx) {
    string result(getString(args, 0));
    boost::to_lower(result);
    return move(result);
}

Variable Routines::getStringRight(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    int count = getInt(args, 1);
    return str.substr(str.length() - count, count);
}

Variable Routines::getStringLeft(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    int count = getInt(args, 1);
    return str.substr(0, count);
}

Variable Routines::insertString(const VariablesList &args, ExecutionContext &ctx) {
    string dest(getString(args, 0));
    string str(getString(args, 1));
    int pos = getInt(args, 2);
    return dest.insert(pos, str);
}

Variable Routines::getSubString(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    int start = getInt(args, 1);
    int count = getInt(args, 2);
    return str.substr(start, count);
}

Variable Routines::findSubString(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    string substr(getString(args, 1));
    size_t pos = str.find(substr);
    return pos != string::npos ? static_cast<int>(pos) : -1;
}

Variable Routines::shipBuild(const VariablesList &args, ExecutionContext &ctx) {
    return 1;
}

Variable Routines::executeScript(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getObject(args, 1);
    if (target) {
        string script(getString(args, 0));
        int scriptVar = getInt(args, 2, -1);

        _game->setRunScriptVar(scriptVar);
        _game->scriptRunner().run(script, target->id());
    } else {
        warn("Routines: executeScript: target is invalid");
    }

    return Variable();
}

Variable Routines::getRunScriptVar(const VariablesList &args, ExecutionContext &ctx) {
    return _game->getRunScriptVar();
}

Variable Routines::random(const VariablesList &args, ExecutionContext &ctx) {
    return reone::random(0, getInt(args, 0) - 1);
}

Variable Routines::getLoadFromSaveGame(const VariablesList &args, ExecutionContext &ctx) {
    return Variable(_game->isLoadFromSaveGame() ? 1 : 0);
}

Variable Routines::getStringByStrRef(const VariablesList &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    return Variable(Resources::instance().getString(strRef));
}

Variable Routines::startNewModule(const VariablesList &args, ExecutionContext &ctx) {
    string moduleName(getString(args, 0));
    boost::to_lower(moduleName);

    string waypoint(getString(args, 1, ""));
    boost::to_lower(waypoint);

    _game->scheduleModuleTransition(moduleName, waypoint);

    return Variable();
}

Variable Routines::playMovie(const VariablesList &args, ExecutionContext &ctx) {
    string movie(getString(args, 0));
    boost::to_lower(movie);

    _game->playVideo(movie);

    return Variable();
}

Variable Routines::printString(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    info(str);
    return Variable();
}

Variable Routines::printFloat(const VariablesList &args, ExecutionContext &ctx) {
    float value(getFloat(args, 0));
    info(to_string(value));
    return Variable();
}

Variable Routines::printInteger(const VariablesList &args, ExecutionContext &ctx) {
    int value(getInt(args, 0));
    info(to_string(value));
    return Variable();
}

Variable Routines::printObject(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    info(to_string(object->id()));
    return Variable();
}

Variable Routines::printVector(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 value(getVector(args, 0));
    info(boost::format("%f %f %f") % value.x % value.y % value.z);
    return Variable();
}

} // namespace game

} // namespace reone
