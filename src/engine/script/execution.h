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

#include "executionstate.h"
#include "types.h"

namespace reone {

namespace script {

#define R_INSTR_HANDLER(a) void execute##a(const Instruction &);

struct ExecutionContext;
struct Instruction;
struct Variable;

class ScriptProgram;

class ScriptExecution : boost::noncopyable {
public:
    ScriptExecution(std::shared_ptr<ScriptProgram> program, std::unique_ptr<ExecutionContext> context);

    int run();

    int getStackSize() const;
    const Variable &getStackVariable(int index) const;

private:
    std::shared_ptr<ScriptProgram> _program;
    std::unique_ptr<ExecutionContext> _context;
    std::unordered_map<InstructionType, std::function<void(const Instruction &)>> _handlers;
    std::vector<Variable> _stack;
    std::vector<uint32_t> _returnOffsets;
    uint32_t _nextInstruction {0};
    int _globalCount {0};
    ExecutionState _savedState;

    void registerHandler(InstructionType type, std::function<void(ScriptExecution *, const Instruction &)> handler) {
        _handlers.insert(std::make_pair(type, std::bind(handler, this, std::placeholders::_1)));
    }

    Variable getVectorFromStack();
    Variable getFloatFromStack();
    void getTwoIntegersFromStack(Variable &left, Variable &right);

    // Handlers

    R_INSTR_HANDLER(CPDOWNSP)
    R_INSTR_HANDLER(RSADDI)
    R_INSTR_HANDLER(RSADDF)
    R_INSTR_HANDLER(RSADDS)
    R_INSTR_HANDLER(RSADDO)
    R_INSTR_HANDLER(RSADDEFF)
    R_INSTR_HANDLER(RSADDEVT)
    R_INSTR_HANDLER(RSADDLOC)
    R_INSTR_HANDLER(RSADDTAL)
    R_INSTR_HANDLER(CPTOPSP)
    R_INSTR_HANDLER(CONSTI)
    R_INSTR_HANDLER(CONSTF)
    R_INSTR_HANDLER(CONSTS)
    R_INSTR_HANDLER(CONSTO)
    R_INSTR_HANDLER(ACTION)
    R_INSTR_HANDLER(LOGANDII)
    R_INSTR_HANDLER(LOGORII)
    R_INSTR_HANDLER(INCORII)
    R_INSTR_HANDLER(EXCORII)
    R_INSTR_HANDLER(BOOLANDII)
    R_INSTR_HANDLER(EQUALxx)
    R_INSTR_HANDLER(EQUALTT)
    R_INSTR_HANDLER(NEQUALxx)
    R_INSTR_HANDLER(NEQUALTT)
    R_INSTR_HANDLER(GEQxx)
    R_INSTR_HANDLER(GTxx)
    R_INSTR_HANDLER(LTxx)
    R_INSTR_HANDLER(LEQxx)
    R_INSTR_HANDLER(SHLEFTII)
    R_INSTR_HANDLER(SHRIGHTII)
    R_INSTR_HANDLER(USHRIGHTII)
    R_INSTR_HANDLER(ADDxx)
    R_INSTR_HANDLER(SUBxx)
    R_INSTR_HANDLER(MULxx)
    R_INSTR_HANDLER(DIVxx)
    R_INSTR_HANDLER(MODII)
    R_INSTR_HANDLER(NEGI)
    R_INSTR_HANDLER(NEGF)
    R_INSTR_HANDLER(MOVSP)
    R_INSTR_HANDLER(JMP)
    R_INSTR_HANDLER(JSR)
    R_INSTR_HANDLER(JZ)
    R_INSTR_HANDLER(RETN)
    R_INSTR_HANDLER(DESTRUCT)
    R_INSTR_HANDLER(DECISP)
    R_INSTR_HANDLER(INCISP)
    R_INSTR_HANDLER(NOTI)
    R_INSTR_HANDLER(JNZ)
    R_INSTR_HANDLER(CPDOWNBP)
    R_INSTR_HANDLER(CPTOPBP)
    R_INSTR_HANDLER(DECIBP)
    R_INSTR_HANDLER(INCIBP)
    R_INSTR_HANDLER(SAVEBP)
    R_INSTR_HANDLER(RESTOREBP)
    R_INSTR_HANDLER(STORE_STATE)

    // END Handlers
};

} // namespace script

} // namespace reone
