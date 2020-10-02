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

#include "execution.h"

#include <stdexcept>

#include <boost/format.hpp>

#include "../core/debug.h"
#include "../core/log.h"

#include "routine.h"
#include "util.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::resource;

namespace reone {

namespace script {

static const int kStartInstructionOffset = 13;

ScriptExecution::ScriptExecution(const shared_ptr<ScriptProgram> &program, const ExecutionContext &ctx) : _context(ctx), _program(program) {
    _handlers.insert(make_pair(ByteCode::CopyDownSP, bind(&ScriptExecution::executeCopyDownSP, this, _1)));
    _handlers.insert(make_pair(ByteCode::Reserve, bind(&ScriptExecution::executeReserve, this, _1)));
    _handlers.insert(make_pair(ByteCode::CopyTopSP, bind(&ScriptExecution::executeCopyTopSP, this, _1)));
    _handlers.insert(make_pair(ByteCode::PushConstant, bind(&ScriptExecution::executePushConstant, this, _1)));
    _handlers.insert(make_pair(ByteCode::CallRoutine, bind(&ScriptExecution::executeCallRoutine, this, _1)));
    _handlers.insert(make_pair(ByteCode::LogicalAnd, bind(&ScriptExecution::executeLogicalAnd, this, _1)));
    _handlers.insert(make_pair(ByteCode::LogicalOr, bind(&ScriptExecution::executeLogicalOr, this, _1)));
    _handlers.insert(make_pair(ByteCode::InclusiveBitwiseOr, bind(&ScriptExecution::executeInclusiveBitwiseOr, this, _1)));
    _handlers.insert(make_pair(ByteCode::ExclusiveBitwiseOr, bind(&ScriptExecution::executeExclusiveBitwiseOr, this, _1)));
    _handlers.insert(make_pair(ByteCode::BitwiseAnd, bind(&ScriptExecution::executeBitwiseAnd, this, _1)));
    _handlers.insert(make_pair(ByteCode::Equal, bind(&ScriptExecution::executeEqual, this, _1)));
    _handlers.insert(make_pair(ByteCode::NotEqual, bind(&ScriptExecution::executeNotEqual, this, _1)));
    _handlers.insert(make_pair(ByteCode::GreaterThanOrEqual, bind(&ScriptExecution::executeGreaterThanOrEqual, this, _1)));
    _handlers.insert(make_pair(ByteCode::GreaterThan, bind(&ScriptExecution::executeGreaterThan, this, _1)));
    _handlers.insert(make_pair(ByteCode::LessThan, bind(&ScriptExecution::executeLessThan, this, _1)));
    _handlers.insert(make_pair(ByteCode::LessThanOrEqual, bind(&ScriptExecution::executeLessThanOrEqual, this, _1)));
    _handlers.insert(make_pair(ByteCode::ShiftLeft, bind(&ScriptExecution::executeShiftLeft, this, _1)));
    _handlers.insert(make_pair(ByteCode::ShiftRight, bind(&ScriptExecution::executeShiftRight, this, _1)));
    _handlers.insert(make_pair(ByteCode::UnsignedShiftRight, bind(&ScriptExecution::executeUnsignedShiftRight, this, _1)));
    _handlers.insert(make_pair(ByteCode::Add, bind(&ScriptExecution::executeAdd, this, _1)));
    _handlers.insert(make_pair(ByteCode::Subtract, bind(&ScriptExecution::executeSubtract, this, _1)));
    _handlers.insert(make_pair(ByteCode::Multiply, bind(&ScriptExecution::executeMultiply, this, _1)));
    _handlers.insert(make_pair(ByteCode::Divide, bind(&ScriptExecution::executeDivide, this, _1)));
    _handlers.insert(make_pair(ByteCode::Mod, bind(&ScriptExecution::executeMod, this, _1)));
    _handlers.insert(make_pair(ByteCode::Negate, bind(&ScriptExecution::executeNegate, this, _1)));
    _handlers.insert(make_pair(ByteCode::AdjustSP, bind(&ScriptExecution::executeAdjustSP, this, _1)));
    _handlers.insert(make_pair(ByteCode::Jump, bind(&ScriptExecution::executeJump, this, _1)));
    _handlers.insert(make_pair(ByteCode::JumpToSubroutine, bind(&ScriptExecution::executeJumpToSubroutine, this, _1)));
    _handlers.insert(make_pair(ByteCode::JumpIfZero, bind(&ScriptExecution::executeJumpIfZero, this, _1)));
    _handlers.insert(make_pair(ByteCode::Return, bind(&ScriptExecution::executeReturn, this, _1)));
    _handlers.insert(make_pair(ByteCode::LogicalNot, bind(&ScriptExecution::executeLogicalNot, this, _1)));
    _handlers.insert(make_pair(ByteCode::DecRelToSP, bind(&ScriptExecution::executeDecRelToSP, this, _1)));
    _handlers.insert(make_pair(ByteCode::IncRelToSP, bind(&ScriptExecution::executeIncRelToSP, this, _1)));
    _handlers.insert(make_pair(ByteCode::JumpIfNonZero, bind(&ScriptExecution::executeJumpIfNonZero, this, _1)));
    _handlers.insert(make_pair(ByteCode::CopyDownBP, bind(&ScriptExecution::executeCopyDownBP, this, _1)));
    _handlers.insert(make_pair(ByteCode::CopyTopBP, bind(&ScriptExecution::executeCopyTopBP, this, _1)));
    _handlers.insert(make_pair(ByteCode::DecRelToBP, bind(&ScriptExecution::executeDecRelToBP, this, _1)));
    _handlers.insert(make_pair(ByteCode::IncRelToBP, bind(&ScriptExecution::executeIncRelToBP, this, _1)));
    _handlers.insert(make_pair(ByteCode::SaveBP, bind(&ScriptExecution::executeSaveBP, this, _1)));
    _handlers.insert(make_pair(ByteCode::RestoreBP, bind(&ScriptExecution::executeRestoreBP, this, _1)));
    _handlers.insert(make_pair(ByteCode::StoreState, bind(&ScriptExecution::executeStoreState, this, _1)));
    _handlers.insert(make_pair(ByteCode::Noop, [](const Instruction &) {}));
}

int ScriptExecution::run() {
    debug("Script: " + _program->name());
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

        if (getDebugLevel() >= 2) {
            debug("Script: " + describeInstruction(ins), 3);
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
    assert(count == 1);

    int srcIdx = static_cast<int>(_stack.size()) - count;
    int dstIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack[dstIdx++] = _stack[srcIdx++];
    }
}

void ScriptExecution::executeReserve(const Instruction &ins) {
    VariableType type;
    switch (ins.type) {
        case InstructionType::Int:
            type = VariableType::Int;
            break;
        case InstructionType::Float:
            type = VariableType::Float;
            break;
        case InstructionType::String:
            type = VariableType::String;
            break;
        case InstructionType::Object:
            type = VariableType::Object;
            break;
        case InstructionType::Effect:
            type = VariableType::Effect;
            break;
        case InstructionType::Event:
            type = VariableType::Event;
            break;
        case InstructionType::Location:
            type = VariableType::Location;
            break;
        case InstructionType::Talent:
            type = VariableType::Talent;
            break;
        default:
            type = VariableType::Void;
            break;
    }
    _stack.push_back(Variable(type));
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
            Variable var(VariableType::Object);
            var.objectId = ins.objectId;
            _stack.push_back(move(var));
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

    if (getDebugLevel() >= 2) {
        debug(boost::format("Script: %s -> %s") % routine.name() % retValue.toString(), 2);
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
    assert(_stack.size() >= 2);

    right = _stack.back();
    _stack.pop_back();

    left = _stack.back();
    _stack.pop_back();

    assert(left.type == VariableType::Int && right.type == VariableType::Int);
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
    assert(count > 0);

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
    assert(count == 1);

    int srcIdx = _globalCount - count;
    int dstIdx = _globalCount + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack[dstIdx++] = _stack[srcIdx++];
    }
}

void ScriptExecution::executeCopyTopBP(const Instruction &ins) {
    int count = ins.size / 4;
    assert(count == 1);

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
    assert(!_stack.empty() && _stack.back().type == VariableType::Int);
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

} // namespace script

} // namespae reone
