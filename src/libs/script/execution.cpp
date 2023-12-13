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

#include "reone/script/execution.h"

#include "reone/script/executioncontext.h"
#include "reone/script/instrutil.h"
#include "reone/script/program.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"
#include "reone/script/variable.h"
#include "reone/system/logutil.h"

namespace reone {

namespace script {

static constexpr int kStartInstructionOffset = 13;
static constexpr float kFragmentloatTolerance = 1e-5;

ScriptExecution::ScriptExecution(std::shared_ptr<ScriptProgram> program, std::unique_ptr<ExecutionContext> context) :
    _context(std::move(context)),
    _program(std::move(program)) {

    static std::unordered_map<InstructionType, std::function<void(ScriptExecution *, const Instruction &)>> g_handlers {
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
        {InstructionType::EQUALII, &ScriptExecution::executeEQUALII},
        {InstructionType::EQUALFF, &ScriptExecution::executeEQUALFF},
        {InstructionType::EQUALSS, &ScriptExecution::executeEQUALSS},
        {InstructionType::EQUALOO, &ScriptExecution::executeEQUALOO},
        {InstructionType::EQUALTT, &ScriptExecution::executeEQUALTT},
        {InstructionType::EQUALEFFEFF, &ScriptExecution::executeEQUALEFFEFF},
        {InstructionType::EQUALEVTEVT, &ScriptExecution::executeEQUALEVTEVT},
        {InstructionType::EQUALLOCLOC, &ScriptExecution::executeEQUALLOCLOC},
        {InstructionType::EQUALTALTAL, &ScriptExecution::executeEQUALTALTAL},
        {InstructionType::NEQUALII, &ScriptExecution::executeNEQUALII},
        {InstructionType::NEQUALFF, &ScriptExecution::executeNEQUALFF},
        {InstructionType::NEQUALSS, &ScriptExecution::executeNEQUALSS},
        {InstructionType::NEQUALOO, &ScriptExecution::executeNEQUALOO},
        {InstructionType::NEQUALTT, &ScriptExecution::executeNEQUALTT},
        {InstructionType::NEQUALEFFEFF, &ScriptExecution::executeNEQUALEFFEFF},
        {InstructionType::NEQUALEVTEVT, &ScriptExecution::executeNEQUALEVTEVT},
        {InstructionType::NEQUALLOCLOC, &ScriptExecution::executeNEQUALLOCLOC},
        {InstructionType::NEQUALTALTAL, &ScriptExecution::executeNEQUALTALTAL},
        {InstructionType::GEQII, &ScriptExecution::executeGEQII},
        {InstructionType::GEQFF, &ScriptExecution::executeGEQFF},
        {InstructionType::GTII, &ScriptExecution::executeGTII},
        {InstructionType::GTFF, &ScriptExecution::executeGTFF},
        {InstructionType::LTII, &ScriptExecution::executeLTII},
        {InstructionType::LTFF, &ScriptExecution::executeLTFF},
        {InstructionType::LEQII, &ScriptExecution::executeLEQII},
        {InstructionType::LEQFF, &ScriptExecution::executeLEQFF},
        {InstructionType::SHLEFTII, &ScriptExecution::executeSHLEFTII},
        {InstructionType::SHRIGHTII, &ScriptExecution::executeSHRIGHTII},
        {InstructionType::USHRIGHTII, &ScriptExecution::executeUSHRIGHTII},
        {InstructionType::ADDII, &ScriptExecution::executeADDII},
        {InstructionType::ADDIF, &ScriptExecution::executeADDIF},
        {InstructionType::ADDFI, &ScriptExecution::executeADDFI},
        {InstructionType::ADDFF, &ScriptExecution::executeADDFF},
        {InstructionType::ADDSS, &ScriptExecution::executeADDSS},
        {InstructionType::ADDVV, &ScriptExecution::executeADDVV},
        {InstructionType::SUBII, &ScriptExecution::executeSUBII},
        {InstructionType::SUBIF, &ScriptExecution::executeSUBIF},
        {InstructionType::SUBFI, &ScriptExecution::executeSUBFI},
        {InstructionType::SUBFF, &ScriptExecution::executeSUBFF},
        {InstructionType::SUBVV, &ScriptExecution::executeSUBVV},
        {InstructionType::MULII, &ScriptExecution::executeMULII},
        {InstructionType::MULIF, &ScriptExecution::executeMULIF},
        {InstructionType::MULFI, &ScriptExecution::executeMULFI},
        {InstructionType::MULFF, &ScriptExecution::executeMULFF},
        {InstructionType::MULVF, &ScriptExecution::executeMULVF},
        {InstructionType::MULFV, &ScriptExecution::executeMULFV},
        {InstructionType::DIVII, &ScriptExecution::executeDIVII},
        {InstructionType::DIVIF, &ScriptExecution::executeDIVIF},
        {InstructionType::DIVFI, &ScriptExecution::executeDIVFI},
        {InstructionType::DIVFF, &ScriptExecution::executeDIVFF},
        {InstructionType::DIVVF, &ScriptExecution::executeDIVVF},
        {InstructionType::DIVFV, &ScriptExecution::executeDIVFV},
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
    _handlers.insert(std::make_pair(InstructionType::NOP, [](auto &) {}));
    _handlers.insert(std::make_pair(InstructionType::NOP2, [](auto &) {}));
}

int ScriptExecution::run() {
    uint32_t insOff = kStartInstructionOffset;

    if (_context->savedState) {
        std::vector<Variable> globals(_context->savedState->globals);
        copy(globals.begin(), globals.end(), back_inserter(_stack));
        _globalCount = static_cast<int>(_stack.size());

        std::vector<Variable> locals(_context->savedState->locals);
        copy(locals.begin(), locals.end(), back_inserter(_stack));

        insOff = _context->savedState->insOffset;
    }

    debug(str(boost::format("Run '%s': offset=%04x, caller=%u, triggerrer=%u") %
              _program->name() %
              insOff %
              _context->callerId %
              _context->triggererId),
          LogChannel::Script);

    while (insOff < _program->length()) {
        const Instruction &ins = _program->getInstruction(insOff);
        auto handler = _handlers.find(ins.type);

        if (handler == _handlers.end()) {
            error(str(boost::format("Instruction not implemented: %04x") % static_cast<int>(ins.type)), LogChannel::Script);
            return -1;
        }
        _nextInstruction = ins.nextOffset;

        if (isLogChannelEnabled(LogChannel::Script3)) {
            debug(str(boost::format("Instruction: %s") % describeInstruction(ins, *_context->routines)), LogChannel::Script3);
        }
        try {
            handler->second(ins);
        } catch (const std::exception &ex) {
            debug(str(boost::format("Halt '%s'") % _program->name()), LogChannel::Script);
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
    auto &routine = _context->routines->get(ins.routine);
    if (ins.argCount > routine.getArgumentCount()) {
        throw std::invalid_argument("Too many routine arguments");
    }

    std::vector<Variable> args;
    for (int i = 0; i < ins.argCount; ++i) {
        VariableType type = routine.getArgumentType(i);
        switch (type) {
        case VariableType::Vector:
            args.push_back(Variable::ofVector(getVectorFromStack()));
            break;

        case VariableType::Action: {
            auto ctx = std::make_shared<ExecutionContext>(*_context);
            ctx->savedState = std::make_shared<ExecutionState>(_savedState);
            args.push_back(Variable::ofAction(std::move(ctx)));
            break;
        }
        default:
            Variable var(_stack.back());
            if (var.type != type) {
                throw std::runtime_error("Invalid argument variable type");
            }
            args.push_back(std::move(var));
            _stack.pop_back();
            break;
        }
    }

    Variable retValue = routine.invoke(args, *_context);
    if (isLogChannelEnabled(LogChannel::Script2)) {
        std::vector<std::string> argStrings;
        for (auto &arg : args) {
            argStrings.push_back(arg.toString());
        }
        std::string argsString(boost::join(argStrings, ", "));
        debug(str(boost::format("Action: %04x %s(%s) -> %s") % ins.offset % routine.name() % argsString % retValue.toString()), LogChannel::Script2);
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

void ScriptExecution::executeLOGANDII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left && right)));
    });
}

void ScriptExecution::executeLOGORII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left || right)));
    });
}

void ScriptExecution::executeINCORII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left | right));
    });
}

void ScriptExecution::executeEXCORII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left ^ right));
    });
}

void ScriptExecution::executeBOOLANDII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left & right));
    });
}

void ScriptExecution::executeEQUALII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left == right)));
    });
}

void ScriptExecution::executeEQUALFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(fabs(left - right) < kFragmentloatTolerance)));
    });
}

void ScriptExecution::executeEQUALSS(const Instruction &ins) {
    withStringsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left == right)));
    });
}

void ScriptExecution::executeEQUALOO(const Instruction &ins) {
    withObjectsFromStack([this](uint32_t left, uint32_t right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left == right)));
    });
}

void ScriptExecution::executeEQUALTT(const Instruction &ins) {
    int numVariables = ins.size / 4;
    std::vector<Variable> vars1;
    for (int i = 0; i < numVariables; ++i) {
        vars1.push_back(std::move(_stack.back()));
        _stack.pop_back();
    }
    std::vector<Variable> vars2;
    for (int i = 0; i < numVariables; ++i) {
        vars2.push_back(std::move(_stack.back()));
        _stack.pop_back();
    }
    bool equal = std::equal(vars1.begin(), vars1.end(), vars2.begin());
    _stack.push_back(Variable::ofInt(static_cast<int>(equal)));
}

void ScriptExecution::executeEQUALEFFEFF(const Instruction &ins) {
    withEffectsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left == right)));
    });
}

void ScriptExecution::executeEQUALEVTEVT(const Instruction &ins) {
    withEventsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left == right)));
    });
}

void ScriptExecution::executeEQUALLOCLOC(const Instruction &ins) {
    withLocationsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left == right)));
    });
}

void ScriptExecution::executeEQUALTALTAL(const Instruction &ins) {
    withTalentsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left == right)));
    });
}

void ScriptExecution::executeNEQUALII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeNEQUALFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeNEQUALSS(const Instruction &ins) {
    withStringsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeNEQUALOO(const Instruction &ins) {
    withObjectsFromStack([this](uint32_t left, uint32_t right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeNEQUALTT(const Instruction &ins) {
    int numVariables = ins.size / 4;
    std::vector<Variable> vars1;
    for (int i = 0; i < numVariables; ++i) {
        vars1.push_back(std::move(_stack.back()));
        _stack.pop_back();
    }
    std::vector<Variable> vars2;
    for (int i = 0; i < numVariables; ++i) {
        vars2.push_back(std::move(_stack.back()));
        _stack.pop_back();
    }
    bool notEqual = !std::equal(vars1.begin(), vars1.end(), vars2.begin());
    _stack.push_back(Variable::ofInt(static_cast<int>(notEqual)));
}

void ScriptExecution::executeNEQUALEFFEFF(const Instruction &ins) {
    withEffectsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeNEQUALEVTEVT(const Instruction &ins) {
    withEventsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeNEQUALLOCLOC(const Instruction &ins) {
    withLocationsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeNEQUALTALTAL(const Instruction &ins) {
    withTalentsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left != right)));
    });
}

void ScriptExecution::executeGEQII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left >= right)));
    });
}

void ScriptExecution::executeGEQFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left >= right)));
    });
}

void ScriptExecution::executeGTII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left > right)));
    });
}

void ScriptExecution::executeGTFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left > right)));
    });
}

void ScriptExecution::executeLTII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left < right)));
    });
}

void ScriptExecution::executeLTFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left < right)));
    });
}

void ScriptExecution::executeLEQII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left <= right)));
    });
}

void ScriptExecution::executeLEQFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofInt(static_cast<int>(left <= right)));
    });
}

void ScriptExecution::executeSHLEFTII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left << right));
    });
}

void ScriptExecution::executeSHRIGHTII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        int result = left;
        if (result < 0) {
            result = -((-result) >> right);
        } else {
            result >>= right;
        }
        _stack.push_back(Variable::ofInt(result));
    });
}

void ScriptExecution::executeUSHRIGHTII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(static_cast<unsigned int>(left) >> right));
    });
}

void ScriptExecution::executeADDII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left + right));
    });
}

void ScriptExecution::executeADDIF(const Instruction &ins) {
    withIntFloatFromStack([this](int left, float right) {
        _stack.push_back(Variable::ofFloat(left + right));
    });
}

void ScriptExecution::executeADDFI(const Instruction &ins) {
    withFloatIntFromStack([this](float left, int right) {
        _stack.push_back(Variable::ofFloat(left + right));
    });
}

void ScriptExecution::executeADDFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofFloat(left + right));
    });
}

void ScriptExecution::executeADDSS(const Instruction &ins) {
    withStringsFromStack([this](auto &left, auto &right) {
        _stack.push_back(Variable::ofString(left + right));
    });
}

void ScriptExecution::executeADDVV(const Instruction &ins) {
    withVectorsFromStack([this](auto &left, auto &right) {
        auto result = left + right;
        _stack.push_back(Variable::ofFloat(result.x));
        _stack.push_back(Variable::ofFloat(result.y));
        _stack.push_back(Variable::ofFloat(result.z));
    });
}

void ScriptExecution::executeSUBII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left - right));
    });
}

void ScriptExecution::executeSUBIF(const Instruction &ins) {
    withIntFloatFromStack([this](int left, float right) {
        _stack.push_back(Variable::ofFloat(left - right));
    });
}

void ScriptExecution::executeSUBFI(const Instruction &ins) {
    withFloatIntFromStack([this](float left, int right) {
        _stack.push_back(Variable::ofFloat(left - right));
    });
}

void ScriptExecution::executeSUBFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofFloat(left - right));
    });
}

void ScriptExecution::executeSUBVV(const Instruction &ins) {
    withVectorsFromStack([this](auto &left, auto &right) {
        auto result = left - right;
        _stack.push_back(Variable::ofFloat(result.x));
        _stack.push_back(Variable::ofFloat(result.y));
        _stack.push_back(Variable::ofFloat(result.z));
    });
}

void ScriptExecution::executeMULII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left * right));
    });
}

void ScriptExecution::executeMULIF(const Instruction &ins) {
    withIntFloatFromStack([this](int left, float right) {
        _stack.push_back(Variable::ofFloat(left * right));
    });
}

void ScriptExecution::executeMULFI(const Instruction &ins) {
    withFloatIntFromStack([this](float left, int right) {
        _stack.push_back(Variable::ofFloat(left * right));
    });
}

void ScriptExecution::executeMULFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofFloat(left * right));
    });
}

void ScriptExecution::executeMULVF(const Instruction &ins) {
    withVectorFloatFromStack([this](auto &left, float right) {
        auto result = left * right;
        _stack.push_back(Variable::ofFloat(result.x));
        _stack.push_back(Variable::ofFloat(result.y));
        _stack.push_back(Variable::ofFloat(result.z));
    });
}

void ScriptExecution::executeMULFV(const Instruction &ins) {
    withFloatVectorFromStack([this](float left, auto &right) {
        auto result = left * right;
        _stack.push_back(Variable::ofFloat(result.x));
        _stack.push_back(Variable::ofFloat(result.y));
        _stack.push_back(Variable::ofFloat(result.z));
    });
}

void ScriptExecution::executeDIVII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left / right));
    });
}

void ScriptExecution::executeDIVIF(const Instruction &ins) {
    withIntFloatFromStack([this](int left, float right) {
        _stack.push_back(Variable::ofFloat(left / std::max(kFragmentloatTolerance, right)));
    });
}

void ScriptExecution::executeDIVFI(const Instruction &ins) {
    withFloatIntFromStack([this](float left, int right) {
        _stack.push_back(Variable::ofFloat(left / right));
    });
}

void ScriptExecution::executeDIVFF(const Instruction &ins) {
    withFloatsFromStack([this](float left, float right) {
        _stack.push_back(Variable::ofFloat(left / std::max(kFragmentloatTolerance, right)));
    });
}

void ScriptExecution::executeDIVVF(const Instruction &ins) {
    withVectorFloatFromStack([this](auto &left, float right) {
        auto result = left / right;
        _stack.push_back(Variable::ofFloat(result.x));
        _stack.push_back(Variable::ofFloat(result.y));
        _stack.push_back(Variable::ofFloat(result.z));
    });
}

void ScriptExecution::executeDIVFV(const Instruction &ins) {
    withFloatVectorFromStack([this](float left, auto &right) {
        auto result = left / right;
        _stack.push_back(Variable::ofFloat(result.x));
        _stack.push_back(Variable::ofFloat(result.y));
        _stack.push_back(Variable::ofFloat(result.z));
    });
}

void ScriptExecution::executeMODII(const Instruction &ins) {
    withIntsFromStack([this](int left, int right) {
        _stack.push_back(Variable::ofInt(left % right));
    });
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
    _nextInstruction = ins.offset + ins.jumpOffset;
}

void ScriptExecution::executeJSR(const Instruction &ins) {
    _returnOffsets.push_back(ins.nextOffset);
    _nextInstruction = ins.offset + ins.jumpOffset;
}

void ScriptExecution::executeJZ(const Instruction &ins) {
    bool zero = getIntFromStack() == 0;
    if (zero) {
        _nextInstruction = ins.offset + ins.jumpOffset;
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
    int value = getIntFromStack();
    _stack.push_back(Variable::ofInt(static_cast<int>(!value)));
}

void ScriptExecution::executeJNZ(const Instruction &ins) {
    bool notZero = getIntFromStack() != 0;
    if (notZero) {
        _nextInstruction = ins.offset + ins.jumpOffset;
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
    _globalCount = getIntFromStack();
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

int ScriptExecution::getIntFromStack() {
    Variable var(std::move(_stack.back()));
    _stack.pop_back();

    throwIfInvalidType(VariableType::Int, var.type);

    return var.intValue;
}

float ScriptExecution::getFloatFromStack() {
    Variable var(std::move(_stack.back()));
    _stack.pop_back();

    throwIfInvalidType(VariableType::Float, var.type);

    return var.floatValue;
}

glm::vec3 ScriptExecution::getVectorFromStack() {
    float z = getFloatFromStack();
    float y = getFloatFromStack();
    float x = getFloatFromStack();

    return glm::vec3(x, y, z);
}

void ScriptExecution::withStackVertexariables(const std::function<void(const Variable &, const Variable &)> &fn) {
    Variable second(std::move(_stack.back()));
    _stack.pop_back();

    Variable first(std::move(_stack.back()));
    _stack.pop_back();

    fn(first, second);
}

void ScriptExecution::withIntsFromStack(const std::function<void(int, int)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Int, left.type);
        throwIfInvalidType(VariableType::Int, right.type);
        fn(left.intValue, right.intValue);
    });
}

void ScriptExecution::withIntFloatFromStack(const std::function<void(int, float)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Int, left.type);
        throwIfInvalidType(VariableType::Float, right.type);
        fn(left.intValue, right.floatValue);
    });
}

void ScriptExecution::withFloatIntFromStack(const std::function<void(float, int)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Float, left.type);
        throwIfInvalidType(VariableType::Int, right.type);
        fn(left.floatValue, right.intValue);
    });
}

void ScriptExecution::withFloatsFromStack(const std::function<void(float, float)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Float, left.type);
        throwIfInvalidType(VariableType::Float, right.type);
        fn(left.floatValue, right.floatValue);
    });
}

void ScriptExecution::withStringsFromStack(const std::function<void(const std::string &, const std::string &)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::String, left.type);
        throwIfInvalidType(VariableType::String, right.type);
        fn(left.strValue, right.strValue);
    });
}

void ScriptExecution::withObjectsFromStack(const std::function<void(uint32_t, uint32_t)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Object, left.type);
        throwIfInvalidType(VariableType::Object, right.type);
        fn(left.objectId, right.objectId);
    });
}

void ScriptExecution::withEffectsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Effect, left.type);
        throwIfInvalidType(VariableType::Effect, right.type);
        fn(left.engineType, right.engineType);
    });
}

void ScriptExecution::withEventsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Event, left.type);
        throwIfInvalidType(VariableType::Event, right.type);
        fn(left.engineType, right.engineType);
    });
}

void ScriptExecution::withLocationsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Location, left.type);
        throwIfInvalidType(VariableType::Location, right.type);
        fn(left.engineType, right.engineType);
    });
}

void ScriptExecution::withTalentsFromStack(const std::function<void(const std::shared_ptr<EngineType> &, const std::shared_ptr<EngineType> &)> &fn) {
    withStackVertexariables([this, &fn](auto &left, auto &right) {
        throwIfInvalidType(VariableType::Talent, left.type);
        throwIfInvalidType(VariableType::Talent, right.type);
        fn(left.engineType, right.engineType);
    });
}

void ScriptExecution::withFloatVectorFromStack(const std::function<void(float, const glm::vec3 &)> &fn) {
    auto right = getVectorFromStack();
    auto left = getFloatFromStack();

    fn(left, right);
}

void ScriptExecution::withVectorFloatFromStack(const std::function<void(const glm::vec3 &, float)> &fn) {
    auto right = getFloatFromStack();
    auto left = getVectorFromStack();

    fn(left, right);
}

void ScriptExecution::withVectorsFromStack(const std::function<void(const glm::vec3 &, const glm::vec3 &)> &fn) {
    auto right = getVectorFromStack();
    auto left = getVectorFromStack();

    fn(left, right);
}

void ScriptExecution::throwIfInvalidType(VariableType expected, VariableType actual) {
    if (actual != expected) {
        throw std::runtime_error(str(boost::format("Invalid variable type: expected=%d, actual=%d") %
                                     static_cast<int>(expected) %
                                     static_cast<int>(actual)));
    }
}

int ScriptExecution::getStackSize() const {
    return static_cast<int>(_stack.size());
}

const Variable &ScriptExecution::getStackVertexariable(int index) const {
    return _stack[index];
}

} // namespace script

} // namespace reone
