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

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "program.h"
#include "types.h"
#include "variable.h"

namespace reone {

namespace script {

class ScriptExecution {
public:
    ScriptExecution(const std::shared_ptr<ScriptProgram> &program, const ExecutionContext &ctx);

    int run();

private:
    std::shared_ptr<ScriptProgram> _program;
    ExecutionContext _context;
    std::map<ByteCode, std::function<void(const Instruction &)>> _handlers;
    std::vector<Variable> _stack;
    std::vector<uint32_t> _returnOffsets;
    uint32_t _nextInstruction { 0 };
    int _globalCount { 0 };
    ExecutionState _savedState;

    ScriptExecution(const ScriptExecution &) = delete;
    ScriptExecution &operator=(const ScriptExecution &) = delete;

    void executeCopyDownSP(const Instruction &ins);
    void executeReserve(const Instruction &ins);
    void executeCopyTopSP(const Instruction &ins);
    void executePushConstant(const Instruction &ins);
    void executeCallRoutine(const Instruction &ins);
    Variable getVectorFromStack();
    Variable getFloatFromStack();
    void executeLogicalAnd(const Instruction &ins);
    void getTwoIntegersFromStack(Variable &left, Variable &right);
    void executeLogicalOr(const Instruction &ins);
    void executeInclusiveBitwiseOr(const Instruction &ins);
    void executeExclusiveBitwiseOr(const Instruction &ins);
    void executeBitwiseAnd(const Instruction &ins);
    void executeEqual(const Instruction &ins);
    void executeNotEqual(const Instruction &ins);
    void executeGreaterThanOrEqual(const Instruction &ins);
    void executeGreaterThan(const Instruction &ins);
    void executeLessThan(const Instruction &ins);
    void executeLessThanOrEqual(const Instruction &ins);
    void executeShiftLeft(const Instruction &ins);
    void executeShiftRight(const Instruction &ins);
    void executeUnsignedShiftRight(const Instruction &ins);
    void executeAdd(const Instruction &ins);
    void executeSubtract(const Instruction &ins);
    void executeMultiply(const Instruction &ins);
    void executeDivide(const Instruction &ins);
    void executeMod(const Instruction &ins);
    void executeNegate(const Instruction &ins);
    void executeAdjustSP(const Instruction &ins);
    void executeJump(const Instruction &ins);
    void executeJumpToSubroutine(const Instruction &ins);
    void executeJumpIfZero(const Instruction &ins);
    void executeReturn(const Instruction &ins);
    void executeDecRelToSP(const Instruction &ins);
    void executeIncRelToSP(const Instruction &ins);
    void executeLogicalNot(const Instruction &ins);
    void executeJumpIfNonZero(const Instruction &ins);
    void executeCopyDownBP(const Instruction &ins);
    void executeCopyTopBP(const Instruction &ins);
    void executeDecRelToBP(const Instruction &ins);
    void executeIncRelToBP(const Instruction &ins);
    void executeSaveBP(const Instruction &ins);
    void executeRestoreBP(const Instruction &ins);
    void executeStoreState(const Instruction &ins);
};

} // namespace script

} // namespae reone
