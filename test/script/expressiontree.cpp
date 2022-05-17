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

    auto routines = Routines();
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

    auto routines = Routines();
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
    BOOST_CHECK_EQUAL(1ll, globalsVec.size());
    BOOST_CHECK_EQUAL(static_cast<int>(VariableType::Int), static_cast<int>(globalsVec[0]->variableType));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionTree::ParameterLocality::Global), static_cast<int>(globalsVec[0]->locality));

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

BOOST_AUTO_TEST_CASE(should_decompile_program__conditionals) {
    // given

    auto routines = Routines();
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
    BOOST_CHECK_EQUAL(0ll, globals.size());

    auto &functions = tree.functions();
    BOOST_CHECK_EQUAL(2ll, functions.size());

    auto mainFunc = functions[0];
    BOOST_CHECK_EQUAL("main", mainFunc->name);
    BOOST_CHECK_EQUAL(0ll, mainFunc->inArgumentTypes.size());
    BOOST_CHECK_EQUAL(0ll, mainFunc->outArgumentTypes.size());
    BOOST_CHECK_EQUAL(static_cast<int>(VariableType::Void), static_cast<int>(mainFunc->returnType));
    BOOST_CHECK_EQUAL(11, mainFunc->block->expressions.size());
    // loc_jnz:
    // return;
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Label), static_cast<int>(mainFunc->block->expressions[7]->type));
    auto jnzLabelExpr = static_cast<ExpressionTree::LabelExpression *>(mainFunc->block->expressions[7]);
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Return), static_cast<int>(mainFunc->block->expressions[8]->type));
    // loc_jz:
    // return;
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Label), static_cast<int>(mainFunc->block->expressions[9]->type));
    auto jzLabelExpr = static_cast<ExpressionTree::LabelExpression *>(mainFunc->block->expressions[9]);
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Return), static_cast<int>(mainFunc->block->expressions[10]->type));
    // int a = 2;
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Assign), static_cast<int>(mainFunc->block->expressions[0]->type));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Parameter), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[0])->left->type));
    auto aExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[0])->left);
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Constant), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[0])->right->type));
    // int b = 1;
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Assign), static_cast<int>(mainFunc->block->expressions[1]->type));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Parameter), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[1])->left->type));
    auto bExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[1])->left);
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Constant), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[1])->right->type));
    // int c = a;
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Assign), static_cast<int>(mainFunc->block->expressions[2]->type));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Parameter), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->left->type));
    auto cExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->left);
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Parameter), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->right->type));
    BOOST_CHECK_EQUAL(aExpr, static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[2])->right);
    // if(c == 0) { goto loc_jz; }
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Conditional), static_cast<int>(mainFunc->block->expressions[3]->type));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Equal), static_cast<int>(static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[3])->test->type));
    auto jzIfTrue = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[3])->ifTrue;
    BOOST_CHECK(jzIfTrue);
    BOOST_CHECK_EQUAL(1ll, jzIfTrue->expressions.size());
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Goto), static_cast<int>(jzIfTrue->expressions[0]->type));
    BOOST_CHECK_EQUAL(jzLabelExpr, static_cast<ExpressionTree::GotoExpression *>(jzIfTrue->expressions[0])->label);
    auto jzIfFalse = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[3])->ifFalse;
    BOOST_CHECK(!jzIfFalse);
    // int d = a / b;
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Assign), static_cast<int>(mainFunc->block->expressions[4]->type));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Parameter), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->left->type));
    auto dExpr = static_cast<ExpressionTree::ParameterExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->left);
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Divide), static_cast<int>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->right->type));
    auto aDivBExpr = static_cast<ExpressionTree::BinaryExpression *>(static_cast<ExpressionTree::BinaryExpression *>(mainFunc->block->expressions[4])->right);
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Parameter), static_cast<int>(aDivBExpr->left->type));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Parameter), static_cast<int>(aDivBExpr->right->type));
    BOOST_CHECK_EQUAL(aExpr, aDivBExpr->left);
    BOOST_CHECK_EQUAL(bExpr, aDivBExpr->right);
    // if(d != 0) { goto loc_jnz; }
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Conditional), static_cast<int>(mainFunc->block->expressions[5]->type));
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::NotEqual), static_cast<int>(static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[5])->test->type));
    auto jnzIfTrue = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[5])->ifTrue;
    BOOST_CHECK(jnzIfTrue);
    BOOST_CHECK_EQUAL(1ll, jnzIfTrue->expressions.size());
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Goto), static_cast<int>(jnzIfTrue->expressions[0]->type));
    BOOST_CHECK_EQUAL(jnzLabelExpr, static_cast<ExpressionTree::GotoExpression *>(jnzIfTrue->expressions[0])->label);
    auto jnzIfFalse = static_cast<ExpressionTree::ConditionalExpression *>(mainFunc->block->expressions[5])->ifFalse;
    BOOST_CHECK(!jnzIfFalse);
    // return;
    BOOST_CHECK_EQUAL(static_cast<int>(ExpressionType::Return), static_cast<int>(mainFunc->block->expressions[6]->type));

    auto startFunc = functions[1];
    BOOST_CHECK_EQUAL("_start", startFunc->name);
}

BOOST_AUTO_TEST_SUITE_END()
