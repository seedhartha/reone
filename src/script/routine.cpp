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

#include "routine.h"

using namespace std;

namespace reone {

namespace script {

Routine::Routine(const string &name, VariableType retType, const vector<VariableType> &argTypes) :
    _name(name), _returnType(retType), _argumentTypes(argTypes) {
}

Routine::Routine(
    const string &name,
    VariableType retType,
    const vector<VariableType> &argTypes,
    const function<Variable(const vector<Variable> &, ExecutionContext &ctx)> &fn
) :
    _name(name), _returnType(retType), _argumentTypes(argTypes), _func(fn) {
}

Variable Routine::invoke(const vector<Variable> &args, ExecutionContext &ctx) const {
    if (_func) {
        return _func(args, ctx);
    }

    return Variable(_returnType);
}

const string &Routine::name() const {
    return _name;
}

VariableType Routine::returnType() const {
    return _returnType;
}

VariableType Routine::argumentType(int index) const {
    assert(index >= 0 && index < _argumentTypes.size());
    return _argumentTypes[index];
}

} // namespace script

} // namespace reone
