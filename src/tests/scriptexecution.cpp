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

#define BOOST_TEST_MODULE scriptexecution

#include <boost/test/included/unit_test.hpp>

#include "../engine/script/execution.h"

using namespace std;

using namespace reone::script;

BOOST_AUTO_TEST_CASE(test_destruct) {
    Instruction instr;
    auto program = make_shared<ScriptProgram>("");

    instr.offset = 13;
    instr.byteCode = ByteCode::PushConstant;
    instr.type = InstructionType::Int;
    instr.intValue = 0;
    instr.nextOffset = instr.offset + 6;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.intValue = 1;
    instr.nextOffset = instr.offset + 6;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.intValue = 2;
    instr.nextOffset = instr.offset + 6;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.intValue = 3;
    instr.nextOffset = instr.offset + 6;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.byteCode = ByteCode::Destruct;
    instr.size = 16;
    instr.stackOffset = 4;
    instr.sizeNoDestroy = 4;
    instr.nextOffset = instr.offset + 8;
    program->add(instr);

    program->setLength(instr.nextOffset);

    auto context = make_unique<ExecutionContext>();
    ScriptExecution execution(program, move(context));
    execution.run();

    BOOST_TEST((execution.getStackSize() == 1));
    BOOST_TEST((execution.getStackVariable(0).intValue == 1));
}

BOOST_AUTO_TEST_CASE(test_cptopbp) {
    Instruction instr;
    auto program = make_shared<ScriptProgram>("");

    instr.offset = 13;
    instr.byteCode = ByteCode::PushConstant;
    instr.type = InstructionType::Int;
    instr.intValue = 0;
    instr.nextOffset = instr.offset + 6;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.intValue = 1;
    instr.nextOffset = instr.offset + 6;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.intValue = 2;
    instr.nextOffset = instr.offset + 6;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.byteCode = ByteCode::SaveBP;
    instr.type = InstructionType::None;
    instr.nextOffset = instr.offset + 2;
    program->add(instr);

    instr.offset = instr.nextOffset;
    instr.byteCode = ByteCode::CopyTopBP;
    instr.stackOffset = -8;
    instr.size = 8;
    instr.nextOffset = instr.offset + 8;
    program->add(instr);

    program->setLength(instr.nextOffset);

    auto context = make_unique<ExecutionContext>();
    ScriptExecution execution(program, move(context));
    execution.run();

    BOOST_TEST((execution.getStackSize() == 6));
    BOOST_TEST((execution.getStackVariable(4).intValue == 1));
    BOOST_TEST((execution.getStackVariable(5).intValue == 2));
}
