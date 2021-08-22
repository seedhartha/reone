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

#include "declarations.h"

#include "../../../../common/random.h"
#include "../../../../script/types.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable random(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::ofInt(reone::random(0, getIntOrElse(args, 0) - 1));
}

Variable d2(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return Variable::ofInt(result);
}

Variable d3(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return Variable::ofInt(result);
}

Variable d4(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return Variable::ofInt(result);
}

Variable d6(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return Variable::ofInt(result);
}

Variable d8(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return Variable::ofInt(result);
}

Variable d10(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return Variable::ofInt(result);
}

Variable d12(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return Variable::ofInt(result);
}

Variable d20(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getIntOrElse(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return Variable::ofInt(result);
}

Variable d100(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
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
