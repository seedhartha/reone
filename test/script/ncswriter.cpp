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

#include <boost/test/unit_test.hpp>

#include "../../src/common/stream/bytearrayoutput.h"
#include "../../src/common/stringbuilder.h"
#include "../../src/script/format/ncswriter.h"
#include "../../src/script/program.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(ncs_writer)

BOOST_AUTO_TEST_CASE(should_write_ncs) {
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
                              .build();

    auto program = ScriptProgram("");
    program.add(Instruction::newCPDOWNSP(13, -4, 4));
    program.add(Instruction::newCPTOPSP(21, -4, 4));
    program.add(Instruction::newCPDOWNBP(29, -4, 4));
    program.add(Instruction::newCPTOPBP(37, -4, 4));
    program.add(Instruction::newCONSTI(45, 1));
    program.add(Instruction::newCONSTF(51, 1.0f));
    program.add(Instruction::newCONSTS(57, "Aa"));
    program.add(Instruction::newCONSTO(63, 2));
    program.add(Instruction::newACTION(69, 1, 2));
    program.add(Instruction::newMOVSP(74, -4));
    program.add(Instruction::newJMP(80, 1));
    program.add(Instruction::newJSR(86, 2));
    program.add(Instruction::newJZ(92, 3));
    program.add(Instruction::newJNZ(98, 4));
    program.add(Instruction::newDESTRUCT(104, 8, -4, 4));
    program.add(Instruction::newDECISP(112, -4));
    program.add(Instruction::newINCISP(118, -4));
    program.add(Instruction::newDECIBP(124, -4));
    program.add(Instruction::newINCIBP(130, -4));
    program.add(Instruction::newSTORE_STATE(136, 4, 8));
    program.add(Instruction::newEQUALTT(146, 4));
    program.add(Instruction::newNEQUALTT(150, 4));
    program.add(Instruction(154, InstructionType::NOP));
    program.add(Instruction(156, InstructionType::RSADDI));
    program.add(Instruction(158, InstructionType::RSADDF));
    program.add(Instruction(160, InstructionType::RSADDS));
    program.add(Instruction(162, InstructionType::RSADDO));
    program.add(Instruction(164, InstructionType::RSADDEFF));
    program.add(Instruction(166, InstructionType::RSADDEVT));
    program.add(Instruction(168, InstructionType::RSADDLOC));
    program.add(Instruction(170, InstructionType::RSADDTAL));
    program.add(Instruction(172, InstructionType::LOGANDII));
    program.add(Instruction(174, InstructionType::LOGORII));
    program.add(Instruction(176, InstructionType::INCORII));
    program.add(Instruction(178, InstructionType::EXCORII));
    program.add(Instruction(180, InstructionType::BOOLANDII));
    program.add(Instruction(182, InstructionType::EQUALII));
    program.add(Instruction(184, InstructionType::EQUALFF));
    program.add(Instruction(186, InstructionType::EQUALSS));
    program.add(Instruction(188, InstructionType::EQUALOO));
    program.add(Instruction(190, InstructionType::EQUALEFFEFF));
    program.add(Instruction(192, InstructionType::EQUALEVTEVT));
    program.add(Instruction(194, InstructionType::EQUALLOCLOC));
    program.add(Instruction(196, InstructionType::EQUALTALTAL));
    program.add(Instruction(198, InstructionType::NEQUALII));
    program.add(Instruction(200, InstructionType::NEQUALFF));
    program.add(Instruction(202, InstructionType::NEQUALSS));
    program.add(Instruction(204, InstructionType::NEQUALOO));
    program.add(Instruction(206, InstructionType::NEQUALEFFEFF));
    program.add(Instruction(208, InstructionType::NEQUALEVTEVT));
    program.add(Instruction(210, InstructionType::NEQUALLOCLOC));
    program.add(Instruction(212, InstructionType::NEQUALTALTAL));
    program.add(Instruction(214, InstructionType::GEQII));
    program.add(Instruction(216, InstructionType::GEQFF));
    program.add(Instruction(218, InstructionType::GTII));
    program.add(Instruction(220, InstructionType::GTFF));
    program.add(Instruction(222, InstructionType::LTII));
    program.add(Instruction(224, InstructionType::LTFF));
    program.add(Instruction(226, InstructionType::LEQII));
    program.add(Instruction(228, InstructionType::LEQFF));
    program.add(Instruction(230, InstructionType::SHLEFTII));
    program.add(Instruction(232, InstructionType::SHRIGHTII));
    program.add(Instruction(234, InstructionType::USHRIGHTII));
    program.add(Instruction(236, InstructionType::ADDII));
    program.add(Instruction(238, InstructionType::ADDIF));
    program.add(Instruction(240, InstructionType::ADDFI));
    program.add(Instruction(242, InstructionType::ADDFF));
    program.add(Instruction(244, InstructionType::ADDSS));
    program.add(Instruction(246, InstructionType::ADDVV));
    program.add(Instruction(248, InstructionType::SUBII));
    program.add(Instruction(250, InstructionType::SUBIF));
    program.add(Instruction(252, InstructionType::SUBFI));
    program.add(Instruction(254, InstructionType::SUBFF));
    program.add(Instruction(256, InstructionType::SUBVV));
    program.add(Instruction(258, InstructionType::MULII));
    program.add(Instruction(260, InstructionType::MULIF));
    program.add(Instruction(262, InstructionType::MULFI));
    program.add(Instruction(264, InstructionType::MULFF));
    program.add(Instruction(266, InstructionType::MULVF));
    program.add(Instruction(268, InstructionType::MULFV));
    program.add(Instruction(270, InstructionType::DIVII));
    program.add(Instruction(272, InstructionType::DIVIF));
    program.add(Instruction(274, InstructionType::DIVFI));
    program.add(Instruction(276, InstructionType::DIVFF));
    program.add(Instruction(278, InstructionType::DIVVF));
    program.add(Instruction(280, InstructionType::DIVFV));
    program.add(Instruction(282, InstructionType::MODII));
    program.add(Instruction(284, InstructionType::NEGI));
    program.add(Instruction(286, InstructionType::NEGF));
    program.add(Instruction(288, InstructionType::COMPI));
    program.add(Instruction(290, InstructionType::RETN));
    program.add(Instruction(292, InstructionType::NOTI));
    program.add(Instruction(294, InstructionType::SAVEBP));
    program.add(Instruction(296, InstructionType::RESTOREBP));
    program.add(Instruction(298, InstructionType::NOP2));

    auto writer = NcsWriter(program);
    auto bytes = ByteArray();
    auto stream = make_shared<ByteArrayOutputStream>(bytes);

    // when

    writer.save(stream);

    // then

    auto actualOutput = string(&bytes[0], bytes.size());
    BOOST_TEST((expectedOutput == actualOutput), notEqualMessage(expectedOutput, actualOutput));
}

BOOST_AUTO_TEST_SUITE_END()
