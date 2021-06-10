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

#pragma once

#include "variable.h"

namespace reone {

namespace script {

class Routine {
public:
    Routine() = default;
    Routine(std::string name, VariableType retType, std::vector<VariableType> argTypes);
    Routine(std::string name, VariableType retType, std::vector<VariableType> argTypes, const std::function<Variable(const std::vector<Variable> &, ExecutionContext &ctx)> &fn);

    Variable invoke(const std::vector<Variable> &args, ExecutionContext &ctx) const;

    int getArgumentCount() const;
    VariableType getArgumentType(int index) const;

    const std::string &name() const { return _name; }
    VariableType returnType() const { return _returnType; }

private:
    std::string _name;
    VariableType _returnType { VariableType::Void };
    std::vector<VariableType> _argumentTypes;
    std::function<Variable(const std::vector<Variable> &, ExecutionContext &ctx)> _func;
};

} // namespace script

} // namespace reone
