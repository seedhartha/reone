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

using namespace std;

using namespace reone::script;

namespace reone {

NwscriptProgram NwscriptProgram::fromCompiled(const ScriptProgram &compiled) {
    auto functions = vector<shared_ptr<Function>>();
    auto expressions = vector<shared_ptr<Expression>>();

    // main function
    auto exprMainBlock = make_unique<BlockExpression>();
    for (auto &instruction : compiled.instructions()) {
        auto expression = expressionFromInstruction(instruction);
        if (!expression) {
            continue;
        }
        exprMainBlock->expressions.push_back(expression.get());
        expressions.push_back(move(expression));
    }
    auto fnMain = make_unique<Function>(Function {"main", 0, nullptr});
    fnMain->block = exprMainBlock.get();

    functions.push_back(move(fnMain));
    expressions.push_back(move(exprMainBlock));

    return NwscriptProgram(move(functions), move(expressions));
}

unique_ptr<NwscriptProgram::Expression> NwscriptProgram::expressionFromInstruction(const Instruction &instr) {
    unique_ptr<Expression> expression;
    switch (instr.type) {
    case InstructionType::CONSTI:
    case InstructionType::CONSTF:
    case InstructionType::CONSTS:
    case InstructionType::CONSTO: {
        auto constExpr = make_unique<ConstantExpression>();
        constExpr->offset = instr.offset;
        if (instr.type == InstructionType::CONSTI) {
            constExpr->value = Variable::ofInt(instr.intValue);
        } else if (instr.type == InstructionType::CONSTF) {
            constExpr->value = Variable::ofFloat(instr.floatValue);
        } else if (instr.type == InstructionType::CONSTS) {
            constExpr->value = Variable::ofString(instr.strValue);
        } else if (instr.type == InstructionType::CONSTO) {
            constExpr->value = Variable::ofObject(instr.objectId);
        }
        expression = move(constExpr);
        break;
    }
    default:
        break;
    }
    return move(expression);
}

} // namespace reone