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

#include "../src/script/format/ncsreader.h"
#include "../src/script/program.h"

using namespace std;

using namespace reone;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(ncs_reader)

BOOST_AUTO_TEST_CASE(should_read_ncs) {
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

    auto reader = NcsReader("");
    auto stream = make_shared<istringstream>(ss.str());

    // when

    reader.load(stream);

    // then

    auto program = reader.program();
    BOOST_CHECK_EQUAL(154, program->length());
    BOOST_CHECK_EQUAL(22ll, program->instructions().size());
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CPDOWNSP), static_cast<int>(program->getInstruction(13).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(13).stackOffset);
    BOOST_CHECK_EQUAL(4, program->getInstruction(13).size);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CPTOPSP), static_cast<int>(program->getInstruction(21).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(21).stackOffset);
    BOOST_CHECK_EQUAL(4, program->getInstruction(21).size);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CPDOWNBP), static_cast<int>(program->getInstruction(29).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(29).stackOffset);
    BOOST_CHECK_EQUAL(4, program->getInstruction(29).size);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CPTOPBP), static_cast<int>(program->getInstruction(37).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(37).stackOffset);
    BOOST_CHECK_EQUAL(4, program->getInstruction(37).size);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CONSTI), static_cast<int>(program->getInstruction(45).type));
    BOOST_CHECK_EQUAL(1, program->getInstruction(45).intValue);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CONSTF), static_cast<int>(program->getInstruction(51).type));
    BOOST_CHECK_EQUAL(1.0f, program->getInstruction(51).floatValue);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CONSTS), static_cast<int>(program->getInstruction(57).type));
    BOOST_CHECK_EQUAL("Aa", program->getInstruction(57).strValue);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::CONSTO), static_cast<int>(program->getInstruction(63).type));
    BOOST_CHECK_EQUAL(2, program->getInstruction(63).objectId);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::ACTION), static_cast<int>(program->getInstruction(69).type));
    BOOST_CHECK_EQUAL(1, program->getInstruction(69).routine);
    BOOST_CHECK_EQUAL(2, program->getInstruction(69).argCount);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::MOVSP), static_cast<int>(program->getInstruction(74).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(74).stackOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::JMP), static_cast<int>(program->getInstruction(80).type));
    BOOST_CHECK_EQUAL(1, program->getInstruction(80).jumpOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::JSR), static_cast<int>(program->getInstruction(86).type));
    BOOST_CHECK_EQUAL(2, program->getInstruction(86).jumpOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::JZ), static_cast<int>(program->getInstruction(92).type));
    BOOST_CHECK_EQUAL(3, program->getInstruction(92).jumpOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::JNZ), static_cast<int>(program->getInstruction(98).type));
    BOOST_CHECK_EQUAL(4, program->getInstruction(98).jumpOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::DESTRUCT), static_cast<int>(program->getInstruction(104).type));
    BOOST_CHECK_EQUAL(8, program->getInstruction(104).size);
    BOOST_CHECK_EQUAL(-4, program->getInstruction(104).stackOffset);
    BOOST_CHECK_EQUAL(4, program->getInstruction(104).sizeNoDestroy);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::DECISP), static_cast<int>(program->getInstruction(112).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(112).stackOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::INCISP), static_cast<int>(program->getInstruction(118).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(118).stackOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::DECIBP), static_cast<int>(program->getInstruction(124).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(124).stackOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::INCIBP), static_cast<int>(program->getInstruction(130).type));
    BOOST_CHECK_EQUAL(-4, program->getInstruction(130).stackOffset);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::STORE_STATE), static_cast<int>(program->getInstruction(136).type));
    BOOST_CHECK_EQUAL(4, program->getInstruction(136).size);
    BOOST_CHECK_EQUAL(8, program->getInstruction(136).sizeLocals);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::EQUALTT), static_cast<int>(program->getInstruction(146).type));
    BOOST_CHECK_EQUAL(4, program->getInstruction(146).size);
    BOOST_CHECK_EQUAL(static_cast<int>(InstructionType::NEQUALTT), static_cast<int>(program->getInstruction(150).type));
    BOOST_CHECK_EQUAL(4, program->getInstruction(150).size);
}

BOOST_AUTO_TEST_SUITE_END()
