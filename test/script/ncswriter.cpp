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

#include "../../src/script/format/ncswriter.h"
#include "../../src/script/program.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(ncs_writer)

BOOST_AUTO_TEST_CASE(should_write_ncs) {
    // given

    auto ss = ostringstream();
    ss << "NCS V1.0";
    ss << string("\x42\x00\x00\x01\x2c", 5);                      // T
    ss << string("\x01\x01\xff\xff\xff\xfc\x00\x04", 8);          // CPDOWNSP
    ss << string("\x03\x01\xff\xff\xff\xfc\x00\x04", 8);          // CPTOPSP
    ss << string("\x26\x01\xff\xff\xff\xfc\x00\x04", 8);          // CPDOWNBP
    ss << string("\x27\x01\xff\xff\xff\xfc\x00\x04", 8);          // CPTOPBP
    ss << string("\x04\x03\x00\x00\x00\x01", 6);                  // CONSTI
    ss << string("\x04\x04\x3f\x80\x00\x00", 6);                  // CONSTF
    ss << string("\x04\x05\x00\x02\x41\x61", 6);                  // CONSTS
    ss << string("\x04\x06\x00\x00\x00\x02", 6);                  // CONSTO
    ss << string("\x05\x00\x00\x01\x02", 5);                      // ACTION
    ss << string("\x1b\x00\xff\xff\xff\xfc", 6);                  // MOVSP
    ss << string("\x1d\x00\x00\x00\x00\x01", 6);                  // JMP
    ss << string("\x1e\x00\x00\x00\x00\x02", 6);                  // JSR
    ss << string("\x1f\x00\x00\x00\x00\x03", 6);                  // JZ
    ss << string("\x25\x00\x00\x00\x00\x04", 6);                  // JNZ
    ss << string("\x21\x01\x00\x08\xff\xfc\x00\x04", 8);          // DESTRUCT
    ss << string("\x23\x03\xff\xff\xff\xfc", 6);                  // DECISP
    ss << string("\x24\x03\xff\xff\xff\xfc", 6);                  // INCISP
    ss << string("\x28\x03\xff\xff\xff\xfc", 6);                  // DECIBP
    ss << string("\x29\x03\xff\xff\xff\xfc", 6);                  // INCIBP
    ss << string("\x2c\x10\x00\x00\x00\x04\x00\x00\x00\x08", 10); // STORE_STATE
    ss << string("\x0b\x24\x00\x04", 4);                          // EQUALTT
    ss << string("\x0c\x24\x00\x04", 4);                          // NEQUALTT
    ss << string("\x00\x0c", 2);                                  // NOP
    ss << string("\x02\x03", 2);                                  // RSADDI
    ss << string("\x02\x04", 2);                                  // RSADDF
    ss << string("\x02\x05", 2);                                  // RSADDS
    ss << string("\x02\x06", 2);                                  // RSADDO
    ss << string("\x02\x10", 2);                                  // RSADDEFF
    ss << string("\x02\x11", 2);                                  // RSADDEVT
    ss << string("\x02\x12", 2);                                  // RSADDLOC
    ss << string("\x02\x13", 2);                                  // RSADDTAL
    ss << string("\x06\x20", 2);                                  // LOGANDII
    ss << string("\x07\x20", 2);                                  // LOGORII
    ss << string("\x08\x20", 2);                                  // INCORII
    ss << string("\x09\x20", 2);                                  // EXCORII
    ss << string("\x0a\x20", 2);                                  // BOOLANDII
    ss << string("\x0b\x20", 2);                                  // EQUALII
    ss << string("\x0b\x21", 2);                                  // EQUALFF
    ss << string("\x0b\x23", 2);                                  // EQUALSS
    ss << string("\x0b\x22", 2);                                  // EQUALOO
    ss << string("\x0b\x30", 2);                                  // EQUALEFFEFF
    ss << string("\x0b\x31", 2);                                  // EQUALEVTEVT
    ss << string("\x0b\x32", 2);                                  // EQUALLOCLOC
    ss << string("\x0b\x33", 2);                                  // EQUALTALTAL
    ss << string("\x0c\x20", 2);                                  // NEQUALII
    ss << string("\x0c\x21", 2);                                  // NEQUALFF
    ss << string("\x0c\x23", 2);                                  // NEQUALSS
    ss << string("\x0c\x22", 2);                                  // NEQUALOO
    ss << string("\x0c\x30", 2);                                  // NEQUALEFFEFF
    ss << string("\x0c\x31", 2);                                  // NEQUALEVTEVT
    ss << string("\x0c\x32", 2);                                  // NEQUALLOCLOC
    ss << string("\x0c\x33", 2);                                  // NEQUALTALTAL
    ss << string("\x0d\x20", 2);                                  // GEQII
    ss << string("\x0d\x21", 2);                                  // GEQFF
    ss << string("\x0e\x20", 2);                                  // GTII
    ss << string("\x0e\x21", 2);                                  // GTFF
    ss << string("\x0f\x20", 2);                                  // LTII
    ss << string("\x0f\x21", 2);                                  // LTFF
    ss << string("\x10\x20", 2);                                  // LEQII
    ss << string("\x10\x21", 2);                                  // LEQFF
    ss << string("\x11\x20", 2);                                  // SHLEFTII
    ss << string("\x12\x20", 2);                                  // SHRIGHTII
    ss << string("\x13\x20", 2);                                  // USHRIGHTII
    ss << string("\x14\x20", 2);                                  // ADDII
    ss << string("\x14\x25", 2);                                  // ADDIF
    ss << string("\x14\x26", 2);                                  // ADDFI
    ss << string("\x14\x21", 2);                                  // ADDFF
    ss << string("\x14\x23", 2);                                  // ADDSS
    ss << string("\x14\x3a", 2);                                  // ADDVV
    ss << string("\x15\x20", 2);                                  // SUBII
    ss << string("\x15\x25", 2);                                  // SUBIF
    ss << string("\x15\x26", 2);                                  // SUBFI
    ss << string("\x15\x21", 2);                                  // SUBFF
    ss << string("\x15\x3a", 2);                                  // SUBVV
    ss << string("\x16\x20", 2);                                  // MULII
    ss << string("\x16\x25", 2);                                  // MULIF
    ss << string("\x16\x26", 2);                                  // MULFI
    ss << string("\x16\x21", 2);                                  // MULFF
    ss << string("\x16\x3b", 2);                                  // MULVF
    ss << string("\x16\x3c", 2);                                  // MULFV
    ss << string("\x17\x20", 2);                                  // DIVII
    ss << string("\x17\x25", 2);                                  // DIVIF
    ss << string("\x17\x26", 2);                                  // DIVFI
    ss << string("\x17\x21", 2);                                  // DIVFF
    ss << string("\x17\x3b", 2);                                  // DIVVF
    ss << string("\x17\x3c", 2);                                  // DIVFV
    ss << string("\x18\x20", 2);                                  // MODII
    ss << string("\x19\x03", 2);                                  // NEGI
    ss << string("\x19\x04", 2);                                  // NEGF
    ss << string("\x1a\x03", 2);                                  // COMPI
    ss << string("\x20\x00", 2);                                  // RETN
    ss << string("\x22\x03", 2);                                  // NOTI
    ss << string("\x2a\x00", 2);                                  // SAVEBP
    ss << string("\x2b\x00", 2);                                  // RESTOREBP
    ss << string("\x2d\x00", 2);                                  // NOP2

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
    auto stream = make_shared<ostringstream>();
    auto expectedOutput = ss.str();

    // when

    writer.save(stream);

    // then

    auto actualOutput = stream->str();
    BOOST_TEST((expectedOutput == actualOutput), notEqualMessage(expectedOutput, actualOutput));
}

BOOST_AUTO_TEST_SUITE_END()
