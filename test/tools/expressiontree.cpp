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
#include "reone/tools/script/expressiontree.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(expression_tree)

BOOST_AUTO_TEST_CASE(should_decompile_program__minimal) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction(InstructionType::RETN));

    auto routines = Routines(*static_cast<Game *>(nullptr), *static_cast<ServicesView *>(nullptr));
    routines.initForKotOR();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_TEST(0ll == globals.size());

    auto &functions = tree.functions();
    BOOST_TEST(1ll == functions.size());

    auto &startFunc = functions[0];
    BOOST_TEST("_start" == startFunc->name);
    BOOST_TEST(0ll == startFunc->inputs.size());
    BOOST_TEST(0ll == startFunc->outputs.size());
    BOOST_TEST(static_cast<int>(VariableType::Void) == static_cast<int>(startFunc->returnType));
    BOOST_TEST(1ll == startFunc->block->expressions.size());
}

BOOST_AUTO_TEST_CASE(should_decompile_program__starting_conditional_without_globals) {
    // given

    auto routines = Routines(*static_cast<Game *>(nullptr), *static_cast<ServicesView *>(nullptr));
    routines.initForKotOR();

    auto program = ScriptProgram("");
    program.add(Instruction(InstructionType::RSADDI));
    program.add(Instruction::newJSR(8));
    program.add(Instruction(InstructionType::RETN));
    program.add(Instruction::newCONSTI(1));
    program.add(Instruction::newCPDOWNSP(-8, 4));
    program.add(Instruction::newMOVSP(-4));
    program.add(Instruction(InstructionType::RETN));

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_TEST(0ll == globals.size());

    auto &functions = tree.functions();
    BOOST_TEST(2ll == functions.size());

    auto startingConditionalFunc = functions[0];
    BOOST_TEST("StartingConditional" == startingConditionalFunc->name);
    BOOST_TEST(0ll == startingConditionalFunc->inputs.size());
    BOOST_TEST(1ll == startingConditionalFunc->outputs.size());
    BOOST_TEST(static_cast<int>(VariableType::Int) == static_cast<int>(startingConditionalFunc->outputs[0].type));
    BOOST_TEST(static_cast<int>(VariableType::Void) == static_cast<int>(startingConditionalFunc->returnType));

    auto startFunc = functions[1];
    BOOST_TEST("_start" == startFunc->name);
}

BOOST_AUTO_TEST_CASE(should_decompile_program__main_with_globals) {
    // given

    auto routines = Routines(*static_cast<Game *>(nullptr), *static_cast<ServicesView *>(nullptr));
    routines.initForKotOR();

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

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    auto globalsVec = vector<const ExpressionTree::ParameterExpression *>(globals.begin(), globals.end());
    BOOST_TEST(1ll == globalsVec.size());
    BOOST_TEST(static_cast<int>(VariableType::Int) == static_cast<int>(globalsVec[0]->variableType));
    BOOST_TEST(static_cast<int>(ExpressionTree::ParameterLocality::Global) == static_cast<int>(globalsVec[0]->locality));

    auto &functions = tree.functions();
    BOOST_TEST(3ll == functions.size());

    auto mainFunc = functions[0];
    BOOST_TEST("main" == mainFunc->name);
    BOOST_TEST(0ll == mainFunc->inputs.size());
    BOOST_TEST(0ll == mainFunc->outputs.size());
    BOOST_TEST(static_cast<int>(VariableType::Void) == static_cast<int>(mainFunc->returnType));

    auto globalsFunc = functions[1];
    BOOST_TEST("_globals" == globalsFunc->name);

    auto startFunc = functions[2];
    BOOST_TEST("_start" == startFunc->name);
}

BOOST_AUTO_TEST_CASE(should_decompile_program__conditionals) {
    // given

    auto routines = Routines(*static_cast<Game *>(nullptr), *static_cast<ServicesView *>(nullptr));
    routines.initForKotOR();

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

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_TEST(0ll == globals.size());

    auto &functions = tree.functions();
    BOOST_TEST(2ll == functions.size());

    auto mainFunc = functions[0];
    BOOST_TEST("main" == mainFunc->name);
    BOOST_TEST(0ll == mainFunc->inputs.size());
    BOOST_TEST(0ll == mainFunc->outputs.size());
    BOOST_TEST(static_cast<int>(VariableType::Void) == static_cast<int>(mainFunc->returnType));
    BOOST_TEST(11 == mainFunc->block->expressions.size());
    // loc_jnz:
    // return;
    BOOST_TEST(static_cast<int>(ExpressionType::Label) == static_cast<int>(mainFunc->block->expressions[7]->type));
    auto jnzLabelExpr = static_cast<ExpressionTree::LabelExpression *>(mainFunc->block->expressions[7]);
    BOOST_TEST(static_cast<int>(ExpressionType::Return) == static_cast<int>(mainFunc->block->expressions[8]->type));
    // loc_jz:
    // return;
    BOOST_TEST(static_cast<int>(ExpressionType::Label) == static_cast<int>(mainFunc->block->expressions[9]->type));
    auto jzLabelExpr = static_cast<ExpressionTree::LabelExpression *>(mainFunc->block->expressions[9]);
    BOOST_TEST(static_cast<int>(ExpressionType::Return) == static_cast<int>(mainFunc->block->expressions[10]->type));
    // int a = 2;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[0]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Parameter) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[0])->left->type));
    auto aExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[0])->left);
    BOOST_TEST(static_cast<int>(ExpressionType::Constant) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[0])->right->type));
    // int b = 1;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[1]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Parameter) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[1])->left->type));
    auto bExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[1])->left);
    BOOST_TEST(static_cast<int>(ExpressionType::Constant) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[1])->right->type));
    // int c = a;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[2]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Parameter) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->left->type));
    auto cExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->left);
    BOOST_TEST(static_cast<int>(ExpressionType::Parameter) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->right->type));
    BOOST_TEST(aExpr == static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->right);
    // if(c == 0) { goto loc_jz; }
    BOOST_TEST(static_cast<int>(ExpressionType::Conditional) == static_cast<int>(mainFunc->block->expressions[3]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Equal) == static_cast<int>(static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[3])->test->type));
    auto jzIfTrue = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[3])->ifTrue;
    BOOST_TEST(jzIfTrue);
    BOOST_TEST(1ll == jzIfTrue->expressions.size());
    BOOST_TEST(static_cast<int>(ExpressionType::Goto) == static_cast<int>(jzIfTrue->expressions[0]->type));
    BOOST_TEST(jzLabelExpr == static_cast<ExpressionTree::GotoExpression *>(jzIfTrue->expressions[0])->label);
    auto jzIfFalse = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[3])->ifFalse;
    BOOST_TEST(!jzIfFalse);
    // int d = a / b;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[4]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Parameter) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->left->type));
    auto dExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->left);
    BOOST_TEST(static_cast<int>(ExpressionType::Divide) == static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->right->type));
    auto aDivBExpr = static_cast<ExpressionTree::BinaryExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->right);
    BOOST_TEST(static_cast<int>(ExpressionType::Parameter) == static_cast<int>(aDivBExpr->left->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Parameter) == static_cast<int>(aDivBExpr->right->type));
    BOOST_TEST(aExpr == aDivBExpr->left);
    BOOST_TEST(bExpr == aDivBExpr->right);
    // if(d != 0) { goto loc_jnz; }
    BOOST_TEST(static_cast<int>(ExpressionType::Conditional) == static_cast<int>(mainFunc->block->expressions[5]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::NotEqual) == static_cast<int>(static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[5])->test->type));
    auto jnzIfTrue = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[5])->ifTrue;
    BOOST_TEST(jnzIfTrue);
    BOOST_TEST(1ll == jnzIfTrue->expressions.size());
    BOOST_TEST(static_cast<int>(ExpressionType::Goto) == static_cast<int>(jnzIfTrue->expressions[0]->type));
    BOOST_TEST(jnzLabelExpr == static_cast<ExpressionTree::GotoExpression *>(jnzIfTrue->expressions[0])->label);
    auto jnzIfFalse = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[5])->ifFalse;
    BOOST_TEST(!jnzIfFalse);
    // return;
    BOOST_TEST(static_cast<int>(ExpressionType::Return) == static_cast<int>(mainFunc->block->expressions[6]->type));

    auto startFunc = functions[1];
    BOOST_TEST("_start" == startFunc->name);
}

BOOST_AUTO_TEST_CASE(should_decompile_program__loop) {
    // given

    auto routines = Routines(*static_cast<Game *>(nullptr), *static_cast<ServicesView *>(nullptr));
    routines.initForKotOR();

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

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_TEST(0ll == globals.size());

    auto &functions = tree.functions();
    BOOST_TEST(2ll == functions.size());

    auto mainFunc = functions[0];
    BOOST_TEST("main" == mainFunc->name);
    BOOST_TEST(0ll == mainFunc->inputs.size());
    BOOST_TEST(0ll == mainFunc->outputs.size());
    BOOST_TEST(static_cast<int>(VariableType::Void) == static_cast<int>(mainFunc->returnType));

    auto startFunc = functions[1];
    BOOST_TEST("_start" == startFunc->name);
}

BOOST_AUTO_TEST_CASE(should_decompile_program__vectors) {
    // given

    auto routines = Routines(*static_cast<Game *>(nullptr), *static_cast<ServicesView *>(nullptr));
    routines.initForKotOR();

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

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_TEST(0ll == globals.size());

    auto &functions = tree.functions();
    BOOST_TEST(2ll == functions.size());

    auto mainFunc = functions[0];
    BOOST_TEST("main" == mainFunc->name);
    BOOST_TEST(0ll == mainFunc->inputs.size());
    BOOST_TEST(0ll == mainFunc->outputs.size());
    BOOST_TEST(static_cast<int>(VariableType::Void) == static_cast<int>(mainFunc->returnType));
    BOOST_TEST(71ll == mainFunc->block->expressions.size());
    // vector v1 = Vector(1.0f, 2.0f, 3.0f);
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[0]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[1]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[2]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[3]->type));
    auto v1Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[3]);
    BOOST_TEST(static_cast<int>(ExpressionType::Action) == static_cast<int>(v1Assign->right->type));
    auto v1Action = static_cast<ExpressionTree::ActionExpression *>(v1Assign->right);
    BOOST_TEST(142 == v1Action->action);
    BOOST_TEST(3ll == v1Action->arguments.size());
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[4]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[5]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[6]->type));
    // vector v2 = Vector(-3.0f, -2.0f, -1.0f);
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[7]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[8]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[9]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[10]->type));
    auto v2Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[10]);
    BOOST_TEST(static_cast<int>(ExpressionType::Action) == static_cast<int>(v2Assign->right->type));
    auto v2Action = static_cast<ExpressionTree::ActionExpression *>(v2Assign->right);
    BOOST_TEST(142 == v2Action->action);
    BOOST_TEST(3ll == v2Action->arguments.size());
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[11]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[12]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[13]->type));
    // vector v3 = v1 + v2;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[14]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[15]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[16]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[17]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[18]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[19]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[20]->type));
    auto v3Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[20]);
    BOOST_TEST(static_cast<int>(ExpressionType::Add) == static_cast<int>(v3Assign->right->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[21]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[22]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[23]->type));
    // vector v4 = v1 - v2;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[24]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[25]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[26]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[27]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[28]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[29]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[30]->type));
    auto v4Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[30]);
    BOOST_TEST(static_cast<int>(ExpressionType::Subtract) == static_cast<int>(v4Assign->right->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[31]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[32]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[33]->type));
    // vector v5 = 2.0f * v1;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[34]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[35]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[36]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[37]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[38]->type));
    auto v5Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[38]);
    BOOST_TEST(static_cast<int>(ExpressionType::Multiply) == static_cast<int>(v5Assign->right->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[39]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[40]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[41]->type));
    // vector v6 = 2.0f / v1;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[42]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[43]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[44]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[45]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[46]->type));
    auto v6Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[46]);
    BOOST_TEST(static_cast<int>(ExpressionType::Divide) == static_cast<int>(v6Assign->right->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[47]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[48]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[49]->type));
    // vector v7 = v1 * 2.0f;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[50]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[51]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[52]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[53]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[54]->type));
    auto v7Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[54]);
    BOOST_TEST(static_cast<int>(ExpressionType::Multiply) == static_cast<int>(v7Assign->right->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[55]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[56]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[57]->type));
    // vector v8 = v1 / 2.0f;
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[58]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[59]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[60]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[61]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[62]->type));
    auto v8Assign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[62]);
    BOOST_TEST(static_cast<int>(ExpressionType::Divide) == static_cast<int>(v8Assign->right->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[63]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[64]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[65]->type));
    // float f = VectorToAngle(v1);
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[66]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[67]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[68]->type));
    BOOST_TEST(static_cast<int>(ExpressionType::Assign) == static_cast<int>(mainFunc->block->expressions[69]->type));
    auto fAssign = static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[69]);
    BOOST_TEST(static_cast<int>(ExpressionType::Action) == static_cast<int>(fAssign->right->type));
    auto fAction = static_cast<ExpressionTree::ActionExpression *>(fAssign->right);
    BOOST_TEST(145 == fAction->action);
    BOOST_TEST(1ll == fAction->arguments.size());
    BOOST_TEST(static_cast<int>(ExpressionType::Vector) == static_cast<int>(fAction->arguments[0]->type));
    // return;
    BOOST_TEST(static_cast<int>(ExpressionType::Return) == static_cast<int>(mainFunc->block->expressions[70]->type));

    auto startFunc = functions[1];
    BOOST_TEST("_start" == startFunc->name);
}

BOOST_AUTO_TEST_SUITE_END()
