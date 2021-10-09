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

#include "../common/guardutil.h"
#include "../common/logutil.h"

#include "executioncontext.h"
#include "instrutil.h"
#include "program.h"
#include "routine.h"
#include "routineprovider.h"
#include "variable.h"

using namespace std;
using namespace std::placeholders;

namespace reone {

namespace script {

static constexpr int kStartInstructionOffset = 13;

ScriptExecution::ScriptExecution(shared_ptr<ScriptProgram> program, unique_ptr<ExecutionContext> context) :
    _context(move(context)),
    _program(ensurePresent(program, "program")) {

    static unordered_map<InstructionType, function<void(ScriptExecution *, const Instruction &)>> g_handlers {
        {InstructionType::CPDOWNSP, &ScriptExecution::executeCPDOWNSP},
        {InstructionType::RSADDI, &ScriptExecution::executeRSADDI},
        {InstructionType::RSADDF, &ScriptExecution::executeRSADDF},
        {InstructionType::RSADDS, &ScriptExecution::executeRSADDS},
        {InstructionType::RSADDO, &ScriptExecution::executeRSADDO},
        {InstructionType::RSADDEFF, &ScriptExecution::executeRSADDEFF},
        {InstructionType::RSADDEVT, &ScriptExecution::executeRSADDEVT},
        {InstructionType::RSADDLOC, &ScriptExecution::executeRSADDLOC},
        {InstructionType::RSADDTAL, &ScriptExecution::executeRSADDTAL},
        {InstructionType::CPTOPSP, &ScriptExecution::executeCPTOPSP},
        {InstructionType::CONSTI, &ScriptExecution::executeCONSTI},
        {InstructionType::CONSTF, &ScriptExecution::executeCONSTF},
        {InstructionType::CONSTS, &ScriptExecution::executeCONSTS},
        {InstructionType::CONSTO, &ScriptExecution::executeCONSTO},
        {InstructionType::ACTION, &ScriptExecution::executeACTION},
        {InstructionType::LOGANDII, &ScriptExecution::executeLOGANDII},
        {InstructionType::LOGORII, &ScriptExecution::executeLOGORII},
        {InstructionType::INCORII, &ScriptExecution::executeINCORII},
        {InstructionType::EXCORII, &ScriptExecution::executeEXCORII},
        {InstructionType::BOOLANDII, &ScriptExecution::executeBOOLANDII},
        {InstructionType::EQUALII, &ScriptExecution::executeEQUALxx},
        {InstructionType::EQUALFF, &ScriptExecution::executeEQUALxx},
        {InstructionType::EQUALSS, &ScriptExecution::executeEQUALxx},
        {InstructionType::EQUALOO, &ScriptExecution::executeEQUALxx},
        {InstructionType::EQUALTT, &ScriptExecution::executeEQUALTT},
        {InstructionType::EQUALEFFEFF, &ScriptExecution::executeEQUALxx},
        {InstructionType::EQUALEVTEVT, &ScriptExecution::executeEQUALxx},
        {InstructionType::EQUALLOCLOC, &ScriptExecution::executeEQUALxx},
        {InstructionType::EQUALTALTAL, &ScriptExecution::executeEQUALxx},
        {InstructionType::NEQUALII, &ScriptExecution::executeNEQUALxx},
        {InstructionType::NEQUALFF, &ScriptExecution::executeNEQUALxx},
        {InstructionType::NEQUALSS, &ScriptExecution::executeNEQUALxx},
        {InstructionType::NEQUALOO, &ScriptExecution::executeNEQUALxx},
        {InstructionType::NEQUALTT, &ScriptExecution::executeNEQUALTT},
        {InstructionType::NEQUALEFFEFF, &ScriptExecution::executeNEQUALxx},
        {InstructionType::NEQUALEVTEVT, &ScriptExecution::executeNEQUALxx},
        {InstructionType::NEQUALLOCLOC, &ScriptExecution::executeNEQUALxx},
        {InstructionType::NEQUALTALTAL, &ScriptExecution::executeNEQUALxx},
        {InstructionType::GEQII, &ScriptExecution::executeGEQxx},
        {InstructionType::GEQFF, &ScriptExecution::executeGEQxx},
        {InstructionType::GTII, &ScriptExecution::executeGTxx},
        {InstructionType::GTFF, &ScriptExecution::executeGTxx},
        {InstructionType::LTII, &ScriptExecution::executeLTxx},
        {InstructionType::LTFF, &ScriptExecution::executeLTxx},
        {InstructionType::LEQII, &ScriptExecution::executeLEQxx},
        {InstructionType::LEQFF, &ScriptExecution::executeLEQxx},
        {InstructionType::SHLEFTII, &ScriptExecution::executeSHLEFTII},
        {InstructionType::SHRIGHTII, &ScriptExecution::executeSHRIGHTII},
        {InstructionType::USHRIGHTII, &ScriptExecution::executeUSHRIGHTII},
        {InstructionType::ADDII, &ScriptExecution::executeADDxx},
        {InstructionType::ADDIF, &ScriptExecution::executeADDxx},
        {InstructionType::ADDFI, &ScriptExecution::executeADDxx},
        {InstructionType::ADDFF, &ScriptExecution::executeADDxx},
        {InstructionType::ADDSS, &ScriptExecution::executeADDxx},
        {InstructionType::ADDVV, &ScriptExecution::executeADDxx},
        {InstructionType::SUBII, &ScriptExecution::executeSUBxx},
        {InstructionType::SUBIF, &ScriptExecution::executeSUBxx},
        {InstructionType::SUBFI, &ScriptExecution::executeSUBxx},
        {InstructionType::SUBFF, &ScriptExecution::executeSUBxx},
        {InstructionType::SUBVV, &ScriptExecution::executeSUBxx},
        {InstructionType::MULII, &ScriptExecution::executeMULxx},
        {InstructionType::MULIF, &ScriptExecution::executeMULxx},
        {InstructionType::MULFI, &ScriptExecution::executeMULxx},
        {InstructionType::MULFF, &ScriptExecution::executeMULxx},
        {InstructionType::MULVF, &ScriptExecution::executeMULxx},
        {InstructionType::MULFV, &ScriptExecution::executeMULxx},
        {InstructionType::DIVII, &ScriptExecution::executeDIVxx},
        {InstructionType::DIVIF, &ScriptExecution::executeDIVxx},
        {InstructionType::DIVFI, &ScriptExecution::executeDIVxx},
        {InstructionType::DIVFF, &ScriptExecution::executeDIVxx},
        {InstructionType::DIVVF, &ScriptExecution::executeDIVxx},
        {InstructionType::DIVFV, &ScriptExecution::executeDIVxx},
        {InstructionType::MODII, &ScriptExecution::executeMODII},
        {InstructionType::NEGI, &ScriptExecution::executeNEGI},
        {InstructionType::NEGF, &ScriptExecution::executeNEGF},
        {InstructionType::MOVSP, &ScriptExecution::executeMOVSP},
        {InstructionType::JMP, &ScriptExecution::executeJMP},
        {InstructionType::JSR, &ScriptExecution::executeJSR},
        {InstructionType::JZ, &ScriptExecution::executeJZ},
        {InstructionType::RETN, &ScriptExecution::executeRETN},
        {InstructionType::DESTRUCT, &ScriptExecution::executeDESTRUCT},
        {InstructionType::NOTI, &ScriptExecution::executeNOTI},
        {InstructionType::DECISP, &ScriptExecution::executeDECISP},
        {InstructionType::INCISP, &ScriptExecution::executeINCISP},
        {InstructionType::JNZ, &ScriptExecution::executeJNZ},
        {InstructionType::CPDOWNBP, &ScriptExecution::executeCPDOWNBP},
        {InstructionType::CPTOPBP, &ScriptExecution::executeCPTOPBP},
        {InstructionType::DECIBP, &ScriptExecution::executeDECIBP},
        {InstructionType::INCIBP, &ScriptExecution::executeINCIBP},
        {InstructionType::SAVEBP, &ScriptExecution::executeSAVEBP},
        {InstructionType::RESTOREBP, &ScriptExecution::executeRESTOREBP},
        {InstructionType::STORE_STATE, &ScriptExecution::executeSTORE_STATE}};
    for (auto &pair : g_handlers) {
        registerHandler(pair.first, pair.second);
    }
    _handlers.insert(make_pair(InstructionType::NOP, [](auto &) {}));
    _handlers.insert(make_pair(InstructionType::NOP2, [](auto &) {}));
}

int ScriptExecution::run() {
    uint32_t insOff = kStartInstructionOffset;

    if (_context->savedState) {
        vector<Variable> globals(_context->savedState->globals);
        copy(globals.begin(), globals.end(), back_inserter(_stack));
        _globalCount = static_cast<int>(_stack.size());

        vector<Variable> locals(_context->savedState->locals);
        copy(locals.begin(), locals.end(), back_inserter(_stack));

        insOff = _context->savedState->insOffset;
    }

    debug(boost::format("Run '%s': offset=%04x, caller=%u, triggerrer=%u") %
              _program->name() %
              insOff %
              _context->callerId %
              _context->triggererId,
          LogChannels::script);

    while (insOff < _program->length()) {
        const Instruction &ins = _program->getInstruction(insOff);
        auto handler = _handlers.find(ins.type);

        if (handler == _handlers.end()) {
            error(boost::format("Instruction not implemented: %04x") % static_cast<int>(ins.type), LogChannels::script);
            return -1;
        }
        _nextInstruction = ins.nextOffset;

        if (isLogChannelEnabled(LogChannels::script3)) {
            debug(boost::format("Instruction: %s") % describeInstruction(ins, *_context->routines), LogChannels::script3);
        }
        try {
            handler->second(ins);
        } catch (const exception &ex) {
            debug(boost::format("Halt '%s'") % _program->name(), LogChannels::script);
            return -1;
        }

        insOff = _nextInstruction;
    }

    if (!_stack.empty() && _stack.back().type == VariableType::Int) {
        return _stack.back().intValue;
    }

    return -1;
}

void ScriptExecution::executeCPDOWNSP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = static_cast<int>(_stack.size()) - count;
    int dstIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack[dstIdx++] = _stack[srcIdx++];
    }
}

void ScriptExecution::executeRSADDI(const Instruction &ins) {
    _stack.push_back(Variable::ofInt(0));
}

void ScriptExecution::executeRSADDF(const Instruction &ins) {
    _stack.push_back(Variable::ofFloat(0.0f));
}

void ScriptExecution::executeRSADDS(const Instruction &ins) {
    _stack.push_back(Variable::ofString(""));
}

void ScriptExecution::executeRSADDO(const Instruction &ins) {
    _stack.push_back(Variable::ofObject(kObjectInvalid));
}

void ScriptExecution::executeRSADDEFF(const Instruction &ins) {
    _stack.push_back(Variable::ofEffect(nullptr));
}

void ScriptExecution::executeRSADDEVT(const Instruction &ins) {
    _stack.push_back(Variable::ofEvent(nullptr));
}

void ScriptExecution::executeRSADDLOC(const Instruction &ins) {
    _stack.push_back(Variable::ofLocation(nullptr));
}

void ScriptExecution::executeRSADDTAL(const Instruction &ins) {
    _stack.push_back(Variable::ofTalent(nullptr));
}

void ScriptExecution::executeCPTOPSP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack.push_back(_stack[srcIdx++]);
    }
}

void ScriptExecution::executeCONSTI(const Instruction &ins) {
    _stack.push_back(Variable::ofInt(ins.intValue));
}

void ScriptExecution::executeCONSTF(const Instruction &ins) {
    _stack.push_back(Variable::ofFloat(ins.floatValue));
}

void ScriptExecution::executeCONSTS(const Instruction &ins) {
    _stack.push_back(Variable::ofString(ins.strValue));
}

void ScriptExecution::executeCONSTO(const Instruction &ins) {
    uint32_t objectId = ins.objectId == kObjectSelf ? _context->callerId : ins.objectId;
    _stack.push_back(Variable::ofObject(objectId));
}

void ScriptExecution::executeACTION(const Instruction &ins) {
    const Routine &routine = _context->routines->get(ins.routine);
    if (ins.argCount > routine.getArgumentCount()) {
        throw runtime_error("Too many routine arguments");
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
                throw runtime_error("Invalid argument variable type");
            }
            args.push_back(move(var));
            _stack.pop_back();
            break;
        }
    }

    Variable retValue = routine.invoke(args, *_context);
    if (isLogChannelEnabled(LogChannels::script2)) {
        vector<string> argStrings;
        for (auto &arg : args) {
            argStrings.push_back(arg.toString());
        }
        string argsString(boost::join(argStrings, ", "));
        debug(boost::format("Action: %04x %s(%s) -> %s") % ins.offset % routine.name() % argsString % retValue.toString(), LogChannels::script2);
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
        throw runtime_error("Invalid variable type for a vector component");
    }
    _stack.pop_back();

    return move(var);
}

void ScriptExecution::executeLOGANDII(const Instruction &ins) {
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

void ScriptExecution::executeLOGORII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(static_cast<int>(left.intValue || right.intValue)));
}

void ScriptExecution::executeINCORII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue | right.intValue));
}

void ScriptExecution::executeEXCORII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue ^ right.intValue));
}

void ScriptExecution::executeBOOLANDII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue & right.intValue));
}

void ScriptExecution::executeEQUALxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool equal = _stack[stackSize - 2] == _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(equal)));
}

void ScriptExecution::executeEQUALTT(const Instruction &ins) {
    int numVariables = ins.size / 4;
    vector<Variable> vars1;
    for (int i = 0; i < numVariables; ++i) {
        vars1.push_back(move(_stack.back()));
        _stack.pop_back();
    }
    vector<Variable> vars2;
    for (int i = 0; i < numVariables; ++i) {
        vars2.push_back(move(_stack.back()));
        _stack.pop_back();
    }
    bool equal = std::equal(vars1.begin(), vars1.end(), vars2.begin());
    _stack.push_back(Variable::ofInt(static_cast<int>(equal)));
}

void ScriptExecution::executeNEQUALxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool notEqual = _stack[stackSize - 2] != _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(notEqual)));
}

void ScriptExecution::executeNEQUALTT(const Instruction &ins) {
    int numVariables = ins.size / 4;
    vector<Variable> vars1;
    for (int i = 0; i < numVariables; ++i) {
        vars1.push_back(move(_stack.back()));
        _stack.pop_back();
    }
    vector<Variable> vars2;
    for (int i = 0; i < numVariables; ++i) {
        vars2.push_back(move(_stack.back()));
        _stack.pop_back();
    }
    bool notEqual = !std::equal(vars1.begin(), vars1.end(), vars2.begin());
    _stack.push_back(Variable::ofInt(static_cast<int>(notEqual)));
}

void ScriptExecution::executeGEQxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool ge = _stack[stackSize - 2] >= _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(ge)));
}

void ScriptExecution::executeGTxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool greater = _stack[stackSize - 2] > _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(greater)));
}

void ScriptExecution::executeLTxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool less = _stack[stackSize - 2] < _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(less)));
}

void ScriptExecution::executeLEQxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    bool le = _stack[stackSize - 2] <= _stack[stackSize - 1];

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(Variable::ofInt(static_cast<int>(le)));
}

void ScriptExecution::executeSHLEFTII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue << right.intValue));
}

void ScriptExecution::executeSHRIGHTII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue >> right.intValue));
}

void ScriptExecution::executeUSHRIGHTII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    // TODO: proper unsigned shift
    _stack.push_back(Variable::ofInt(left.intValue >> right.intValue));
}

void ScriptExecution::executeADDxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] + _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeSUBxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] - _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeMULxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] * _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeDIVxx(const Instruction &ins) {
    size_t stackSize = _stack.size();
    Variable result(_stack[stackSize - 2] / _stack[stackSize - 1]);

    _stack.pop_back();
    _stack.pop_back();
    _stack.push_back(move(result));
}

void ScriptExecution::executeMODII(const Instruction &ins) {
    Variable left, right;
    getTwoIntegersFromStack(left, right);

    _stack.push_back(Variable::ofInt(left.intValue % right.intValue));
}

void ScriptExecution::executeNEGI(const Instruction &ins) {
    _stack.back().intValue *= -1;
}

void ScriptExecution::executeNEGF(const Instruction &ins) {
    _stack.back().floatValue *= -1.0f;
}

void ScriptExecution::executeMOVSP(const Instruction &ins) {
    int count = -ins.stackOffset / 4;
    for (int i = 0; i < count; ++i) {
        _stack.pop_back();
    }
}

void ScriptExecution::executeJMP(const Instruction &ins) {
    _nextInstruction = ins.jumpOffset;
}

void ScriptExecution::executeJSR(const Instruction &ins) {
    _returnOffsets.push_back(ins.nextOffset);
    _nextInstruction = ins.jumpOffset;
}

void ScriptExecution::executeJZ(const Instruction &ins) {
    bool zero = _stack.back().intValue == 0;
    _stack.pop_back();

    if (zero) {
        _nextInstruction = ins.jumpOffset;
    }
}

void ScriptExecution::executeRETN(const Instruction &ins) {
    if (_returnOffsets.empty()) {
        _nextInstruction = _program->length();
    } else {
        _nextInstruction = _returnOffsets.back();
        _returnOffsets.pop_back();
    }
}

void ScriptExecution::executeDESTRUCT(const Instruction &ins) {
    int startIdx = static_cast<int>(_stack.size()) - ins.size / 4;
    int startIdxNoDestroy = startIdx + ins.stackOffset / 4;
    int countNoDestroy = ins.sizeNoDestroy / 4;

    for (int i = 0; i < countNoDestroy; ++i) {
        _stack[startIdx + i] = _stack[startIdxNoDestroy + i];
    }
    _stack.resize(startIdx + countNoDestroy);
}

void ScriptExecution::executeDECISP(const Instruction &ins) {
    int dstIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;
    _stack[dstIdx].intValue--;
}

void ScriptExecution::executeINCISP(const Instruction &ins) {
    int dstIdx = static_cast<int>(_stack.size()) + ins.stackOffset / 4;
    _stack[dstIdx].intValue++;
}

void ScriptExecution::executeNOTI(const Instruction &ins) {
    bool zero = _stack.back().intValue == 0;
    _stack.pop_back();

    _stack.push_back(Variable::ofInt(static_cast<int>(zero)));
}

void ScriptExecution::executeJNZ(const Instruction &ins) {
    bool zero = _stack.back().intValue == 0;
    _stack.pop_back();

    if (!zero) {
        _nextInstruction = ins.jumpOffset;
    }
}

void ScriptExecution::executeCPDOWNBP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = static_cast<int>(_stack.size()) - count;
    int dstIdx = _globalCount + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack[dstIdx++] = _stack[srcIdx++];
    }
}

void ScriptExecution::executeCPTOPBP(const Instruction &ins) {
    int count = ins.size / 4;
    int srcIdx = _globalCount + ins.stackOffset / 4;

    for (int i = 0; i < count; ++i) {
        _stack.push_back(_stack[srcIdx++]);
    }
}

void ScriptExecution::executeDECIBP(const Instruction &ins) {
    int dstIdx = _globalCount + ins.stackOffset / 4;
    _stack[dstIdx].intValue--;
}

void ScriptExecution::executeINCIBP(const Instruction &ins) {
    int dstIdx = _globalCount + ins.stackOffset / 4;
    _stack[dstIdx].intValue++;
}

void ScriptExecution::executeSAVEBP(const Instruction &ins) {
    _globalCount = static_cast<int>(_stack.size());
    _stack.push_back(Variable::ofInt(_globalCount));
}

void ScriptExecution::executeRESTOREBP(const Instruction &ins) {
    _globalCount = _stack.back().intValue;
    _stack.pop_back();
}

void ScriptExecution::executeSTORE_STATE(const Instruction &ins) {
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
    _savedState.insOffset = ins.offset + 0x10;
}

int ScriptExecution::getStackSize() const {
    return static_cast<int>(_stack.size());
}

const Variable &ScriptExecution::getStackVariable(int index) const {
    return _stack[index];
}

} // namespace script

} // namespace reone
