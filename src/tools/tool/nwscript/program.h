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

#pragma once

#include "../../../script/program.h"
#include "../../../script/variable.h"

namespace reone {

namespace script {

class IRoutines;

}

class NwscriptProgram {
public:
    enum ExpressionType {
        Block,
        Constant,
        Parameter,
        Label,
        Goto,
        Return,
        Conditional,
        Action,
        Call,

        // Unary

        Negate,
        Not,

        // END Unary

        // Binary

        Assign,

        Add,
        Subtract,
        Multiply,
        Divide,

        And,
        Or,

        Equal,
        NotEqual,
        GreaterThanOrEqual,
        GreaterThan,
        LessThan,
        LessThanOrEqual

        // END Binary
    };

    struct Expression {
        ExpressionType type;
        uint32_t offset {0};
        int index {0};

        Expression(ExpressionType type) :
            type(type) {
        }
    };

    struct BlockExpression : Expression {
        std::vector<Expression *> expressions;

        BlockExpression() :
            Expression(ExpressionType::Block) {
        }
    };

    struct Function {
        std::string name;
        uint32_t offset {0};
        BlockExpression *block {nullptr};
    };

    struct ConstantExpression : Expression {
        script::Variable value;

        ConstantExpression() :
            Expression(ExpressionType::Constant) {
        }
    };

    struct ParameterExpression : Expression {
        script::VariableType variableType {script::VariableType::Int};

        ParameterExpression() :
            Expression(ExpressionType::Parameter) {
        }
    };

    struct UnaryExpression : Expression {
        Expression *operand {nullptr};

        UnaryExpression(ExpressionType type) :
            Expression(type) {
        }
    };

    struct BinaryExpression : Expression {
        Expression *left {nullptr};
        Expression *right {nullptr};

        BinaryExpression(ExpressionType type) :
            Expression(type) {
        }
    };

    struct LabelExpression : Expression {
        LabelExpression() :
            Expression(ExpressionType::Label) {
        }
    };

    struct GotoExpression : Expression {
        LabelExpression *label {nullptr};

        GotoExpression() :
            Expression(ExpressionType::Goto) {
        }
    };

    struct ReturnExpression : Expression {
        ReturnExpression() :
            Expression(ExpressionType::Return) {
        }
    };

    struct ConditionalExpression : Expression {
        Expression *test {nullptr};
        BlockExpression *ifTrue {nullptr};
        BlockExpression *ifFalse {nullptr};

        ConditionalExpression() :
            Expression(ExpressionType::Conditional) {
        }
    };

    struct ActionExpression : Expression {
        int action {0};
        std::vector<ParameterExpression *> arguments;

        ActionExpression() :
            Expression(ExpressionType::Action) {
        }
    };

    struct CallExpression : Expression {
        Function *function {nullptr};
        std::vector<Expression *> arguments;

        CallExpression() :
            Expression(ExpressionType::Call) {
        }
    };

    NwscriptProgram(
        std::vector<std::shared_ptr<Function>> functions,
        std::vector<std::shared_ptr<Expression>> expressions) :
        _functions(std::move(functions)),
        _expressions(std::move(expressions)) {
    }

    const std::vector<std::shared_ptr<Function>> &functions() const {
        return _functions;
    }

    const std::vector<std::shared_ptr<Expression>> &expressions() const {
        return _expressions;
    }

    static NwscriptProgram fromCompiled(const script::ScriptProgram &compiled, const script::IRoutines &routines);

private:
    struct DecompilationContext {
        const script::ScriptProgram &compiled;
        const script::IRoutines &routines;
        std::vector<std::shared_ptr<Function>> &functions;
        std::vector<std::shared_ptr<Expression>> &expressions;

        std::deque<std::pair<ParameterExpression *, int>> stack;

        DecompilationContext(
            const script::ScriptProgram &compiled,
            const script::IRoutines &routines,
            std::vector<std::shared_ptr<Function>> &functions,
            std::vector<std::shared_ptr<Expression>> &expressions) :
            compiled(compiled),
            routines(routines),
            functions(functions),
            expressions(expressions) {
        }
    };

    std::vector<std::shared_ptr<Function>> _functions;
    std::vector<std::shared_ptr<Expression>> _expressions;

    static BlockExpression *decompile(uint32_t start, DecompilationContext &ctx);

    static std::unique_ptr<ConstantExpression> constantExpression(const script::Instruction &ins);
    static std::unique_ptr<ParameterExpression> parameterExpression(const script::Instruction &ins);
};

} // namespace reone