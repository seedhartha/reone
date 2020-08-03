/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <cassert>
#include <stdexcept>

#include "SDL2/SDL_timer.h"

#include "../core/log.h"

using namespace std;

using namespace reone::resources;

namespace reone {

namespace script {

RoutineDecl::RoutineDecl(const string &name, VariableType retType, const vector<VariableType> &argTypes) :
    _name(name), _returnType(retType), _argumentTypes(argTypes) {
}

RoutineDecl::RoutineDecl(
    const string &name,
    VariableType retType,
    const vector<VariableType> &argTypes,
    const function<Variable(const std::vector<Variable> &, ExecutionContext &ctx)> &fn
) :
    _name(name), _returnType(retType), _argumentTypes(argTypes), _func(fn) {
}

Variable RoutineDecl::invoke(const vector<Variable> &args, ExecutionContext &ctx) const {
    if (_func) {
        return _func(args, ctx);
    }

    return Variable(_returnType);
}

const string &RoutineDecl::name() const {
    return _name;
}

VariableType RoutineDecl::returnType() const {
    return _returnType;
}

VariableType RoutineDecl::argumentType(int index) const {
    assert(index >= 0 && index < _argumentTypes.size());
    return _argumentTypes[index];
}

RoutineManager &RoutineManager::instance() {
    static RoutineManager instance;
    return instance;
}

void RoutineManager::init(GameVersion version) {
    if (!_routines.empty()) return;

    switch (version) {
        case GameVersion::KotOR:
            addKotorRoutines();
            break;
        case GameVersion::TheSithLords:
            addTslRoutines();
            break;
        default:
            break;
    }
}

const RoutineDecl &RoutineManager::get(int index) {
    assert(index >= 0 && index < _routines.size());
    return _routines[index];
}

Variable RoutineManager::delayCommand(const std::vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Float &&
        args[1].type == VariableType::Action);

    if (ctx.delayCommand) {
        uint32_t timestamp = SDL_GetTicks() + args[0].floatValue * 1000.0f;
        ctx.delayCommand(timestamp, args[1].context);
    }

    return Variable();
}

Variable RoutineManager::getEnteringObject(const std::vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.enteringObjectId;
    return std::move(result);
}

Variable RoutineManager::getIsPC(const std::vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);
    return Variable(args[0].objectId == ctx.playerId);
}

} // namespace script

} // namespace reone
