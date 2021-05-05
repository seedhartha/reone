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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../common/log.h"

#include "instrutil.h"
#include "routine.h"

using namespace std;
using namespace std::placeholders;

namespace reone {

namespace script {

static constexpr int kStartInstructionOffset = 13;

ScriptExecution::ScriptExecution(const shared_ptr<ScriptProgram> &program, unique_ptr<ExecutionContext> context) : _context(move(context)), _program(program) {
    static unordered_map<ByteCode, function<void(ScriptExecution *, const Instruction &)>> handlers {
        { ByteCode::CopyDownSP, &ScriptExecution::executeCopyDownSP },
        { ByteCode::Reserve, &ScriptExecution::executeReserve },
        { ByteCode::CopyTopSP, &ScriptExecution::executeCopyTopSP },
        { ByteCode::PushConstant, &ScriptExecution::executePushConstant },
        { ByteCode::CallRoutine, &ScriptExecution::executeCallRoutine },
        { ByteCode::LogicalAnd, &ScriptExecution::executeLogicalAnd },
        { ByteCode::LogicalOr, &ScriptExecution::executeLogicalOr },
        { ByteCode::InclusiveBitwiseOr, &ScriptExecution::executeInclusiveBitwiseOr },
        { ByteCode::ExclusiveBitwiseOr, &ScriptExecution::executeExclusiveBitwiseOr },
        { ByteCode::BitwiseAnd, &ScriptExecution::executeBitwiseAnd },
        { ByteCode::Equal, &ScriptExecution::executeEqual },
        { ByteCode::NotEqual, &ScriptExecution::executeNotEqual },
        { ByteCode::GreaterThanOrEqual, &ScriptExecution::executeGreaterThanOrEqual },
        { ByteCode::GreaterThan, &ScriptExecution::executeGreaterThan },
        { ByteCode::LessThan, &ScriptExecution::executeLessThan },
        { ByteCode::LessThanOrEqual, &ScriptExecution::executeLessThanOrEqual },
        { ByteCode::ShiftLeft, &ScriptExecution::executeShiftLeft },
        { ByteCode::ShiftRight, &ScriptExecution::executeShiftRight },
        { ByteCode::UnsignedShiftRight, &ScriptExecution::executeUnsignedShiftRight },
        { ByteCode::Add, &ScriptExecution::executeAdd },
        { ByteCode::Subtract, &ScriptExecution::executeSubtract },
        { ByteCode::Multiply, &ScriptExecution::executeMultiply },
        { ByteCode::Divide, &ScriptExecution::executeDivide },
        { ByteCode::Mod, &ScriptExecution::executeMod },
        { ByteCode::Negate, &ScriptExecution::executeNegate },
        { ByteCode::AdjustSP, &ScriptExecution::executeAdjustSP },
        { ByteCode::Jump, &ScriptExecution::executeJump },
        { ByteCode::JumpToSubroutine, &ScriptExecution::executeJumpToSubroutine },
        { ByteCode::JumpIfZero, &ScriptExecution::executeJumpIfZero },
        { ByteCode::Return, &ScriptExecution::executeReturn },
        { ByteCode::Destruct, &ScriptExecution::executeDestruct },
        { ByteCode::LogicalNot, &ScriptExecution::executeLogicalNot },
        { ByteCode::DecRelToSP, &ScriptExecution::executeDecRelToSP },
        { ByteCode::IncRelToSP, &ScriptExecution::executeIncRelToSP },
        { ByteCode::JumpIfNonZero, &ScriptExecution::executeJumpIfNonZero },
        { ByteCode::CopyDownBP, &ScriptExecution::executeCopyDownBP },
        { ByteCode::CopyTopBP, &ScriptExecution::executeCopyTopBP },
        { ByteCode::DecRelToBP, &ScriptExecution::executeDecRelToBP },
        { ByteCode::IncRelToBP, &ScriptExecution::executeIncRelToBP },
        { ByteCode::SaveBP, &ScriptExecution::executeSaveBP },
        { ByteCode::RestoreBP, &ScriptExecution::executeRestoreBP },
        { ByteCode::StoreState, &ScriptExecution::executeStoreState }
    };
    for (auto &pair : handlers) {
        registerHandler(pair.first, pair.second);
    }
    _handlers.insert(make_pair(ByteCode::Noop, [](const Instruction &) {}));
}

int ScriptExecution::run() {
    debug(boost::format("Script: run %s as %u") % _program->name() % _context->callerId, 1, DebugChannels::script);
    uint32_t insOff = kStartInstructionOffset;

    if (_context->savedState) {
        vector<Variable> globals(_context->savedState->globals);
        copy(globals.begin(), globals.end(), back_inserter(_stack));
        _globalCount = static_cast<int>(_stack.size());

        vector<Variable> locals(_context->savedState->locals);
        copy(locals.begin(), locals.end(), back_inserter(_stack));

        insOff = _context->savedState->insOffset;
    }

    while (insOff < _program->length()) {
        const Instruction &ins = _program->getInstruction(insOff);
        auto handler = _handlers.find(ins.byteCode);

        if (handler == _handlers.end()) {
            debug("Script: byte code not implemented: " + describeByteCode(ins.byteCode), 1, DebugChannels::script);
            return -1;
        }
        _nextInstruction = ins.nextOffset;

        if (getDebugLogLevel() >= 2) {
            debug(boost::format("Script: instruction: %s") % describeInstruction(ins), 3, DebugChannels::script);
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
            _stack.push_back(Variable::ofInt(ins.intValue));
            break;
        case InstructionType::Float:
            _stack.push_back(Variable::ofFloat(ins.floatValue));
            break;
        case InstructionType::Object: {
            uint32_t objectId = ins.objectId == kObjectSelf ? _context->callerId : ins.objectId;
            _stack.push_back(Variable::ofObject(objectId));
            break;
        }
        case InstructionType::String:
            _stack.push_back(Variable::ofString(ins.strValue));
            break;
        default:
            throw invalid_argument("Script: invalid instruction type: " + to_string(static_cast<int>(ins.type)));
    }
}

void ScriptExecution::executeCallRoutine(const Instruction &ins) {
    const Routine &routine = _context->routines->get(ins.routine);

    if (ins.argCount > routine.getArgumentCount()) {
        throw runtime_error("Script: too many routine arguments");
    }
    vector<Variable> args;

    for (int i = 0; i < ins.argCount; ++i) {
        VariableType type = routine.getArgumentType(i);

        switch (type) {
            case VariableType::Vector:
                args.push_back(getVectorFromStack());
                break;

            case VariableType::Action: {
                auto ctx = make_shared<ExecutionContext>(*_context);
                ctx->savedState = make_shared<ExecutionState>(_savedState);
                args.push_back(Variable::ofAction(move(ctx)));
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
    Variable retValue = routine.invoke(args, *_context);

    if (getDebugLogLevel() >= 2) {
        vector<string> argStrings;
        for (auto &arg : args) {
            argStrings.push_back(arg.toString());
        }
        string argsString(boost::join(argStrings, ", "));
        debug(boost::format("Script: action: %04x %s(%s) -> %s") % ins.offset % routine.name() % argsString % retValue.toString(), 2, DebugChannels::script);
    }
    switch (routine.returnType()) {
        case VariableType::Void:
            break;
        case VariableType::Vector:
            _stack.push_back(Variable::ofFloat(retValue.vecValue.z));
            _stack.push_back(Variable::ofFloat(retValue.vecValue.y));
            _stack.push_back(Variable::ofFloat(retValue.vecValue.x));
            break;
        default:
            _stack.push_back(retValue);
            break;
    }
}

Variable ScriptExecution::getVectorFromStack() {
    float x = getFloatFromStack().floatValue;
    float y = getFloatFromStack().floatValue;
    float z = getFloatFromStack().floatValue;

    return Variable::ofVector(glm::vec3(x, y, z));
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

    _stack.push_back(Variable::ofInt(static_cast<int>(left.intValue && right.intValue)));
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

    _stack.push_back(Variable::ofInt(static_cast<int>(left.intValue || right.intValue)));
}

void ScriptExecution::executeInclusiveBitwiseOr(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue | right.intValue));
}

void ScriptExecution::executeExclusiveBitwiseOr(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue ^ right.intValue));
}

void ScriptExecution::executeBitwiseAnd(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue & right.intValue));
}

void ScriptExecution::executeEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool equal = _stack[stackSize - 2] == _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(equal)));
}

void ScriptExecution::executeNotEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool notEqual = _stack[stackSize - 2] != _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(notEqual)));
}

void ScriptExecution::executeGreaterThanOrEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool ge = _stack[stackSize - 2] >= _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(ge)));
}

void ScriptExecution::executeGreaterThan(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool greater = _stack[stackSize - 2] > _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(greater)));
}

void ScriptExecution::executeLessThan(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool less = _stack[stackSize - 2] < _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(less)));
}

void ScriptExecution::executeLessThanOrEqual(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool le = _stack[stackSize - 2] <= _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(le)));
}

void ScriptExecution::executeShiftLeft(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue << right.intValue));
}

void ScriptExecution::executeShiftRight(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue >> right.intValue));
}

void ScriptExecution::executeUnsignedShiftRight(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    // TODO: proper unsigned shift
    _stack.push_back(Variable::ofInt(left.intValue >> right.intValue));
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

    _stack.push_back(Variable::ofInt(left.intValue % right.intValue));
}

void ScriptExecution::executeNegate(const Instruction &ins) {
    switch (ins.type) {
        case InstructionType::Int:
            _stack.back().intValue *= -1;
            break;
        case InstructionType::Float:
            _stack.back().floatValue *= -1.0f;
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

    _stack.push_back(Variable::ofInt(static_cast<int>(zero)));
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
    _stack.push_back(Variable::ofInt(_globalCount));
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

int ScriptExecution::getStackSize() const {
    return static_cast<int>(_stack.size());
}

const Variable &ScriptExecution::getStackVariable(int index) const {
    return _stack[index];
}

} // namespace script

} // namespae reone
