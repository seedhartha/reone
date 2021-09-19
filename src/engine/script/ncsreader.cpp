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

#include "ncsreader.h"

#include "../common/logutil.h"

#include "program.h"

using namespace std;

namespace reone {

namespace script {

NcsReader::NcsReader(const string &resRef) : BinaryReader(8, "NCS V1.0"), _resRef(resRef) {
    _endianess = boost::endian::order::big;
}

void NcsReader::doLoad() {
    uint8_t byteCode = readByte();
    uint32_t length = readUint32();

    _program = make_unique<ScriptProgram>(_resRef);
    _program->_length = length;

    size_t off = tell();
    while (off < length) {
        readInstruction(off);
    }
}

void NcsReader::readInstruction(size_t &offset) {
    seek(offset);

    uint8_t byteCode = readByte();
    uint8_t type = readByte();

    Instruction ins;
    ins.offset = static_cast<uint32_t>(offset);
    ins.byteCode = static_cast<ByteCode>(byteCode);
    ins.type = static_cast<InstructionType>(type);

    switch (ins.byteCode) {
        case ByteCode::CopyDownSP:
        case ByteCode::CopyTopSP:
        case ByteCode::CopyDownBP:
        case ByteCode::CopyTopBP:
            ins.stackOffset = readInt32();
            ins.size = readUint16();
            break;

        case ByteCode::PushConstant:
            switch (ins.type) {
                case InstructionType::Int:
                    ins.intValue = readInt32();
                    break;
                case InstructionType::Float:
                    ins.floatValue = readFloat();
                    break;
                case InstructionType::String: {
                    uint16_t len = readUint16();
                    ins.strValue = readCString(len);
                    break;
                }
                case InstructionType::Object:
                    ins.objectId = readInt32();
                    break;
                default:
                    throw runtime_error(str(boost::format("NCS: unsupported instruction type: %02x %02x") % static_cast<int>(ins.byteCode) % static_cast<int>(ins.type)));
            }
            break;

        case ByteCode::CallRoutine:
            ins.routine = readUint16();
            ins.argCount = readByte();
            break;

        case ByteCode::AdjustSP:
            ins.stackOffset = readInt32();
            break;

        case ByteCode::Jump:
        case ByteCode::JumpToSubroutine:
        case ByteCode::JumpIfZero:
        case ByteCode::JumpIfNonZero:
            ins.jumpOffset = static_cast<uint32_t>(offset + readInt32());
            break;

        case ByteCode::Destruct:
            ins.size = readUint16();
            ins.stackOffset = readInt16();
            ins.sizeNoDestroy = readUint16();
            break;

        case ByteCode::DecRelToSP:
        case ByteCode::IncRelToSP:
        case ByteCode::DecRelToBP:
        case ByteCode::IncRelToBP:
            ins.stackOffset = readInt32();
            break;

        case ByteCode::StoreState:
            ins.size = readUint32();
            ins.sizeLocals = readUint32();
            break;

        case ByteCode::Reserve:
        case ByteCode::LogicalAnd:
        case ByteCode::LogicalOr:
        case ByteCode::InclusiveBitwiseOr:
        case ByteCode::ExclusiveBitwiseOr:
        case ByteCode::BitwiseAnd:
        case ByteCode::Equal:
        case ByteCode::NotEqual:
        case ByteCode::GreaterThanOrEqual:
        case ByteCode::GreaterThan:
        case ByteCode::LessThan:
        case ByteCode::LessThanOrEqual:
        case ByteCode::ShiftLeft:
        case ByteCode::ShiftRight:
        case ByteCode::UnsignedShiftRight:
        case ByteCode::Add:
        case ByteCode::Subtract:
        case ByteCode::Multiply:
        case ByteCode::Divide:
        case ByteCode::Mod:
        case ByteCode::Negate:
        case ByteCode::OnesComplement:
        case ByteCode::Return:
        case ByteCode::SaveBP:
        case ByteCode::RestoreBP:
        case ByteCode::Noop:
        case ByteCode::LogicalNot:
            break;

        default:
            throw runtime_error(str(boost::format("NCS: unsupported byte code: %02x") % static_cast<int>(ins.byteCode)));
    }

    size_t pos = tell();
    ins.nextOffset = static_cast<uint32_t>(pos);

    _program->_instructions.insert(make_pair(static_cast<uint32_t>(offset), move(ins)));

    offset = pos;
}

} // namespace script

} // namespace reone
