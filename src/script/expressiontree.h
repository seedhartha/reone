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

#include "program.h"
#include "variable.h"

namespace reone {

namespace script {

class IRoutines;

class ExpressionTree {
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
        OnesComplement,

        Increment,
        Decrement,

        // END Unary

        // Binary

        Assign,

        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,

        LogicalAnd,
        LogicalOr,
        BitwiseOr,
        BitwiseExlusiveOr,
        BitwiseAnd,

        LeftShift,
        RightShift,
        RightShiftUnsigned,

        Equal,
        NotEqual,
        GreaterThanOrEqual,
        GreaterThan,
        LessThan,
        LessThanOrEqual

        // END Binary
    };

    enum class ParameterLocality {
        Local,
        Input,
        Output,
        Global
    };

    struct Expression {
        ExpressionType type;
        uint32_t offset {0};

        Expression(ExpressionType type) :
            type(type) {
        }
    };

    struct BlockExpression : Expression {
        std::vector<Expression *> expressions;

        BlockExpression() :
            Expression(ExpressionType::Block) {
        }

        bool contains(uint32_t offset) {
            return !expressions.empty() &&
                   offset >= expressions.front()->offset &&
                   offset <= expressions.back()->offset;
        }

        void insert(Expression *e) {
            expressions.push_back(e);
        }
    };

    struct ConstantExpression : Expression {
        Variable value;

        ConstantExpression() :
            Expression(ExpressionType::Constant) {
        }
    };

    struct ParameterExpression : Expression {
        VariableType variableType {VariableType::Int};
        ParameterLocality locality {ParameterLocality::Local};
        int index {0};

        ParameterExpression() :
            Expression(ExpressionType::Parameter) {
        }
    };

    struct UnaryExpression : Expression {
        ParameterExpression *operand {nullptr};

        UnaryExpression(ExpressionType type) :
            Expression(type) {
        }
    };

    struct BinaryExpression : Expression {
        Expression *left {nullptr};
        Expression *right {nullptr};
        bool declareLeft {false};

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
        Expression *value {nullptr};

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
        std::vector<Expression *> arguments;

        ActionExpression() :
            Expression(ExpressionType::Action) {
        }
    };

    struct Function {
        std::string name;
        uint32_t offset {0};
        VariableType returnType {VariableType::Void};
        std::vector<VariableType> inArgumentTypes;
        std::vector<VariableType> outArgumentTypes;
        BlockExpression *block {nullptr};
    };

    struct CallExpression : Expression {
        Function *function {nullptr};
        std::vector<ParameterExpression *> arguments;

        CallExpression() :
            Expression(ExpressionType::Call) {
        }
    };

    ExpressionTree(
        std::vector<std::shared_ptr<Function>> functions,
        std::vector<std::shared_ptr<Expression>> expressions,
        std::set<const ParameterExpression *> globals) :
        _functions(std::move(functions)),
        _expressions(std::move(expressions)),
        _globals(std::move(globals)) {
    }

    const std::vector<std::shared_ptr<Function>> &functions() const {
        return _functions;
    }

    const std::vector<std::shared_ptr<Expression>> &expressions() const {
        return _expressions;
    }

    const std::set<const ParameterExpression *> &globals() const {
        return _globals;
    }

    static ExpressionTree fromProgram(const ScriptProgram &program, const IRoutines &routines);

private:
    struct CallStackFrame {
        Function *function {nullptr};

        CallStackFrame(Function *function) :
            function(function) {
        }
    };

    struct StackFrame {
        Function *allocatedBy {nullptr};
        ParameterExpression *param {nullptr};
        int component {0}; // XYZ (vector)

        StackFrame(
            Function *allocatedBy,
            ParameterExpression *param,
            int component) :
            allocatedBy(allocatedBy),
            param(param),
            component(component) {
        }

        StackFrame(const StackFrame &other) {
            allocatedBy = other.allocatedBy;
            param = other.param;
            component = other.component;
        }

        StackFrame withAllocatedBy(Function &allocatedBy) {
            auto copy = StackFrame(*this);
            copy.allocatedBy = &allocatedBy;
            return std::move(copy);
        }
    };

    struct DecompilationContext {
        const ScriptProgram &compiled;
        const IRoutines &routines;
        const std::unordered_map<uint32_t, LabelExpression *> &labels;
        std::vector<std::shared_ptr<Function>> &functions;
        std::vector<std::shared_ptr<Expression>> &expressions;

        std::vector<CallStackFrame> callStack;
        std::vector<StackFrame> stack;
        int numGlobals {0};
        int prevNumGlobals {0};
        BlockExpression *savedAction {nullptr};

        std::vector<ParameterExpression *> *inputs {nullptr};
        std::vector<ParameterExpression *> *outputs {nullptr};
        std::map<uint32_t, BlockExpression *> *branches {nullptr};

        DecompilationContext(
            const ScriptProgram &compiled,
            const IRoutines &routines,
            const std::unordered_map<uint32_t, LabelExpression *> &labels,
            std::vector<std::shared_ptr<Function>> &functions,
            std::vector<std::shared_ptr<Expression>> &expressions) :
            compiled(compiled),
            routines(routines),
            labels(labels),
            functions(functions),
            expressions(expressions) {
        }

        DecompilationContext(const DecompilationContext &other) :
            compiled(other.compiled),
            routines(other.routines),
            labels(other.labels),
            functions(other.functions),
            expressions(other.expressions),
            callStack(other.callStack),
            stack(other.stack),
            numGlobals(other.numGlobals),
            prevNumGlobals(other.prevNumGlobals),
            savedAction(other.savedAction),
            inputs(other.inputs),
            outputs(other.outputs),
            branches(other.branches) {
        }
    };

    std::vector<std::shared_ptr<Function>> _functions;
    std::vector<std::shared_ptr<Expression>> _expressions;
    std::set<const ParameterExpression *> _globals;

    static BlockExpression *decompile(uint32_t start, DecompilationContext &ctx);

    static std::unique_ptr<ConstantExpression> constantExpression(const Instruction &ins);
    static std::unique_ptr<ParameterExpression> parameterExpression(const Instruction &ins);
};

} // namespace script

} // namespace reone
