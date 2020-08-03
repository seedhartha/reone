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

#include "program.h"

#include <stdexcept>

#include <boost/format.hpp>

using namespace std;

namespace reone {

namespace script {

ScriptProgram::ScriptProgram(const string &name) : _name(name) {
}

const string &ScriptProgram::name() const {
    return _name;
}

uint32_t ScriptProgram::length() const {
    return _length;
}

const Instruction &ScriptProgram::getInstruction(uint32_t offset) const {
    return _instructions.find(offset)->second;
}

static map<ByteCode, string> g_codeDesc = {
    { ByteCode::CopyDownSP, "CopyDownSP" },
    { ByteCode::Reserve, "Reserve" },
    { ByteCode::CopyTopSP, "CopyTopSP" },
    { ByteCode::PushConstant, "PushConstant" },
    { ByteCode::CallRoutine, "CallRoutine" },
    { ByteCode::LogicalAnd, "LogicalAnd" },
    { ByteCode::LogicalOr, "LogicalOr" },
    { ByteCode::InclusiveBitwiseOr, "InclusiveBitwiseOr" },
    { ByteCode::ExclusiveBitwiseOr, "ExclusiveBitwiseOr" },
    { ByteCode::BitwiseAnd, "BitwiseAnd" },
    { ByteCode::Equal, "Equal" },
    { ByteCode::NotEqual, "NotEqual" },
    { ByteCode::GreaterThanOrEqual, "GreaterThanOrEqual" },
    { ByteCode::GreaterThan, "GreaterThan" },
    { ByteCode::LessThan, "LessThan" },
    { ByteCode::LessThanOrEqual, "LessThanOrEqual" },
    { ByteCode::ShiftLeft, "ShiftLeft" },
    { ByteCode::ShiftRight, "ShiftRight" },
    { ByteCode::UnsignedShiftRight, "UnsignedShiftRight" },
    { ByteCode::Add, "Add" },
    { ByteCode::Subtract, "Subtract" },
    { ByteCode::Multiply, "Multiply" },
    { ByteCode::Divide, "Divide" },
    { ByteCode::Mod, "Mod" },
    { ByteCode::Negate, "Negate" },
    { ByteCode::AdjustSP, "AdjustSP" },
    { ByteCode::Jump, "Jump" },
    { ByteCode::JumpToSubroutine, "JumpToSubroutine" },
    { ByteCode::JumpIfZero, "JumpIfZero" },
    { ByteCode::Return, "Return" },
    { ByteCode::Destruct, "Destruct" },
    { ByteCode::LogicalNot, "LogicalNot" },
    { ByteCode::DecRelToSP, "DecRelToSP" },
    { ByteCode::IncRelToSP, "IncRelToSP" },
    { ByteCode::JumpIfNonZero, "JumpIfNonZero" },
    { ByteCode::CopyDownBP, "CopyDownBP" },
    { ByteCode::CopyTopBP, "CopyTopBP" },
    { ByteCode::DecRelToBP, "DecRelToBP" },
    { ByteCode::IncRelToBP, "IncRelToBP" },
    { ByteCode::SaveBP, "SaveBP" },
    { ByteCode::RestoreBP, "RestoreBP" },
    { ByteCode::StoreState, "StoreState" },
    { ByteCode::Noop, "Noop" },
    { ByteCode::Invalid, "Invalid" }
};

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
