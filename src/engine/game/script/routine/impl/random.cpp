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

#include "../../routines.h"

#include "../../../../common/random.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::random(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(reone::random(0, getInt(args, 0) - 1));
}

Variable Routines::d2(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return Variable::ofInt(result);
}

Variable Routines::d3(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return Variable::ofInt(result);
}

Variable Routines::d4(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return Variable::ofInt(result);
}

Variable Routines::d6(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return Variable::ofInt(result);
}

Variable Routines::d8(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return Variable::ofInt(result);
}

Variable Routines::d10(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return Variable::ofInt(result);
}

Variable Routines::d12(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return Variable::ofInt(result);
}

Variable Routines::d20(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return Variable::ofInt(result);
}

Variable Routines::d100(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return Variable::ofInt(result);
}

} // namespace game

} // namespace reone
