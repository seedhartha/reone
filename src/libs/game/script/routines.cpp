/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/script/routines.h"

#include "reone/game/di/services.h"
#include "reone/game/script/routine/context.h"
#include "reone/game/types.h"
#include "reone/script/variable.h"

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

static constexpr int kBaseItemInvalid = 256;

void Routines::init() {
    if (_gameId == GameID::TSL) {
        registerTslRoutines();
    } else {
        registerKotorRoutines();
    }
}

void Routines::registerKotorRoutines() {
    registerMainKotorRoutines();
    registerActionKotorRoutines();
    registerEffectKotorRoutines();
    registerMinigameKotorRoutines();
}

void Routines::registerTslRoutines() {
    registerMainTslRoutines();
    registerActionTslRoutines();
    registerEffectTslRoutines();
    registerMinigameTslRoutines();
}

Routine &Routines::get(int index) {
    if (_routines.count(index) == 0) {
        throw std::out_of_range("index out of range: " + std::to_string(index));
    }
    return _routines.at(index);
}

int Routines::getIndexByName(const std::string &name) const {
    for (auto it = _routines.begin(); it != _routines.end(); ++it) {
        if (it->second.name() == name) {
            return it->first;
        }
    }
    return -1;
}

void Routines::insert(
    int index,
    std::string name,
    VariableType retType,
    std::vector<VariableType> argTypes,
    Variable (*fn)(const std::vector<Variable> &args, const RoutineContext &ctx)) {

    Variable defRetValue;
    defRetValue.type = retType;
    switch (retType) {
    case VariableType::Float:
        defRetValue.floatValue = -1.0f;
        break;
    case VariableType::Object:
        defRetValue.objectId = kObjectInvalid;
        break;
    default:
        break;
    }

    _routines[index] = Routine(
        std::move(name),
        retType,
        std::move(defRetValue),
        std::move(argTypes),
        [this, fn](auto &args, auto &execution) {
            RoutineContext ctx(*_game, *_services, execution);
            return fn(args, std::move(ctx));
        });
}

} // namespace game

} // namespace reone
