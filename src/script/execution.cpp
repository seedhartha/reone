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

#include "execution.h"

#include <stdexcept>

#include <boost/format.hpp>

#include "../common/log.h"

#include "instrutil.h"
#include "routine.h"

using namespace std;
using namespace std::placeholders;

namespace reone {

namespace script {

static constexpr int kStartInstructionOffset = 13;

ScriptExecution::ScriptExecution(const shared_ptr<ScriptProgram> &program, const ExecutionContext &ctx) : _context(ctx), _program(program) {
    registerHandler(ByteCode::CopyDownSP, &ScriptExecution::executeCopyDownSP);
    registerHandler(ByteCode::Reserve, &ScriptExecution::executeReserve);
    registerHandler(ByteCode::CopyTopSP, &ScriptExecution::executeCopyTopSP);
    registerHandler(ByteCode::PushConstant, &ScriptExecution::executePushConstant);
    registerHandler(ByteCode::CallRoutine, &ScriptExecution::executeCallRoutine);
    registerHandler(ByteCode::LogicalAnd, &ScriptExecution::executeLogicalAnd);
    registerHandler(ByteCode::LogicalOr, &ScriptExecution::executeLogicalOr);
    registerHandler(ByteCode::InclusiveBitwiseOr, &ScriptExecution::executeInclusiveBitwiseOr);
    registerHandler(ByteCode::ExclusiveBitwiseOr, &ScriptExecution::executeExclusiveBitwiseOr);
    registerHandler(ByteCode::BitwiseAnd, &ScriptExecution::executeBitwiseAnd);
    registerHandler(ByteCode::Equal, &ScriptExecution::executeEqual);
    registerHandler(ByteCode::NotEqual, &ScriptExecution::executeNotEqual);
    registerHandler(ByteCode::GreaterThanOrEqual, &ScriptExecution::executeGreaterThanOrEqual);
    registerHandler(ByteCode::GreaterThan, &ScriptExecution::executeGreaterThan);
    registerHandler(ByteCode::LessThan, &ScriptExecution::executeLessThan);
    registerHandler(ByteCode::LessThanOrEqual, &ScriptExecution::executeLessThanOrEqual);
    registerHandler(ByteCode::ShiftLeft, &ScriptExecution::executeShiftLeft);
    registerHandler(ByteCode::ShiftRight, &ScriptExecution::executeShiftRight);
    registerHandler(ByteCode::UnsignedShiftRight, &ScriptExecution::executeUnsignedShiftRight);
    registerHandler(ByteCode::Add, &ScriptExecution::executeAdd);
    registerHandler(ByteCode::Subtract, &ScriptExecution::executeSubtract);
    registerHandler(ByteCode::Multiply, &ScriptExecution::executeMultiply);
    registerHandler(ByteCode::Divide, &ScriptExecution::executeDivide);
    registerHandler(ByteCode::Mod, &ScriptExecution::executeMod);
    registerHandler(ByteCode::Negate, &ScriptExecution::executeNegate);
    registerHandler(ByteCode::AdjustSP, &ScriptExecution::executeAdjustSP);
    registerHandler(ByteCode::Jump, &ScriptExecution::executeJump);
    registerHandler(ByteCode::JumpToSubroutine, &ScriptExecution::executeJumpToSubroutine);
    registerHandler(ByteCode::JumpIfZero, &ScriptExecution::executeJumpIfZero);
    registerHandler(ByteCode::Return, &ScriptExecution::executeReturn);
    registerHandler(ByteCode::Destruct, &ScriptExecution::executeDestruct);
    registerHandler(ByteCode::LogicalNot, &ScriptExecution::executeLogicalNot);
    registerHandler(ByteCode::DecRelToSP, &ScriptExecution::executeDecRelToSP);
    registerHandler(ByteCode::IncRelToSP, &ScriptExecution::executeIncRelToSP);
    registerHandler(ByteCode::JumpIfNonZero, &ScriptExecution::executeJumpIfNonZero);
    registerHandler(ByteCode::CopyDownBP, &ScriptExecution::executeCopyDownBP);
    registerHandler(ByteCode::CopyTopBP, &ScriptExecution::executeCopyTopBP);
    registerHandler(ByteCode::DecRelToBP, &ScriptExecution::executeDecRelToBP);
    registerHandler(ByteCode::IncRelToBP, &ScriptExecution::executeIncRelToBP);
    registerHandler(ByteCode::SaveBP, &ScriptExecution::executeSaveBP);
    registerHandler(ByteCode::RestoreBP, &ScriptExecution::executeRestoreBP);
    registerHandler(ByteCode::StoreState, &ScriptExecution::executeStoreState);

    _handlers.insert(make_pair(ByteCode::Noop, [](const Instruction &) {}));
}

int ScriptExecution::run() {
    debug("Script: run " + _program->name());
    uint32_t insOff = kStartInstructionOffset;

    if (_context.savedState) {
        vector<Variable> globals(_context.savedState->globals);
        copy(globals.begin(), globals.end(), back_inserter(_stack));
        _globalCount = static_cast<int>(_stack.size());

        vector<Variable> locals(_context.savedState->locals);
        copy(locals.begin(), locals.end(), back_inserter(_stack));

        insOff = _context.savedState->insOffset;
    }

    while (insOff < _program->length()) {
        const Instruction &ins = _program->getInstruction(insOff);
        auto handler = _handlers.find(ins.byteCode);

        if (handler == _handlers.end()) {
            warn("Script: not implemented: " + describeByteCode(ins.byteCode));
            return -1;
        }
        _nextInstruction = ins.nextOffset;

        if (getDebugLogLevel() >= 2) {
            debug("Script: instruction " + describeInstruction(ins), 3);
        }
        handler->second(ins);

        insOff = _nextInstruction;
    }

    if (!_stack.empty() && _stack.back().type == VariableType::Int) {
        return _stack.back().intValue;
    }

    return -1;
}

void ScriptExecution::executeCopyDownSP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = static_cast<int>(_stack.size()) - count;
    int dstIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack[dstIdx++] = _stack[srcIdx++];
    }
}

void ScriptExecution::executeReserve(const Instruction &ins) {
    Variable result;
    switch (ins.type) {
        case InstructionType::Int:
            result.type = VariableType::Int;
            break;
        case InstructionType::Float:
            result.type = VariableType::Float;
            break;
        case InstructionType::String:
            result.type = VariableType::String;
            break;
        case InstructionType::Object:
            result.type = VariableType::Object;
            break;
        case InstructionType::Effect:
            result.type = VariableType::Effect;
            break;
        case InstructionType::Event:
            result.type = VariableType::Event;
            break;
        case InstructionType::Location:
            result.type = VariableType::Location;
            break;
        case InstructionType::Talent:
            result.type = VariableType::Talent;
            break;
        default:
            result.type = VariableType::Void;
            break;
    }
    _stack.push_back(move(result));
}

void ScriptExecution::executeCopyTopSP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack.push_back(_stack[srcIdx++]);
    }
}

void ScriptExecution::executePushConstant(const Instruction &ins) {
    switch (ins.type) {
        case InstructionType::Int:
            _stack.push_back(ins.intValue);
            break;
        case InstructionType::Float:
            _stack.push_back(ins.floatValue);
            break;
        case InstructionType::Object: {
            shared_ptr<ScriptObject> object;
            switch (ins.objectId) {
                case kObjectSelf:
                    object = _context.caller;
                    break;
                case kObjectInvalid:
                    break;
                default:
                    throw logic_error("Invalid object id");
            }
            _stack.emplace_back(object);
            break;
        }
        case InstructionType::String:
            _stack.push_back(ins.strValue);
            break;
        default:
            throw invalid_argument("Script: invalid instruction type: " + to_string(static_cast<int>(ins.type)));
    }
}

void ScriptExecution::executeCallRoutine(const Instruction &ins) {
    const Routine &routine = _context.routines->get(ins.routine);

    if (ins.argCount > routine.argumentCount()) {
        throw runtime_error("Script: too many routine arguments");
    }
    vector<Variable> args;

    for (int i = 0; i < ins.argCount; ++i) {
        VariableType type = routine.argumentType(i);

        switch (type) {
            case VariableType::Vector:
                args.push_back(getVectorFromStack());
                break;

            case VariableType::Action: {
                ExecutionContext ctx(_context);
                ctx.savedState = make_shared<ExecutionState>(_savedState);
                args.push_back(ctx);
                break;
            }
            default:
                Variable var(_stack.back());

                if (var.type != type) {
                    throw runtime_error("Script: invalid argument variable type");
                }
                args.push_back(move(var));
                _stack.pop_back();
                break;
        }
    }
    Variable retValue = routine.invoke(args, _context);

    if (getDebugLogLevel() >= 2) {
        debug(boost::format("Script: action: '%s' -> %s") % routine.name() % retValue.toString(), 2);
    }
    switch (routine.returnType()) {
        case VariableType::Void:
            break;
        case VariableType::Vector:
            _stack.push_back(retValue.vecValue.z);
            _stack.push_back(retValue.vecValue.y);
            _stack.push_back(retValue.vecValue.x);
            break;
        default:
            _stack.push_back(retValue);
            break;
    }
}

Variable ScriptExecution::getVectorFromStack() {
    float z = getFloatFromStack().floatValue;
    float y = getFloatFromStack().floatValue;
    float x = getFloatFromStack().floatValue;

    return glm::vec3(x, y, z);
}

Variable ScriptExecution::getFloatFromStack() {
    Variable var(_stack.back());

    if (var.type != VariableType::Float) {
        throw runtime_error("Script: invalid variable type for a vector component");
    }
    _stack.pop_back();

    return move(var);
}

void ScriptExecution::executeLogicalAnd(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue && right.intValue);
}

void ScriptExecution::getTwoIntegersFromStack(Variable &left, Variable &right) {
    right = _stack.back();
    _stack.pop_back();

    left = _stack.back();
    _stack.pop_back();
}

void ScriptExecution::executeLogicalOr(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue || right.intValue);
}

void ScriptExecution::executeInclusiveBitwiseOr(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue | right.intValue);
}

void ScriptExecution::executeExclusiveBitwiseOr(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue ^ right.intValue);
}

void ScriptExecution::executeBitwiseAnd(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue & right.intValue);
}

void ScriptExecution::executeEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool equal = _stack[stackSize - 2] == _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(equal);
}

void ScriptExecution::executeNotEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool notEqual = _stack[stackSize - 2] != _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(notEqual);
}

void ScriptExecution::executeGreaterThanOrEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool ge = _stack[stackSize - 2] >= _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(ge);
}

void ScriptExecution::executeGreaterThan(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool greater = _stack[stackSize - 2] > _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(greater);
}

void ScriptExecution::executeLessThan(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool less = _stack[stackSize - 2] < _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(less);
}

void ScriptExecution::executeLessThanOrEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool le = _stack[stackSize - 2] <= _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(le);
}

void ScriptExecution::executeShiftLeft(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue << right.intValue);
}

void ScriptExecution::executeShiftRight(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue >> right.intValue);
}

void ScriptExecution::executeUnsignedShiftRight(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    // TODO: proper unsigned shift
    _stack.push_back(left.intValue >> right.intValue);
}

void ScriptExecution::executeAdd(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] + _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeSubtract(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] - _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeMultiply(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] * _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeDivide(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] / _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeMod(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(left.intValue % right.intValue);
}

void ScriptExecution::executeNegate(const Instruction &ins) {
    switch (ins.type) {
        case InstructionType::Int:
            _stack.back() = -_stack.back().intValue;
            break;
        case InstructionType::Float:
            _stack.back() = -_stack.back().floatValue;
            break;
        default:
            break;
    }
}

void ScriptExecution::executeAdjustSP(const Instruction &ins) {
    int count = -ins.stackOffset / 4;
    for (int i = 0; i < count; ++i) {
        _stack.pop_back();
    }
}

void ScriptExecution::executeJump(const Instruction &ins) {
    _nextInstruction = ins.jumpOffset;
}

void ScriptExecution::executeJumpToSubroutine(const Instruction &ins) {
    _returnOffsets.push_back(ins.nextOffset);
    _nextInstruction = ins.jumpOffset;
}

void ScriptExecution::executeJumpIfZero(const Instruction &ins) {
    bool zero = _stack.back().intValue == 0;
    _stack.pop_back();

    if (zero) {
        _nextInstruction = ins.jumpOffset;
    }
}

void ScriptExecution::executeReturn(const Instruction &ins) {
    if (_returnOffsets.empty()) {
        _nextInstruction = _program->length();
    } else {
        _nextInstruction = _returnOffsets.back();
        _returnOffsets.pop_back();
    }
}

void ScriptExecution::executeDestruct(const Instruction &ins) {
    int startIdx = static_cast<int>(_stack.size()) - ins.size / 4;
    int startIdxNoDestroy = startIdx + ins.stackOffset / 4;
    int countNoDestroy = ins.sizeNoDestroy / 4;

    for (int i = 0; i < countNoDestroy; ++i) {
        _stack[startIdx + i] = _stack[startIdxNoDestroy + i];
    }
    _stack.resize(startIdx + countNoDestroy);
}

void ScriptExecution::executeDecRelToSP(const Instruction &ins) {
    int dstIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;
    _stack[dstIdx].intValue--;
}

void ScriptExecution::executeIncRelToSP(const Instruction &ins) {
    int dstIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;
    _stack[dstIdx].intValue++;
}

void ScriptExecution::executeLogicalNot(const Instruction &ins) {
    bool zero = _stack.back().intValue == 0;
    _stack.pop_back();

    _stack.push_back(zero);
}

void ScriptExecution::executeJumpIfNonZero(const Instruction &ins) {
    bool zero = _stack.back().intValue == 0;
    _stack.pop_back();

    if (!zero) {
        _nextInstruction = ins.jumpOffset;
    }
}

void ScriptExecution::executeCopyDownBP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = static_cast<int>(_stack.size()) - count;
    int dstIdx = _globalCount + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack[dstIdx++] = _stack[srcIdx++];
    }
}

void ScriptExecution::executeCopyTopBP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = _globalCount + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack.push_back(_stack[srcIdx++]);
    }
}

void ScriptExecution::executeDecRelToBP(const Instruction &ins) {
    int dstIdx = _globalCount + ins.stackOffset / 4;
    _stack[dstIdx].intValue--;
}

void ScriptExecution::executeIncRelToBP(const Instruction &ins) {
    int dstIdx = _globalCount + ins.stackOffset / 4;
    _stack[dstIdx].intValue++;
}

void ScriptExecution::executeSaveBP(const Instruction &ins) {
    _globalCount = static_cast<int>(_stack.size());
    _stack.push_back(_globalCount);
}

void ScriptExecution::executeRestoreBP(const Instruction &ins) {
    _globalCount = _stack.back().intValue;
    _stack.pop_back();
}

void ScriptExecution::executeStoreState(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = _globalCount - count;

    _savedState.globals.clear();
    for (int i = 0; i < count; ++i) {
        _savedState.globals.push_back(_stack[srcIdx++]);
    }

    count = ins.sizeLocals / 4;
    srcIdx = static_cast<int>(_stack.size()) - count;

    _savedState.locals.clear();
    for (int i = 0; i < count; ++i) {
        _savedState.locals.push_back(_stack[srcIdx++]);
    }

    _savedState.program = _program;
    _savedState.insOffset = ins.offset + static_cast<int>(ins.type);
}

int ScriptExecution::stackSize() const {
    return static_cast<int>(_stack.size());
}

const Variable &ScriptExecution::getStackVariable(int index) const {
    return _stack[index];
}

} // namespace script

} // namespae reone
