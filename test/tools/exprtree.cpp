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

#include <boost/test/unit_test.hpp>

#include "reone/game/script/routines.h"
#include "reone/tools/script/exprtree.h"
#include "reone/tools/script/exprtreeoptimizer.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::script;

class StubExpressionTreeOptimizer : public IExpressionTreeOptimizer, boost::noncopyable {
public:
    void optimize(ExpressionTree &tree) {}
};

BOOST_AUTO_TEST_SUITE(expression_tree)

BOOST_AUTO_TEST_CASE(should_decompile_program__minimal) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines(GameID::KotOR, nullptr, nullptr);
    routines.init();

    // when

    auto optimizer = StubExpressionTreeOptimizer();
    auto tree = ExpressionTree::fromProgram(program, routines, optimizer);

    // then
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

    auto routines = Routines(GameID::KotOR, nullptr, nullptr);
    routines.init();

    auto optimizer = StubExpressionTreeOptimizer();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines, optimizer);

    // then
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

    auto routines = Routines(GameID::KotOR, nullptr, nullptr);
    routines.init();

    auto optimizer = StubExpressionTreeOptimizer();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines, optimizer);

    // then
}

BOOST_AUTO_TEST_CASE(should_decompile_program__conditionals) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction::newCONSTI(2));
    program.add(Instruction::newCONSTI(1));
    program.add(Instruction::newCPTOPSP(-8, 4));
    program.add(Instruction::newJZ(18));
    program.add(Instruction(InstructionType::DIVII));
    program.add(Instruction::newJNZ(8));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction::newMOVSP(-8));
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines(GameID::KotOR, nullptr, nullptr);
    routines.init();

    auto optimizer = StubExpressionTreeOptimizer();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines, optimizer);

    // then
}

BOOST_AUTO_TEST_CASE(should_decompile_program__loop) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RETN));
    // int a = 0;
    program.add(Instruction(InstructionType::RSADDI));
    program.add(Instruction::newCONSTI(0));
    program.add(Instruction::newCPDOWNSP(-8, 4));
    program.add(Instruction::newMOVSP(-4));
    // loc_loop:
    // a++;
    program.add(Instruction::newINCISP(-4));
    // if(a < 10) { goto loc_loop; }
    program.add(Instruction::newCONSTI(10));
    program.add(Instruction::newCPTOPSP(-8, 4));
    program.add(Instruction(InstructionType::LTII));
    program.add(Instruction::newJNZ(-22));
    // return;
    program.add(Instruction::newMOVSP(-4));
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines(GameID::KotOR, nullptr, nullptr);
    routines.init();

    auto optimizer = StubExpressionTreeOptimizer();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines, optimizer);

    // then
}

BOOST_AUTO_TEST_CASE(should_decompile_program__vectors) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RETN));
    // vector v1 = Vector(1.0f, 2.0f, 3.0f);
    program.add(Instruction::newCONSTF(3.0f));
    program.add(Instruction::newCONSTF(2.0f));
    program.add(Instruction::newCONSTF(1.0f));
    program.add(Instruction::newACTION(142, 3));
    // vector v2 = Vector(-3.0f, -2.0f, -1.0f);
    program.add(Instruction::newCONSTF(-1.0f));
    program.add(Instruction::newCONSTF(-2.0f));
    program.add(Instruction::newCONSTF(-3.0f));
    program.add(Instruction::newACTION(142, 3));
    // vector v3 = v1 + v2;
    program.add(Instruction::newCPTOPSP(-24, 24));
    program.add(Instruction(InstructionType::ADDVV));
    program.add(Instruction::newMOVSP(-12));
    // vector v4 = v1 - v2;
    program.add(Instruction::newCPTOPSP(-24, 24));
    program.add(Instruction(InstructionType::SUBVV));
    program.add(Instruction::newMOVSP(-12));
    // vector v5 = 2.0f * v1;
    program.add(Instruction::newCONSTF(2.0f));
    program.add(Instruction::newCPTOPSP(-24, 12));
    program.add(Instruction(InstructionType::MULFV));
    program.add(Instruction::newMOVSP(-12));
    // vector v6 = 2.0f / v1;
    program.add(Instruction::newCONSTF(2.0f));
    program.add(Instruction::newCPTOPSP(-24, 12));
    program.add(Instruction(InstructionType::DIVFV));
    program.add(Instruction::newMOVSP(-12));
    // vector v7 = v1 * 2.0f;
    program.add(Instruction::newCPTOPSP(-24, 12));
    program.add(Instruction::newCONSTF(2.0f));
    program.add(Instruction(InstructionType::MULVF));
    program.add(Instruction::newMOVSP(-12));
    // vector v8 = v1 / 2.0f;
    program.add(Instruction::newCPTOPSP(-24, 12));
    program.add(Instruction::newCONSTF(2.0f));
    program.add(Instruction(InstructionType::DIVVF));
    program.add(Instruction::newMOVSP(-12));
    // float f = VectorToAngle(v1);
    program.add(Instruction::newCPTOPSP(-24, 12));
    program.add(Instruction::newACTION(145, 1));
    program.add(Instruction::newMOVSP(-4));
    // return;
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines(GameID::KotOR, nullptr, nullptr);
    routines.init();

    auto optimizer = StubExpressionTreeOptimizer();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines, optimizer);

    // then
}

BOOST_AUTO_TEST_SUITE_END()
