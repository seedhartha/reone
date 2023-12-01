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

#include "reone/resource/format/ncsreader.h"

#include "reone/resource/format/signutil.h"
#include "reone/script/program.h"
#include "reone/system/logutil.h"

using namespace reone::script;

namespace reone {

namespace resource {

void NcsReader::load() {
    checkSignature(_ncs, std::string("NCS V1.0", 8));

    uint8_t byteCode = _ncs.readByte();
    uint32_t length = _ncs.readUint32();

    _program = std::make_unique<ScriptProgram>(_resRef);

    size_t off = _ncs.position();
    while (off < length) {
        readInstruction(off);
    }
}

void NcsReader::readInstruction(size_t &offset) {
    _ncs.seek(offset);

    uint8_t byteCode = _ncs.readByte();
    uint8_t qualifier = _ncs.readByte();

    Instruction ins;
    ins.offset = static_cast<uint32_t>(offset);
    ins.type = R_INSTR_TYPE(byteCode, qualifier);

    switch (ins.type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
        ins.stackOffset = _ncs.readInt32();
        ins.size = _ncs.readUint16();
        break;
    case InstructionType::CONSTI:
        ins.intValue = _ncs.readInt32();
        break;
    case InstructionType::CONSTF:
        ins.floatValue = _ncs.readFloat();
        break;
    case InstructionType::CONSTS: {
        uint16_t len = _ncs.readUint16();
        ins.strValue = _ncs.readString(len);
        break;
    }
    case InstructionType::CONSTO:
        ins.objectId = _ncs.readInt32();
        break;
    case InstructionType::ACTION:
        ins.routine = _ncs.readUint16();
        ins.argCount = _ncs.readByte();
        break;
    case InstructionType::MOVSP:
        ins.stackOffset = _ncs.readInt32();
        break;
    case InstructionType::JMP:
    case InstructionType::JSR:
    case InstructionType::JZ:
    case InstructionType::JNZ:
        ins.jumpOffset = _ncs.readInt32();
        break;
    case InstructionType::DESTRUCT:
        ins.size = _ncs.readUint16();
        ins.stackOffset = _ncs.readInt16();
        ins.sizeNoDestroy = _ncs.readUint16();
        break;
    case InstructionType::DECISP:
    case InstructionType::INCISP:
    case InstructionType::DECIBP:
    case InstructionType::INCIBP:
        ins.stackOffset = _ncs.readInt32();
        break;
    case InstructionType::STORE_STATE:
        ins.size = _ncs.readUint32();
        ins.sizeLocals = _ncs.readUint32();
        break;
    case InstructionType::EQUALTT:
    case InstructionType::NEQUALTT:
        ins.size = _ncs.readUint16();
        break;
    case InstructionType::NOP:
    case InstructionType::RSADDI:
    case InstructionType::RSADDF:
    case InstructionType::RSADDS:
    case InstructionType::RSADDO:
    case InstructionType::RSADDEFF:
    case InstructionType::RSADDEVT:
    case InstructionType::RSADDLOC:
    case InstructionType::RSADDTAL:
    case InstructionType::LOGANDII:
    case InstructionType::LOGORII:
    case InstructionType::INCORII:
    case InstructionType::EXCORII:
    case InstructionType::BOOLANDII:
    case InstructionType::EQUALII:
    case InstructionType::EQUALFF:
    case InstructionType::EQUALSS:
    case InstructionType::EQUALOO:
    case InstructionType::EQUALEFFEFF:
    case InstructionType::EQUALEVTEVT:
    case InstructionType::EQUALLOCLOC:
    case InstructionType::EQUALTALTAL:
    case InstructionType::NEQUALII:
    case InstructionType::NEQUALFF:
    case InstructionType::NEQUALSS:
    case InstructionType::NEQUALOO:
    case InstructionType::NEQUALEFFEFF:
    case InstructionType::NEQUALEVTEVT:
    case InstructionType::NEQUALLOCLOC:
    case InstructionType::NEQUALTALTAL:
    case InstructionType::GEQII:
    case InstructionType::GEQFF:
    case InstructionType::GTII:
    case InstructionType::GTFF:
    case InstructionType::LTII:
    case InstructionType::LTFF:
    case InstructionType::LEQII:
    case InstructionType::LEQFF:
    case InstructionType::SHLEFTII:
    case InstructionType::SHRIGHTII:
    case InstructionType::USHRIGHTII:
    case InstructionType::ADDII:
    case InstructionType::ADDIF:
    case InstructionType::ADDFI:
    case InstructionType::ADDFF:
    case InstructionType::ADDSS:
    case InstructionType::ADDVV:
    case InstructionType::SUBII:
    case InstructionType::SUBIF:
    case InstructionType::SUBFI:
    case InstructionType::SUBFF:
    case InstructionType::SUBVV:
    case InstructionType::MULII:
    case InstructionType::MULIF:
    case InstructionType::MULFI:
    case InstructionType::MULFF:
    case InstructionType::MULVF:
    case InstructionType::MULFV:
    case InstructionType::DIVII:
    case InstructionType::DIVIF:
    case InstructionType::DIVFI:
    case InstructionType::DIVFF:
    case InstructionType::DIVVF:
    case InstructionType::DIVFV:
    case InstructionType::MODII:
    case InstructionType::NEGI:
    case InstructionType::NEGF:
    case InstructionType::COMPI:
    case InstructionType::RETN:
    case InstructionType::NOTI:
    case InstructionType::SAVEBP:
    case InstructionType::RESTOREBP:
    case InstructionType::NOP2:
        break;
    default:
        throw std::runtime_error(str(boost::format("Unsupported instruction type: %04x") % static_cast<int>(ins.type)));
    }

    size_t pos = _ncs.position();
    ins.nextOffset = static_cast<uint32_t>(pos);

    _program->add(std::move(ins));

    offset = pos;
}

} // namespace resource

} // namespace reone
