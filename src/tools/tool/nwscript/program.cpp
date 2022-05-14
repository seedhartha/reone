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

#include "program.h"

#include "../../../common/exception/argument.h"
#include "../../../common/exception/notimplemented.h"
#include "../../../script/routine.h"
#include "../../../script/routines.h"

using namespace std;

using namespace reone::script;

namespace reone {

NwscriptProgram NwscriptProgram::fromCompiled(const ScriptProgram &compiled, const IRoutines &routines) {
    auto functions = vector<shared_ptr<Function>>();
    auto expressions = vector<shared_ptr<Expression>>();
    auto decompilationCtx = DecompilationContext(compiled, routines, functions, expressions);

    auto funcMain = make_shared<Function>();
    funcMain->name = "main";
    funcMain->offset = 13;
    funcMain->block = decompile(13, decompilationCtx);

    decompilationCtx.functions.push_back(move(funcMain));

    return NwscriptProgram(
        decompilationCtx.functions,
        decompilationCtx.expressions);
}

NwscriptProgram::BlockExpression *NwscriptProgram::decompile(uint32_t start, DecompilationContext &ctx) {
    auto block = make_shared<BlockExpression>();
    block->offset = start;

    for (uint32_t offset = start; offset < ctx.compiled.length();) {
        const Instruction &ins = ctx.compiled.getInstruction(offset);

        if (ins.type == InstructionType::RETN) {
            auto retExpr = make_shared<ReturnExpression>();
            block->expressions.push_back(retExpr.get());
            ctx.expressions.push_back(move(retExpr));
            break;

        } else if (ins.type == InstructionType::JMP) {
            if (ins.jumpOffset < 0) {
                throw NotImplementedException("Negative jump offsets are not supported yet");
            }
            offset = ins.offset + ins.jumpOffset;
            continue;

        } else if (ins.type == InstructionType::JSR) {
            if (ins.jumpOffset < 0) {
                throw NotImplementedException("Negative jump offsets are not supported yet");
            }
            auto sub = make_shared<Function>();
            sub->offset = ins.offset + ins.jumpOffset;
            sub->block = decompile(ins.offset + ins.jumpOffset, ctx);

            auto callExpr = make_shared<CallExpression>();
            callExpr->offset = ins.offset;
            callExpr->function = sub.get();
            block->expressions.push_back(callExpr.get());

            ctx.functions.push_back(move(sub));
            ctx.expressions.push_back(move(callExpr));

        } else if (ins.type == InstructionType::JZ ||
                   ins.type == InstructionType::JNZ) {
            if (ins.jumpOffset < 0) {
                throw NotImplementedException("Negative jump offsets are not supported yet");
            }
            auto operand = ctx.stack.top();
            ctx.stack.pop();

            auto testExpr = make_shared<UnaryExpression>(ins.type == InstructionType::JZ ? ExpressionType::Zero : ExpressionType::NotZero);
            testExpr->offset = ins.offset;
            testExpr->operand = operand;

            auto condExpr = make_shared<ConditionalExpression>();
            condExpr->test = testExpr.get();
            auto trueCtx = DecompilationContext(ctx);
            condExpr->ifTrue = decompile(ins.offset + ins.jumpOffset, trueCtx);
            auto falseCtx = DecompilationContext(ctx);
            condExpr->ifFalse = decompile(ins.nextOffset, falseCtx);
            block->expressions.push_back(condExpr.get());

            ctx.expressions.push_back(move(testExpr));
            ctx.expressions.push_back(move(condExpr));
            break;

        } else if (ins.type == InstructionType::CONSTI ||
                   ins.type == InstructionType::CONSTF ||
                   ins.type == InstructionType::CONSTS ||
                   ins.type == InstructionType::CONSTO) {
            auto expression = constantExpression(ins);
            if (expression) {
                block->expressions.push_back(expression.get());
                ctx.stack.push(expression.get());
                ctx.expressions.push_back(move(expression));
            }

        } else if (ins.type == InstructionType::ACTION) {
            vector<Expression *> arguments;
            for (int i = 0; i < ins.argCount; ++i) {
                arguments.push_back(ctx.stack.top());
                ctx.stack.pop();
            }

            auto actionExpr = make_shared<ActionExpression>();
            actionExpr->offset = ins.offset;
            actionExpr->action = ins.routine;
            actionExpr->arguments = move(arguments);

            auto &routine = ctx.routines.get(ins.routine);
            if (routine.returnType() != VariableType::Void) {
                auto retValExpr = make_shared<ParameterExpression>();
                retValExpr->offset = ins.offset;
                retValExpr->variableType = routine.returnType();
                block->expressions.push_back(retValExpr.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = retValExpr.get();
                assignExpr->right = actionExpr.get();
                block->expressions.push_back(assignExpr.get());

                ctx.stack.push(retValExpr.get());
                ctx.expressions.push_back(move(retValExpr));
                ctx.expressions.push_back(move(assignExpr));

            } else {
                block->expressions.push_back(actionExpr.get());
            }

            ctx.expressions.push_back(move(actionExpr));

        } else {
            throw ArgumentException("Cannot decompile expression of type " + to_string(static_cast<int>(ins.type)));
        }

        offset = ins.nextOffset;
    }

    ctx.expressions.push_back(block);

    return block.get();
}

unique_ptr<NwscriptProgram::ConstantExpression> NwscriptProgram::constantExpression(const Instruction &ins) {
    switch (ins.type) {
    case InstructionType::CONSTI:
    case InstructionType::CONSTF:
    case InstructionType::CONSTS:
    case InstructionType::CONSTO: {
        auto constExpr = make_unique<ConstantExpression>();
        constExpr->offset = ins.offset;
        if (ins.type == InstructionType::CONSTI) {
            constExpr->value = Variable::ofInt(ins.intValue);
        } else if (ins.type == InstructionType::CONSTF) {
            constExpr->value = Variable::ofFloat(ins.floatValue);
        } else if (ins.type == InstructionType::CONSTS) {
            constExpr->value = Variable::ofString(ins.strValue);
        } else if (ins.type == InstructionType::CONSTO) {
            constExpr->value = Variable::ofObject(ins.objectId);
        }
        return move(constExpr);
    }
    default:
        throw ArgumentException("Instruction is not of CONSTx type: " + to_string(static_cast<int>(ins.type)));
    }
}

} // namespace reone