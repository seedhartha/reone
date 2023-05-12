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

    void stackPush(Variable var) {
        _stack.push_back(std::move(var));
    }

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

    int getIntFromStack();
    float getFloatFromStack();
    glm::vec3 getVectorFromStack();

    void withStackVariables(const std::function<void(const Variable &, const Variable &)> &fn);
    void withIntsFromStack(const std::function<void(int, int)> &fn);
    void withIntFloatFromStack(const std::function<void(int, float)> &fn);
    void withFloatIntFromStack(const std::function<void(float, int)> &fn);
    void withFloatsFromStack(const std::function<void(float, float)> &fn);
    void withStringsFromStack(const std::function<void(const std::string &, const std::string &)> &fn);
    void withObjectsFromStack(const std::function<void(uint32_t, uint32_t)> &fn);
    void withEffectsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn);
    void withEventsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn);
    void withLocationsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn);
    void withTalentsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn);

    void withFloatVectorFromStack(const std::function<void(float, const glm::vec3 &)> &fn);
    void withVectorFloatFromStack(const std::function<void(const glm::vec3 &, float)> &fn);
    void withVectorsFromStack(const std::function<void(const glm::vec3 &, const glm::vec3 &)> &fn);

    void throwIfInvalidType(VariableType expected, VariableType actual);

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
    R_INSTR_HANDLER(EQUALII)
    R_INSTR_HANDLER(EQUALFF)
    R_INSTR_HANDLER(EQUALSS)
    R_INSTR_HANDLER(EQUALOO)
    R_INSTR_HANDLER(EQUALTT)
    R_INSTR_HANDLER(EQUALEFFEFF)
    R_INSTR_HANDLER(EQUALEVTEVT)
    R_INSTR_HANDLER(EQUALLOCLOC)
    R_INSTR_HANDLER(EQUALTALTAL)
    R_INSTR_HANDLER(NEQUALII)
    R_INSTR_HANDLER(NEQUALFF)
    R_INSTR_HANDLER(NEQUALSS)
    R_INSTR_HANDLER(NEQUALOO)
    R_INSTR_HANDLER(NEQUALTT)
    R_INSTR_HANDLER(NEQUALEFFEFF)
    R_INSTR_HANDLER(NEQUALEVTEVT)
    R_INSTR_HANDLER(NEQUALLOCLOC)
    R_INSTR_HANDLER(NEQUALTALTAL)
    R_INSTR_HANDLER(GEQII)
    R_INSTR_HANDLER(GEQFF)
    R_INSTR_HANDLER(GTII)
    R_INSTR_HANDLER(GTFF)
    R_INSTR_HANDLER(LTII)
    R_INSTR_HANDLER(LTFF)
    R_INSTR_HANDLER(LEQII)
    R_INSTR_HANDLER(LEQFF)
    R_INSTR_HANDLER(SHLEFTII)
    R_INSTR_HANDLER(SHRIGHTII)
    R_INSTR_HANDLER(USHRIGHTII)
    R_INSTR_HANDLER(ADDII)
    R_INSTR_HANDLER(ADDIF)
    R_INSTR_HANDLER(ADDFI)
    R_INSTR_HANDLER(ADDFF)
    R_INSTR_HANDLER(ADDSS)
    R_INSTR_HANDLER(ADDVV)
    R_INSTR_HANDLER(SUBII)
    R_INSTR_HANDLER(SUBIF)
    R_INSTR_HANDLER(SUBFI)
    R_INSTR_HANDLER(SUBFF)
    R_INSTR_HANDLER(SUBVV)
    R_INSTR_HANDLER(MULII)
    R_INSTR_HANDLER(MULIF)
    R_INSTR_HANDLER(MULFI)
    R_INSTR_HANDLER(MULFF)
    R_INSTR_HANDLER(MULVF)
    R_INSTR_HANDLER(MULFV)
    R_INSTR_HANDLER(DIVII)
    R_INSTR_HANDLER(DIVIF)
    R_INSTR_HANDLER(DIVFI)
    R_INSTR_HANDLER(DIVFF)
    R_INSTR_HANDLER(DIVVF)
    R_INSTR_HANDLER(DIVFV)
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
