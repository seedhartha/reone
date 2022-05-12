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

using namespace std;

using namespace reone::script;

namespace reone {

NwscriptProgram NwscriptProgram::fromCompiled(const ScriptProgram &compiled) {
    auto decompilationCtx = DecompilationContext();

    auto funcMain = make_shared<Function>();
    funcMain->name = "main";
    funcMain->offset = 13;
    funcMain->block = decompile(13, compiled, decompilationCtx);

    decompilationCtx.functions.push_back(move(funcMain));

    return NwscriptProgram(
        decompilationCtx.functions,
        decompilationCtx.expressions);
}

NwscriptProgram::BlockExpression *NwscriptProgram::decompile(uint32_t start, const ScriptProgram &compiled, DecompilationContext &ctx) {
    auto block = make_shared<BlockExpression>();
    block->offset = start;

    for (uint32_t offset = start; offset < compiled.length();) {
        const Instruction &ins = compiled.getInstruction(offset);

        if (ins.type == InstructionType::RETN) {
            break;

        } else if (ins.type == InstructionType::JMP) {
            // decompile(ins.jumpOffset, compiled, ctx);

        } else if (ins.type == InstructionType::JSR) {
            auto sub = make_shared<Function>();
            sub->offset = ins.jumpOffset;
            sub->block = decompile(ins.offset + ins.jumpOffset, compiled, ctx);

            auto callExpr = make_shared<CallExpression>();
            callExpr->offset = ins.offset;
            callExpr->function = sub.get();

            block->expressions.push_back(callExpr.get());

            ctx.functions.push_back(move(sub));
            ctx.expressions.push_back(move(callExpr));

        } else if (ins.type == InstructionType::JZ) {
            // auto ifTrue = decompile(ins.jumpOffset, compiled, ctx);
            // auto ifFalse = decompile(ins.nextOffset, compiled, ctx);

        } else if (ins.type == InstructionType::JNZ) {
            // auto ifTrue = decompile(ins.jumpOffset, compiled, ctx);
            // auto ifFalse = decompile(ins.nextOffset, compiled, ctx);

        } else if (ins.type == InstructionType::CONSTI ||
                   ins.type == InstructionType::CONSTF ||
                   ins.type == InstructionType::CONSTS ||
                   ins.type == InstructionType::CONSTO) {
            auto expression = constantExpression(ins);
            if (expression) {
                block->expressions.push_back(expression.get());
                ctx.expressions.push_back(move(expression));
            }
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