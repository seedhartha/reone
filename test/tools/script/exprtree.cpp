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

#include "reone/game/script/routines.h"
#include "reone/tools/script/exprtree.h"
#include "reone/tools/script/exprtreeoptimizer.h"

using namespace reone;
using namespace reone::game;
using namespace reone::resource;
using namespace reone::script;

class StubExpressionTreeOptimizer : public IExpressionTreeOptimizer, boost::noncopyable {
public:
    void optimize(ExpressionTree &tree) {}
};

TEST(expression_tree, should_decompile_minimal_program_with_main) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines(GameID::KotOR, nullptr, nullptr);
    routines.init();

    // when

    auto optimizer = StubExpressionTreeOptimizer();
    auto tree = ExpressionTree::fromProgram(program, routines, optimizer);

    // then

    EXPECT_TRUE(tree.globals().empty());
    EXPECT_EQ(2, tree.functions().size());
    auto &func0 = *tree.functions()[0];
    auto &func1 = *tree.functions()[1];

    EXPECT_EQ("__start", func0.name);
    EXPECT_EQ(VariableType::Void, func0.returnType);
    EXPECT_TRUE(func0.arguments.empty());
    EXPECT_EQ(13, func0.start);
    EXPECT_EQ(19, func0.end);
    EXPECT_EQ(2, func0.block->expressions.size());

    auto &func0expr0 = *func0.block->expressions[0];
    EXPECT_EQ(ExpressionType::Call, func0expr0.type);
    auto &func0expr0call = *static_cast<CallExpression *>(&func0expr0);
    EXPECT_EQ(&func1, func0expr0call.function);
    EXPECT_TRUE(func0expr0call.arguments.empty());

    auto &func0expr1 = *func0.block->expressions[1];
    EXPECT_EQ(ExpressionType::Return, func0expr1.type);
    auto &func0expr1ret = *static_cast<ReturnExpression *>(&func0expr1);
    EXPECT_EQ(nullptr, func0expr1ret.value);

    EXPECT_EQ("main", func1.name);
    EXPECT_EQ(VariableType::Void, func1.returnType);
    EXPECT_TRUE(func1.arguments.empty());
    EXPECT_EQ(21, func1.start);
    EXPECT_EQ(21, func1.end);
    EXPECT_EQ(1, func1.block->expressions.size());

    auto &func1expr1 = *func1.block->expressions[0];
    EXPECT_EQ(ExpressionType::Return, func1expr1.type);
    auto &func1expr1ret = *static_cast<ReturnExpression *>(&func1expr1);
    EXPECT_EQ(nullptr, func1expr1ret.value);
}

TEST(expression_tree, should_decompile_program_with_starting_conditional_and_no_globals) {
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

    EXPECT_TRUE(tree.globals().empty());
    EXPECT_EQ(2, tree.functions().size());
    auto &func0 = *tree.functions()[0];
    auto &func1 = *tree.functions()[1];
    EXPECT_EQ("__start", func0.name);
    EXPECT_EQ("main", func1.name);

    EXPECT_EQ(1, func1.arguments.size());
    auto &func1arg0 = func1.arguments[0];
    EXPECT_EQ(VariableType::Int, func1arg0.type);
    EXPECT_EQ(-4, func1arg0.stackOffset);
    EXPECT_TRUE(func1arg0.pointer);
    EXPECT_EQ(VariableType::Void, func1.returnType);
    EXPECT_EQ(3, func1.block->expressions.size());

    auto &func1expr0 = *func1.block->expressions[0];
    EXPECT_EQ(ExpressionType::Assign, func1expr0.type);
    auto &func1expr0assign = *static_cast<BinaryExpression *>(&func1expr0);
    EXPECT_EQ(ExpressionType::Parameter, func1expr0assign.left->type);
    EXPECT_EQ(ExpressionType::Constant, func1expr0assign.right->type);
    EXPECT_EQ(Variable::ofInt(1), static_cast<ConstantExpression *>(func1expr0assign.right)->value);
    EXPECT_TRUE(func1expr0assign.declareLeft);

    auto &func1expr1 = *func1.block->expressions[1];
    EXPECT_EQ(ExpressionType::Assign, func1expr1.type);
    auto &func1expr1assign = *static_cast<BinaryExpression *>(&func1expr1);
    EXPECT_EQ(ExpressionType::Parameter, func1expr1assign.left->type);
    EXPECT_EQ(ExpressionType::Parameter, func1expr1assign.right->type);
    EXPECT_EQ(func1expr0assign.left, func1expr1assign.right);
    EXPECT_EQ(func1arg0.param, func1expr1assign.left);
    EXPECT_FALSE(func1expr1assign.declareLeft);
    auto &func1expr2 = *func1.block->expressions[2];
    EXPECT_EQ(ExpressionType::Return, func1expr2.type);
    EXPECT_EQ(nullptr, static_cast<ReturnExpression *>(&func1expr2)->value);
}

TEST(expression_tree, should_decompile_program_with_main_and_globals) {
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

    EXPECT_EQ(1, tree.globals().size());
    auto &global0 = tree.globals()[0];
    EXPECT_EQ(VariableType::Int, global0.param->variableType);
    EXPECT_EQ(VariableType::Void, global0.value.type);

    EXPECT_EQ(3, tree.functions().size());
    auto &func0 = *tree.functions()[0];
    auto &func1 = *tree.functions()[1];
    auto &func2 = *tree.functions()[2];
    EXPECT_EQ("__start", func0.name);
    EXPECT_EQ("__globals", func1.name);
    EXPECT_EQ("main", func2.name);

    EXPECT_EQ(4, func1.block->expressions.size());

    auto &func1expr0 = *func1.block->expressions[0];
    EXPECT_EQ(ExpressionType::Assign, func1expr0.type);
    auto &func1expr0assign = *static_cast<BinaryExpression *>(&func1expr0);
    EXPECT_EQ(ExpressionType::Constant, func1expr0assign.right->type);
    EXPECT_TRUE(func1expr0assign.declareLeft);

    auto &func1expr1 = *func1.block->expressions[1];
    EXPECT_EQ(ExpressionType::Assign, func1expr1.type);
    auto &func1expr1assign = *static_cast<BinaryExpression *>(&func1expr1);
    EXPECT_EQ(global0.param, func1expr1assign.left);
    EXPECT_EQ(func1expr0assign.left, func1expr1assign.right);
    EXPECT_FALSE(func1expr1assign.declareLeft);

    auto &func1expr2 = *func1.block->expressions[2];
    EXPECT_EQ(ExpressionType::Call, func1expr2.type);

    auto &func1expr3 = *func1.block->expressions[3];
    EXPECT_EQ(ExpressionType::Return, func1expr3.type);
}

TEST(expression_tree, should_decompile_program_with_conditionals) {
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

    EXPECT_TRUE(tree.globals().empty());
    EXPECT_EQ(2, tree.functions().size());
    auto &func0 = *tree.functions()[0];
    auto &func1 = *tree.functions()[1];
    EXPECT_EQ("__start", func0.name);
    EXPECT_EQ("main", func1.name);

    EXPECT_EQ(7, func1.block->expressions.size());
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[0]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[1]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[2]->type);

    auto &func1expr3 = *func1.block->expressions[3];
    EXPECT_EQ(ExpressionType::Conditional, func1expr3.type);
    auto &func1expr3cond = *static_cast<ConditionalExpression *>(&func1expr3);
    EXPECT_EQ(ExpressionType::Equal, func1expr3cond.test->type);
    auto &func1expr3condTestEqual = *static_cast<BinaryExpression *>(func1expr3cond.test);
    EXPECT_EQ(ExpressionType::Parameter, func1expr3condTestEqual.left->type);
    EXPECT_EQ(ExpressionType::Constant, func1expr3condTestEqual.right->type);
    auto &func1expr3condTestEqualRight = *static_cast<ConstantExpression *>(func1expr3condTestEqual.right);
    EXPECT_EQ(Variable::ofInt(0), func1expr3condTestEqualRight.value);
    EXPECT_EQ(1, func1expr3cond.ifTrue->expressions.size());
    EXPECT_EQ(ExpressionType::Return, func1expr3cond.ifTrue->expressions[0]->type);

    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[4]->type);

    auto &func1expr5 = *func1.block->expressions[5];
    EXPECT_EQ(ExpressionType::Conditional, func1expr5.type);
    auto &func1expr5cond = *static_cast<ConditionalExpression *>(&func1expr5);
    EXPECT_EQ(ExpressionType::NotEqual, func1expr5cond.test->type);
    auto &func1expr5test = *static_cast<BinaryExpression *>(func1expr5cond.test);
    EXPECT_EQ(ExpressionType::Parameter, func1expr5test.left->type);
    EXPECT_EQ(ExpressionType::Constant, func1expr5test.right->type);
    auto &func1expr5testRight = *static_cast<ConstantExpression *>(func1expr5test.right);
    EXPECT_EQ(Variable::ofInt(0), func1expr5testRight.value);
    EXPECT_EQ(1, func1expr5cond.ifTrue->expressions.size());
    EXPECT_EQ(ExpressionType::Return, func1expr5cond.ifTrue->expressions[0]->type);

    EXPECT_EQ(ExpressionType::Return, func1.block->expressions[6]->type);
}

TEST(expression_tree, should_decompile_program_with_a_loop) {
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

    EXPECT_TRUE(tree.globals().empty());
    EXPECT_EQ(2, tree.functions().size());
    auto &func0 = *tree.functions()[0];
    auto &func1 = *tree.functions()[1];
    EXPECT_EQ("__start", func0.name);
    EXPECT_EQ("main", func1.name);

    EXPECT_EQ(10, func1.block->expressions.size());
    EXPECT_EQ(ExpressionType::Parameter, func1.block->expressions[0]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[1]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[2]->type);

    auto &func1expr3 = *func1.block->expressions[3];
    EXPECT_EQ(ExpressionType::Label, func1expr3.type);
    EXPECT_EQ(ExpressionType::Increment, func1.block->expressions[4]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[5]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[6]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[7]->type);

    auto &func1expr8 = *func1.block->expressions[8];
    EXPECT_EQ(ExpressionType::Conditional, func1expr8.type);
    auto &func1expr8cond = *static_cast<ConditionalExpression *>(&func1expr8);
    EXPECT_EQ(ExpressionType::NotEqual, func1expr8cond.test->type);
    EXPECT_EQ(1, func1expr8cond.ifTrue->expressions.size());
    auto &func1expr8ifTrueExpr0 = *func1expr8cond.ifTrue->expressions[0];
    EXPECT_EQ(ExpressionType::Goto, func1expr8ifTrueExpr0.type);
    auto &func1expr8ifTrueExpr0Goto = *static_cast<GotoExpression *>(&func1expr8ifTrueExpr0);
    EXPECT_EQ(&func1expr3, func1expr8ifTrueExpr0Goto.label);

    EXPECT_EQ(ExpressionType::Return, func1.block->expressions[9]->type);
}

TEST(expression_tree, should_decompile_program_with_vectors) {
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

    EXPECT_TRUE(tree.globals().empty());
    EXPECT_EQ(2, tree.functions().size());
    auto &func0 = *tree.functions()[0];
    auto &func1 = *tree.functions()[1];

    EXPECT_EQ(func1.block->expressions.size(), 71);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[0]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[1]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[2]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[3]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[4]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[5]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[6]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[7]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[8]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[9]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[10]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[11]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[12]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[13]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[14]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[15]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[16]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[17]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[18]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[19]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[20]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[21]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[22]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[23]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[24]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[25]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[26]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[27]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[28]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[29]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[30]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[31]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[32]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[33]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[34]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[35]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[36]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[37]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[38]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[39]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[40]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[41]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[42]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[43]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[44]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[45]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[46]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[47]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[48]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[49]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[50]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[51]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[52]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[53]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[54]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[55]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[56]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[57]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[58]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[59]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[60]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[61]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[62]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[63]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[64]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[65]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[66]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[67]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[68]->type);
    EXPECT_EQ(ExpressionType::Assign, func1.block->expressions[69]->type);
    EXPECT_EQ(ExpressionType::Return, func1.block->expressions[70]->type);
}
