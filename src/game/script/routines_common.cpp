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
 *  Implementation of utility and miscellaneous routines.
 */

#include "routines.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../common/log.h"
#include "../../common/random.h"
#include "../../resource/strings.h"

#include "../game.h"

using namespace std;

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

static bool g_shipBuild = true;

Variable Routines::feetToMeters(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getFloat(args, 0) * 0.3048f);
}

Variable Routines::floatToInt(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(getFloat(args, 0)));
}

Variable Routines::floatToString(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle optional arguments
    return Variable::ofString(to_string(getFloat(args, 0)));
}

Variable Routines::hoursToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(getInt(args, 0) * 3600);
}

Variable Routines::intToFloat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(static_cast<float>(getInt(args, 0)));
}

Variable Routines::intToHexString(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofString(str(boost::format("%08x") % getInt(args, 0)));
}

Variable Routines::intToString(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofString(to_string(getInt(args, 0)));
}

Variable Routines::roundsToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getInt(args, 0) / 6.0f);
}

Variable Routines::stringToFloat(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(stof(getString(args, 0)));
}

Variable Routines::stringToInt(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(stoi(getString(args, 0)));
}

Variable Routines::turnsToSeconds(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getInt(args, 0) / 60.0f);
}

Variable Routines::yardsToMeters(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getFloat(args, 0) * 0.9144f);
}

Variable Routines::shipBuild(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(g_shipBuild));
}

Variable Routines::executeScript(const VariablesList &args, ExecutionContext &ctx) {
    string script(getString(args, 0));
    auto target = getObject(args, 1);
    int scriptVar = getInt(args, 2, -1);

    if (target) {
        _game->scriptRunner().run(script, target->id(), kObjectInvalid, kObjectInvalid, scriptVar);
    } else {
        debug("Script: executeScript: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getRunScriptVar(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.scriptVar);
}

Variable Routines::getLoadFromSaveGame(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(static_cast<int>(_game->isLoadFromSaveGame()));
}

Variable Routines::getStringByStrRef(const VariablesList &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    return Variable::ofString(Strings::instance().get(strRef));
}

Variable Routines::startNewModule(const VariablesList &args, ExecutionContext &ctx) {
    string moduleName(boost::to_lower_copy(getString(args, 0)));
    string waypoint(boost::to_lower_copy(getString(args, 1, "")));

    _game->scheduleModuleTransition(moduleName, waypoint);

    return Variable();
}

Variable Routines::playMovie(const VariablesList &args, ExecutionContext &ctx) {
    string movie(boost::to_lower_copy(getString(args, 0)));
    _game->playVideo(movie);
    return Variable();
}

Variable Routines::printString(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    info(str);
    return Variable();
}

Variable Routines::printFloat(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: use formatting parameters
    float value = getFloat(args, 0);
    int width = getInt(args, 1, 18);
    int decimals = getInt(args, 2, 9);

    info(to_string(value));

    return Variable();
}

Variable Routines::printInteger(const VariablesList &args, ExecutionContext &ctx) {
    int value = getInt(args, 0);
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

Variable Routines::random(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(reone::random(0, getInt(args, 0) - 1));
}

} // namespace game

} // namespace reone
