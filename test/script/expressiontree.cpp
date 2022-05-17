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

#include "../../src/game/script/routines.h"
#include "../../src/script/expressiontree.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(expression_tree)

BOOST_AUTO_TEST_CASE(should_decompile_program__minimal) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines();
    routines.initForKotOR();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_CHECK_EQUAL(0ll, globals.size());

    auto &functions = tree.functions();
    BOOST_CHECK_EQUAL(1ll, functions.size());

    auto &startFunc = functions[0];
    BOOST_CHECK_EQUAL("_start", startFunc->name);
    BOOST_CHECK_EQUAL(1ll, startFunc->block->expressions.size());
    BOOST_CHECK_EQUAL(0ll, startFunc->inArgumentTypes.size());
    BOOST_CHECK_EQUAL(0ll, startFunc->outArgumentTypes.size());
    BOOST_CHECK_EQUAL(static_cast<int>(VariableType::Void), static_cast<int>(startFunc->returnType));
}

BOOST_AUTO_TEST_CASE(should_decompile_program__starting_conditional_without_globals) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction(InstructionType::RSADDI));
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction::newCONSTI(1));
    program.add(Instruction::newCPDOWNSP(-8, 4));
    program.add(Instruction::newMOVSP(-4));
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines();
    routines.initForKotOR();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_CHECK_EQUAL(0ll, globals.size());

    auto &functions = tree.functions();
    BOOST_CHECK_EQUAL(2ll, functions.size());

    auto startingConditionalFunc = functions[0];
    BOOST_CHECK_EQUAL("StartingConditional", startingConditionalFunc->name);
    BOOST_CHECK_EQUAL(0ll, startingConditionalFunc->inArgumentTypes.size());
    BOOST_CHECK_EQUAL(1ll, startingConditionalFunc->outArgumentTypes.size());
    BOOST_CHECK_EQUAL(static_cast<int>(VariableType::Int), static_cast<int>(startingConditionalFunc->outArgumentTypes[0]));
    BOOST_CHECK_EQUAL(static_cast<int>(VariableType::Void), static_cast<int>(startingConditionalFunc->returnType));

    auto startFunc = functions[1];
    BOOST_CHECK_EQUAL("_start", startFunc->name);
}

BOOST_AUTO_TEST_CASE(should_decompile_program__main_with_globals) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction(InstructionType::RSADDI));
    program.add(Instruction::newCONSTI(1));
    program.add(Instruction::newCPDOWNSP(-8, 4));
    program.add(Instruction::newMOVSP(-4));
    program.add(Instruction(InstructionType::SAVEBP));
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RESTOREBP));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines();
    routines.initForKotOR();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_CHECK_EQUAL(1ll, globals.size());

    auto &functions = tree.functions();
    BOOST_CHECK_EQUAL(3ll, functions.size());

    auto mainFunc = functions[0];
    BOOST_CHECK_EQUAL("main", mainFunc->name);
    BOOST_CHECK_EQUAL(0ll, mainFunc->inArgumentTypes.size());
    BOOST_CHECK_EQUAL(0ll, mainFunc->outArgumentTypes.size());
    BOOST_CHECK_EQUAL(static_cast<int>(VariableType::Void), static_cast<int>(mainFunc->returnType));

    auto globalsFunc = functions[1];
    BOOST_CHECK_EQUAL("_globals", globalsFunc->name);

    auto startFunc = functions[2];
    BOOST_CHECK_EQUAL("_start", startFunc->name);
}

BOOST_AUTO_TEST_SUITE_END()
