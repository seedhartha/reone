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

#include <gtest/gtest.h>

#include "reone/script/format/ncswriter.h"
#include "reone/script/program.h"
#include "reone/system/stream/memoryoutput.h"
#include "reone/system/stringbuilder.h"

#include "../../checkutil.h"

using namespace reone;
using namespace reone::script;

TEST(ncs_writer, should_write_ncs) {
    // given

    auto expectedOutput = StringBuilder()
                              .append("NCS V1.0")
                              .append("\x42\x00\x00\x01\x2c", 5)                      // T
                              .append("\x01\x01\xff\xff\xff\xfc\x00\x04", 8)          // CPDOWNSP
                              .append("\x03\x01\xff\xff\xff\xfc\x00\x04", 8)          // CPTOPSP
                              .append("\x26\x01\xff\xff\xff\xfc\x00\x04", 8)          // CPDOWNBP
                              .append("\x27\x01\xff\xff\xff\xfc\x00\x04", 8)          // CPTOPBP
                              .append("\x04\x03\x00\x00\x00\x01", 6)                  // CONSTI
                              .append("\x04\x04\x3f\x80\x00\x00", 6)                  // CONSTF
                              .append("\x04\x05\x00\x02\x41\x61", 6)                  // CONSTS
                              .append("\x04\x06\x00\x00\x00\x02", 6)                  // CONSTO
                              .append("\x05\x00\x00\x01\x02", 5)                      // ACTION
                              .append("\x1b\x00\xff\xff\xff\xfc", 6)                  // MOVSP
                              .append("\x1d\x00\x00\x00\x00\x01", 6)                  // JMP
                              .append("\x1e\x00\x00\x00\x00\x02", 6)                  // JSR
                              .append("\x1f\x00\x00\x00\x00\x03", 6)                  // JZ
                              .append("\x25\x00\x00\x00\x00\x04", 6)                  // JNZ
                              .append("\x21\x01\x00\x08\xff\xfc\x00\x04", 8)          // DESTRUCT
                              .append("\x23\x03\xff\xff\xff\xfc", 6)                  // DECISP
                              .append("\x24\x03\xff\xff\xff\xfc", 6)                  // INCISP
                              .append("\x28\x03\xff\xff\xff\xfc", 6)                  // DECIBP
                              .append("\x29\x03\xff\xff\xff\xfc", 6)                  // INCIBP
                              .append("\x2c\x10\x00\x00\x00\x04\x00\x00\x00\x08", 10) // STORE_STATE
                              .append("\x0b\x24\x00\x04", 4)                          // EQUALTT
                              .append("\x0c\x24\x00\x04", 4)                          // NEQUALTT
                              .append("\x00\x0c", 2)                                  // NOP
                              .append("\x02\x03", 2)                                  // RSADDI
                              .append("\x02\x04", 2)                                  // RSADDF
                              .append("\x02\x05", 2)                                  // RSADDS
                              .append("\x02\x06", 2)                                  // RSADDO
                              .append("\x02\x10", 2)                                  // RSADDEFF
                              .append("\x02\x11", 2)                                  // RSADDEVT
                              .append("\x02\x12", 2)                                  // RSADDLOC
                              .append("\x02\x13", 2)                                  // RSADDTAL
                              .append("\x06\x20", 2)                                  // LOGANDII
                              .append("\x07\x20", 2)                                  // LOGORII
                              .append("\x08\x20", 2)                                  // INCORII
                              .append("\x09\x20", 2)                                  // EXCORII
                              .append("\x0a\x20", 2)                                  // BOOLANDII
                              .append("\x0b\x20", 2)                                  // EQUALII
                              .append("\x0b\x21", 2)                                  // EQUALFF
                              .append("\x0b\x23", 2)                                  // EQUALSS
                              .append("\x0b\x22", 2)                                  // EQUALOO
                              .append("\x0b\x30", 2)                                  // EQUALEFFEFF
                              .append("\x0b\x31", 2)                                  // EQUALEVTEVT
                              .append("\x0b\x32", 2)                                  // EQUALLOCLOC
                              .append("\x0b\x33", 2)                                  // EQUALTALTAL
                              .append("\x0c\x20", 2)                                  // NEQUALII
                              .append("\x0c\x21", 2)                                  // NEQUALFF
                              .append("\x0c\x23", 2)                                  // NEQUALSS
                              .append("\x0c\x22", 2)                                  // NEQUALOO
                              .append("\x0c\x30", 2)                                  // NEQUALEFFEFF
                              .append("\x0c\x31", 2)                                  // NEQUALEVTEVT
                              .append("\x0c\x32", 2)                                  // NEQUALLOCLOC
                              .append("\x0c\x33", 2)                                  // NEQUALTALTAL
                              .append("\x0d\x20", 2)                                  // GEQII
                              .append("\x0d\x21", 2)                                  // GEQFF
                              .append("\x0e\x20", 2)                                  // GTII
                              .append("\x0e\x21", 2)                                  // GTFF
                              .append("\x0f\x20", 2)                                  // LTII
                              .append("\x0f\x21", 2)                                  // LTFF
                              .append("\x10\x20", 2)                                  // LEQII
                              .append("\x10\x21", 2)                                  // LEQFF
                              .append("\x11\x20", 2)                                  // SHLEFTII
                              .append("\x12\x20", 2)                                  // SHRIGHTII
                              .append("\x13\x20", 2)                                  // USHRIGHTII
                              .append("\x14\x20", 2)                                  // ADDII
                              .append("\x14\x25", 2)                                  // ADDIF
                              .append("\x14\x26", 2)                                  // ADDFI
                              .append("\x14\x21", 2)                                  // ADDFF
                              .append("\x14\x23", 2)                                  // ADDSS
                              .append("\x14\x3a", 2)                                  // ADDVV
                              .append("\x15\x20", 2)                                  // SUBII
                              .append("\x15\x25", 2)                                  // SUBIF
                              .append("\x15\x26", 2)                                  // SUBFI
                              .append("\x15\x21", 2)                                  // SUBFF
                              .append("\x15\x3a", 2)                                  // SUBVV
                              .append("\x16\x20", 2)                                  // MULII
                              .append("\x16\x25", 2)                                  // MULIF
                              .append("\x16\x26", 2)                                  // MULFI
                              .append("\x16\x21", 2)                                  // MULFF
                              .append("\x16\x3b", 2)                                  // MULVF
                              .append("\x16\x3c", 2)                                  // MULFV
                              .append("\x17\x20", 2)                                  // DIVII
                              .append("\x17\x25", 2)                                  // DIVIF
                              .append("\x17\x26", 2)                                  // DIVFI
                              .append("\x17\x21", 2)                                  // DIVFF
                              .append("\x17\x3b", 2)                                  // DIVVF
                              .append("\x17\x3c", 2)                                  // DIVFV
                              .append("\x18\x20", 2)                                  // MODII
                              .append("\x19\x03", 2)                                  // NEGI
                              .append("\x19\x04", 2)                                  // NEGF
                              .append("\x1a\x03", 2)                                  // COMPI
                              .append("\x20\x00", 2)                                  // RETN
                              .append("\x22\x03", 2)                                  // NOTI
                              .append("\x2a\x00", 2)                                  // SAVEBP
                              .append("\x2b\x00", 2)                                  // RESTOREBP
                              .append("\x2d\x00", 2)                                  // NOP2
                              .string();

    auto program = ScriptProgram("");
    program.add(Instruction::newCPDOWNSP(-4, 4));
    program.add(Instruction::newCPTOPSP(-4, 4));
    program.add(Instruction::newCPDOWNBP(-4, 4));
    program.add(Instruction::newCPTOPBP(-4, 4));
    program.add(Instruction::newCONSTI(1));
    program.add(Instruction::newCONSTF(1.0f));
    program.add(Instruction::newCONSTS("Aa"));
    program.add(Instruction::newCONSTO(2));
    program.add(Instruction::newACTION(1, 2));
    program.add(Instruction::newMOVSP(-4));
    program.add(Instruction::newJMP(1));
    program.add(Instruction::newJSR(2));
    program.add(Instruction::newJZ(3));
    program.add(Instruction::newJNZ(4));
    program.add(Instruction::newDESTRUCT(8, -4, 4));
    program.add(Instruction::newDECISP(-4));
    program.add(Instruction::newINCISP(-4));
    program.add(Instruction::newDECIBP(-4));
    program.add(Instruction::newINCIBP(-4));
    program.add(Instruction::newSTORE_STATE(4, 8));
    program.add(Instruction::newEQUALTT(4));
    program.add(Instruction::newNEQUALTT(4));
    program.add(Instruction(InstructionType::NOP));
    program.add(Instruction(InstructionType::RSADDI));
    program.add(Instruction(InstructionType::RSADDF));
    program.add(Instruction(InstructionType::RSADDS));
    program.add(Instruction(InstructionType::RSADDO));
    program.add(Instruction(InstructionType::RSADDEFF));
    program.add(Instruction(InstructionType::RSADDEVT));
    program.add(Instruction(InstructionType::RSADDLOC));
    program.add(Instruction(InstructionType::RSADDTAL));
    program.add(Instruction(InstructionType::LOGANDII));
    program.add(Instruction(InstructionType::LOGORII));
    program.add(Instruction(InstructionType::INCORII));
    program.add(Instruction(InstructionType::EXCORII));
    program.add(Instruction(InstructionType::BOOLANDII));
    program.add(Instruction(InstructionType::EQUALII));
    program.add(Instruction(InstructionType::EQUALFF));
    program.add(Instruction(InstructionType::EQUALSS));
    program.add(Instruction(InstructionType::EQUALOO));
    program.add(Instruction(InstructionType::EQUALEFFEFF));
    program.add(Instruction(InstructionType::EQUALEVTEVT));
    program.add(Instruction(InstructionType::EQUALLOCLOC));
    program.add(Instruction(InstructionType::EQUALTALTAL));
    program.add(Instruction(InstructionType::NEQUALII));
    program.add(Instruction(InstructionType::NEQUALFF));
    program.add(Instruction(InstructionType::NEQUALSS));
    program.add(Instruction(InstructionType::NEQUALOO));
    program.add(Instruction(InstructionType::NEQUALEFFEFF));
    program.add(Instruction(InstructionType::NEQUALEVTEVT));
    program.add(Instruction(InstructionType::NEQUALLOCLOC));
    program.add(Instruction(InstructionType::NEQUALTALTAL));
    program.add(Instruction(InstructionType::GEQII));
    program.add(Instruction(InstructionType::GEQFF));
    program.add(Instruction(InstructionType::GTII));
    program.add(Instruction(InstructionType::GTFF));
    program.add(Instruction(InstructionType::LTII));
    program.add(Instruction(InstructionType::LTFF));
    program.add(Instruction(InstructionType::LEQII));
    program.add(Instruction(InstructionType::LEQFF));
    program.add(Instruction(InstructionType::SHLEFTII));
    program.add(Instruction(InstructionType::SHRIGHTII));
    program.add(Instruction(InstructionType::USHRIGHTII));
    program.add(Instruction(InstructionType::ADDII));
    program.add(Instruction(InstructionType::ADDIF));
    program.add(Instruction(InstructionType::ADDFI));
    program.add(Instruction(InstructionType::ADDFF));
    program.add(Instruction(InstructionType::ADDSS));
    program.add(Instruction(InstructionType::ADDVV));
    program.add(Instruction(InstructionType::SUBII));
    program.add(Instruction(InstructionType::SUBIF));
    program.add(Instruction(InstructionType::SUBFI));
    program.add(Instruction(InstructionType::SUBFF));
    program.add(Instruction(InstructionType::SUBVV));
    program.add(Instruction(InstructionType::MULII));
    program.add(Instruction(InstructionType::MULIF));
    program.add(Instruction(InstructionType::MULFI));
    program.add(Instruction(InstructionType::MULFF));
    program.add(Instruction(InstructionType::MULVF));
    program.add(Instruction(InstructionType::MULFV));
    program.add(Instruction(InstructionType::DIVII));
    program.add(Instruction(InstructionType::DIVIF));
    program.add(Instruction(InstructionType::DIVFI));
    program.add(Instruction(InstructionType::DIVFF));
    program.add(Instruction(InstructionType::DIVVF));
    program.add(Instruction(InstructionType::DIVFV));
    program.add(Instruction(InstructionType::MODII));
    program.add(Instruction(InstructionType::NEGI));
    program.add(Instruction(InstructionType::NEGF));
    program.add(Instruction(InstructionType::COMPI));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction(InstructionType::NOTI));
    program.add(Instruction(InstructionType::SAVEBP));
    program.add(Instruction(InstructionType::RESTOREBP));
    program.add(Instruction(InstructionType::NOP2));

    auto writer = NcsWriter(program);
    auto bytes = ByteArray();
    auto stream = std::make_shared<MemoryOutputStream>(bytes);

    // when

    writer.save(stream);

    // then

    auto actualOutput = std::string(&bytes[0], bytes.size());
    EXPECT_EQ(expectedOutput, actualOutput) << notEqualMessage(expectedOutput, actualOutput);
}
