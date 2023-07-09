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

#include "reone/tools/script/format/pcodewriter.h"

#include "reone/script/instrutil.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"

namespace reone {

namespace script {

void PcodeWriter::save(IOutputStream &pcode) {
    std::set<uint32_t> jumpOffsets;
    for (auto &instr : _program.instructions()) {
        switch (instr.type) {
        case InstructionType::JMP:
        case InstructionType::JSR:
        case InstructionType::JZ:
        case InstructionType::JNZ:
            jumpOffsets.insert(instr.offset + instr.jumpOffset);
            break;
        default:
            break;
        }
    }
    auto writer = TextWriter(pcode);
    for (auto &instr : _program.instructions()) {
        writeInstruction(instr, writer, jumpOffsets);
    }
}

void PcodeWriter::writeInstruction(const Instruction &ins, TextWriter &pcode, const std::set<uint32_t> &jumpOffsets) {
    if (jumpOffsets.count(ins.offset) > 0) {
        pcode.write(str(boost::format("%08x\t") % ins.offset));
        pcode.write(str(boost::format("loc_%08x:") % ins.offset));
        pcode.write("\n");
    }

    pcode.write(str(boost::format("%08x\t") % ins.offset));

    std::string desc(describeInstructionType(ins.type));

    switch (ins.type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
        desc += str(boost::format(" %d, %d") % ins.stackOffset % ins.size);
        break;
    case InstructionType::CONSTI:
        desc += " " + std::to_string(ins.intValue);
        break;
    case InstructionType::CONSTF:
        desc += " " + std::to_string(ins.floatValue);
        break;
    case InstructionType::CONSTS:
        desc += " \"" + ins.strValue + "\"";
        break;
    case InstructionType::CONSTO:
        desc += " " + std::to_string(ins.objectId);
        break;
    case InstructionType::ACTION:
        desc += str(boost::format(" %s, %d") % _routines.get(ins.routine).name() % ins.argCount);
        break;
    case InstructionType::EQUALTT:
    case InstructionType::NEQUALTT:
        desc += " " + std::to_string(ins.size);
        break;
    case InstructionType::MOVSP:
        desc += " " + std::to_string(ins.stackOffset);
        break;
    case InstructionType::JMP:
    case InstructionType::JSR:
    case InstructionType::JZ:
    case InstructionType::JNZ: {
        uint32_t jumpAddr = ins.offset + ins.jumpOffset;
        desc += str(boost::format(" loc_%08x") % jumpAddr);
        break;
    }
    case InstructionType::DESTRUCT:
        desc += str(boost::format(" %d, %d, %d") % ins.size % ins.stackOffset % ins.sizeNoDestroy);
        break;
    case InstructionType::DECISP:
    case InstructionType::INCISP:
    case InstructionType::DECIBP:
    case InstructionType::INCIBP:
        desc += " " + std::to_string(ins.stackOffset);
        break;
    case InstructionType::STORE_STATE:
        desc += str(boost::format(" %d, %d") % ins.size % ins.sizeLocals);
        break;
    default:
        break;
    }

    pcode.write(desc);
    pcode.write("\n");
}

} // namespace script

} // namespace reone
