/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "pcodewriter.h"

#include "reone/script/instrutil.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace script {

void PcodeWriter::save(const fs::path &path) {
    fs::ofstream pcode(path);
    try {
        set<uint32_t> jumpOffsets;
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
        for (auto &instr : _program.instructions()) {
            writeInstruction(instr, pcode, jumpOffsets);
        }
    } catch (const exception &e) {
        fs::remove(path);
        throw runtime_error(e.what());
    }
}

void PcodeWriter::writeInstruction(const Instruction &ins, fs::ofstream &pcode, const set<uint32_t> &jumpOffsets) {
    if (jumpOffsets.count(ins.offset) > 0) {
        string label(str(boost::format("loc_%08x:") % ins.offset));
        pcode << label << endl;
    }

    string desc(describeInstructionType(ins.type));

    switch (ins.type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
        desc += str(boost::format(" %d, %d") % ins.stackOffset % ins.size);
        break;
    case InstructionType::CONSTI:
        desc += " " + to_string(ins.intValue);
        break;
    case InstructionType::CONSTF:
        desc += " " + to_string(ins.floatValue);
        break;
    case InstructionType::CONSTS:
        desc += " \"" + ins.strValue + "\"";
        break;
    case InstructionType::CONSTO:
        desc += " " + to_string(ins.objectId);
        break;
    case InstructionType::ACTION:
        desc += str(boost::format(" %s, %d") % _routines.get(ins.routine).name() % ins.argCount);
        break;
    case InstructionType::EQUALTT:
    case InstructionType::NEQUALTT:
        desc += " " + to_string(ins.size);
        break;
    case InstructionType::MOVSP:
        desc += " " + to_string(ins.stackOffset);
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
        desc += " " + to_string(ins.stackOffset);
        break;
    case InstructionType::STORE_STATE:
        desc += str(boost::format(" %d, %d") % ins.size % ins.sizeLocals);
        break;
    default:
        break;
    }

    pcode << desc << endl;
}

} // namespace script

} // namespace reone
