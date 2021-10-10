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
 *  Implementation of routines related to random number generation.
 */

#include "../declarations.h"

#include "../../../../../common/randomutil.h"
#include "../../../../../script/types.h"

#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable random(const vector<Variable> &args, const RoutineContext &ctx) {
    int max = getInt(args, 0);
    int result = reone::random(0, max - 1);

    return Variable::ofInt(result);
}

Variable d2(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return Variable::ofInt(result);
}

Variable d3(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return Variable::ofInt(result);
}

Variable d4(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return Variable::ofInt(result);
}

Variable d6(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return Variable::ofInt(result);
}

Variable d8(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return Variable::ofInt(result);
}

Variable d10(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return Variable::ofInt(result);
}

Variable d12(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return Variable::ofInt(result);
}

Variable d20(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return Variable::ofInt(result);
}

Variable d100(const vector<Variable> &args, const RoutineContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return Variable::ofInt(result);
}

} // namespace routine

} // namespace game

} // namespace reone
