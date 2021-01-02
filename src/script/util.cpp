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

#include "util.h"

#include <map>

#include <boost/format.hpp>

#include "program.h"

using namespace std;

namespace reone {

namespace script {

static map<ByteCode, string> g_codeDesc = {
    { ByteCode::CopyDownSP, "CPDOWNSP" },
    { ByteCode::Reserve, "RSADDx" },
    { ByteCode::CopyTopSP, "CPTOPSP" },
    { ByteCode::PushConstant, "CONSTx" },
    { ByteCode::CallRoutine, "ACTION" },
    { ByteCode::LogicalAnd, "LOGANDII" },
    { ByteCode::LogicalOr, "LOGORII" },
    { ByteCode::InclusiveBitwiseOr, "INCORII" },
    { ByteCode::ExclusiveBitwiseOr, "EXCORII" },
    { ByteCode::BitwiseAnd, "BOOLANDII" },
    { ByteCode::Equal, "EQUALxx" },
    { ByteCode::NotEqual, "NEQUALxx" },
    { ByteCode::GreaterThanOrEqual, "GEQxx" },
    { ByteCode::GreaterThan, "GTxx" },
    { ByteCode::LessThan, "LTxx" },
    { ByteCode::LessThanOrEqual, "LEQxx" },
    { ByteCode::ShiftLeft, "SHLEFTII" },
    { ByteCode::ShiftRight, "SHRIGHTII" },
    { ByteCode::UnsignedShiftRight, "USHRIGHTII" },
    { ByteCode::Add, "ADDxx" },
    { ByteCode::Subtract, "SUBxx" },
    { ByteCode::Multiply, "MULxx" },
    { ByteCode::Divide, "DIVxx" },
    { ByteCode::Mod, "MODII" },
    { ByteCode::Negate, "NEGx" },
    { ByteCode::OnesComplement, "COMPI" },
    { ByteCode::AdjustSP, "MOVESP" },
    { ByteCode::Jump, "JMP" },
    { ByteCode::JumpToSubroutine, "JSR" },
    { ByteCode::JumpIfZero, "JZ" },
    { ByteCode::Return, "RETN" },
    { ByteCode::Destruct, "DESTRUCT" },
    { ByteCode::LogicalNot, "NOTI" },
    { ByteCode::DecRelToSP, "DECISP" },
    { ByteCode::IncRelToSP, "INCISP" },
    { ByteCode::JumpIfNonZero, "JNZ" },
    { ByteCode::CopyDownBP, "CPDOWNBP" },
    { ByteCode::CopyTopBP, "CPTOPBP" },
    { ByteCode::DecRelToBP, "DECIBP" },
    { ByteCode::IncRelToBP, "INCIBP" },
    { ByteCode::SaveBP, "SAVEBP" },
    { ByteCode::RestoreBP, "RESTOREBP" },
    { ByteCode::StoreState, "STORE_STATE" },
    { ByteCode::Noop, "NOP" },
    { ByteCode::Invalid, "[invalid]" }
};

string describeInstruction(const Instruction &ins) {
    const string &byteCodeDesc = describeByteCode(ins.byteCode);
    string desc(byteCodeDesc);

    switch (ins.byteCode) {
        case ByteCode::Jump:
        case ByteCode::JumpToSubroutine:
        case ByteCode::JumpIfZero:
        case ByteCode::JumpIfNonZero:
            desc += str(boost::format(" %08x") % ins.jumpOffset);
            break;

        case ByteCode::PushConstant:
            switch (ins.type) {
                case InstructionType::Int:
                    desc += " " + to_string(ins.intValue);
                    break;
                case InstructionType::Float:
                    desc += " " + to_string(ins.floatValue);
                    break;
                case InstructionType::String:
                    desc += " \"" + ins.strValue + "\"";
                    break;
                case InstructionType::Object:
                    desc += " " + to_string(ins.objectId);
                    break;
                default:
                    desc += "[not implemented]";
                    break;
            }
            break;

        case ByteCode::CallRoutine:
            desc += str(boost::format(" %d %d") % ins.routine % ins.argCount);
            break;

        default:
            break;
    }

    return move(desc);
}

const string &describeByteCode(ByteCode code) {
    auto desc = g_codeDesc.find(code);
    if (desc == g_codeDesc.end()) {
        string newDesc(str(boost::format("%02x") % static_cast<int>(code)));
        auto pair = g_codeDesc.insert(make_pair(code, newDesc));

        return pair.first->second;
    }

    return desc->second;
}

} // namespace script

} // namespace reone
