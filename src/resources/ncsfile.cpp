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

#include "ncsfile.h"

#include "../core/log.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace resources {

NcsFile::NcsFile(const string &resRef) : BinaryFile(8, "NCS V1.0"), _resRef(resRef) {
}

void NcsFile::doLoad() {
    uint8_t byteCode = readByte();
    assert(byteCode == 0x42);

    uint32_t length = readUint32BE();
    assert(_size >= length);

    _program = make_unique<ScriptProgram>(_resRef);
    _program->_length = length;

    uint32_t off = tell();
    while (off < length) {
        readInstruction(off);
    }
}

void NcsFile::readInstruction(uint32_t &offset) {
    seek(offset);

    uint8_t byteCode = readByte();
    uint8_t type = readByte();

    Instruction ins;
    ins.offset = offset;
    ins.byteCode = static_cast<ByteCode>(byteCode);
    ins.type = static_cast<InstructionType>(type);

    switch (ins.byteCode) {
        case ByteCode::CopyDownSP:
        case ByteCode::CopyTopSP:
        case ByteCode::CopyDownBP:
        case ByteCode::CopyTopBP:
            assert(ins.type == InstructionType::One);
            ins.stackOffset = readInt32BE();
            ins.size = readUint16BE();
            break;

        case ByteCode::Reserve:
            assert(
                ins.type == InstructionType::Int ||
                ins.type == InstructionType::Float ||
                ins.type == InstructionType::String ||
                ins.type == InstructionType::Object ||
                ins.type == InstructionType::Effect ||
                ins.type == InstructionType::Event ||
                ins.type == InstructionType::Location ||
                ins.type == InstructionType::Talent);

            break;

        case ByteCode::PushConstant:
            switch (ins.type) {
                case InstructionType::Int:
                    ins.intValue = readInt32BE();
                    break;
                case InstructionType::Float:
                    ins.floatValue = readFloatBE();
                    break;
                case InstructionType::String: {
                    uint16_t len = readUint16BE();
                    ins.strValue = readFixedString(len);
                    break;
                }
                case InstructionType::Object:
                    ins.objectId = readInt32BE();
                    break;
                default:
                    throw runtime_error(str(boost::format("NCS: unsupported instruction type: %02x %02x") % static_cast<int>(ins.byteCode) % static_cast<int>(ins.type)));
            }
            break;

        case ByteCode::CallRoutine:
            assert(ins.type == InstructionType::None);
            ins.routine = readUint16BE();
            ins.argCount = readByte();
            break;

        case ByteCode::LogicalAnd:
        case ByteCode::LogicalOr:
        case ByteCode::InclusiveBitwiseOr:
        case ByteCode::ExclusiveBitwiseOr:
        case ByteCode::BitwiseAnd:
            assert(ins.type == InstructionType::IntInt);
            break;

        case ByteCode::Equal:
        case ByteCode::NotEqual:
            assert(
                ins.type == InstructionType::IntInt ||
                ins.type == InstructionType::FloatFloat ||
                ins.type == InstructionType::ObjectObject ||
                ins.type == InstructionType::StringString ||
                ins.type == InstructionType::EffectEffect ||
                ins.type == InstructionType::EventEvent ||
                ins.type == InstructionType::LocationLocation ||
                ins.type == InstructionType::TalentTalent);

            break;

        case ByteCode::GreaterThanOrEqual:
        case ByteCode::GreaterThan:
        case ByteCode::LessThan:
        case ByteCode::LessThanOrEqual:
            assert(ins.type == InstructionType::IntInt || ins.type == InstructionType::FloatFloat);
            break;

        case ByteCode::ShiftLeft:
        case ByteCode::ShiftRight:
        case ByteCode::UnsignedShiftRight:
            assert(ins.type == InstructionType::IntInt);
            break;

        case ByteCode::Add:
            assert(
                ins.type == InstructionType::IntInt ||
                ins.type == InstructionType::FloatFloat ||
                ins.type == InstructionType::StringString ||
                ins.type == InstructionType::IntFloat ||
                ins.type == InstructionType::FloatInt ||
                ins.type == InstructionType::VectorVector);

            break;

        case ByteCode::Subtract:
            assert(
                ins.type == InstructionType::IntInt ||
                ins.type == InstructionType::FloatFloat ||
                ins.type == InstructionType::IntFloat ||
                ins.type == InstructionType::FloatInt ||
                ins.type == InstructionType::VectorVector);

            break;

        case ByteCode::Multiply:
            assert(
                ins.type == InstructionType::IntInt ||
                ins.type == InstructionType::FloatFloat ||
                ins.type == InstructionType::IntFloat ||
                ins.type == InstructionType::FloatInt ||
                ins.type == InstructionType::VectorFloat ||
                ins.type == InstructionType::FloatVector);

            break;

        case ByteCode::Divide:
            assert(
                ins.type == InstructionType::IntInt ||
                ins.type == InstructionType::FloatFloat ||
                ins.type == InstructionType::IntFloat ||
                ins.type == InstructionType::FloatInt ||
                ins.type == InstructionType::VectorFloat);

            break;

        case ByteCode::Mod:
            assert(ins.type == InstructionType::IntInt);
            break;

        case ByteCode::Negate:
            assert(ins.type == InstructionType::Int || ins.type == InstructionType::Float);
            break;

        case ByteCode::AdjustSP:
            assert(ins.type == InstructionType::None);
            ins.stackOffset = readInt32BE();
            break;

        case ByteCode::Jump:
        case ByteCode::JumpToSubroutine:
        case ByteCode::JumpIfZero:
        case ByteCode::JumpIfNonZero:
            assert(ins.type == InstructionType::None);
            ins.jumpOffset = offset + readInt32BE();
            break;

        case ByteCode::Return:
        case ByteCode::SaveBP:
        case ByteCode::RestoreBP:
        case ByteCode::Noop:
            assert(ins.type == InstructionType::None);
            break;

        case ByteCode::Destruct:
            assert(ins.type == InstructionType::One);
            ins.size = readUint16BE();
            ins.stackOffset = readInt16BE();
            ins.sizeNoDestroy = readUint16BE();
            break;

        case ByteCode::LogicalNot:
            assert(ins.type == InstructionType::Int);
            break;

        case ByteCode::DecRelToSP:
        case ByteCode::IncRelToSP:
        case ByteCode::DecRelToBP:
        case ByteCode::IncRelToBP:
            assert(ins.type == InstructionType::Int);
            ins.stackOffset = readInt32BE();
            break;

        case ByteCode::StoreState:
            assert(type == 0x10);
            ins.size = readUint32BE();
            ins.sizeLocals = readUint32BE();
            break;

        default:
            throw runtime_error(str(boost::format("NCS: unsupported byte code: %02x") % static_cast<int>(ins.byteCode)));
    }

    uint32_t pos = tell();
    ins.nextOffset = pos;

    _program->_instructions.insert(make_pair(offset, move(ins)));

    offset = pos;
}

shared_ptr<ScriptProgram> NcsFile::program() const {
    return _program;
}

} // namespace resources

} // namespace reone
