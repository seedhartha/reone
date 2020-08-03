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

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace reone {

namespace script {

class ScriptProgram;
struct Variable;

struct ExecutionState {
    std::shared_ptr<ScriptProgram> program;
    std::vector<Variable> globals;
    std::vector<Variable> locals;
    uint32_t insOffset { 0 };
};

struct ExecutionContext {
    std::shared_ptr<ExecutionState> savedState;
    uint32_t callerId { 0xffffffff };
    uint32_t playerId { 0xffffffff };
    uint32_t enteringObjectId { 0xffffffff };
    std::function<void(uint32_t, const ExecutionContext &)> delayCommand;
};

} // namespace script

} // namespace reone
