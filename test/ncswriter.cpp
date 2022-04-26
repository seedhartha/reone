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

#include "../src/script/format/ncswriter.h"
#include "../src/script/program.h"

#include "checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(ncs_writer)

BOOST_AUTO_TEST_CASE(should_write_ncs) {
    // given

    auto ss = ostringstream();
    ss << "NCS V1.0";
    ss << string("\x42\x00\x00\x00\x9a", 5);                      // T
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
