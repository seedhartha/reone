/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/script/program.h"
#include "reone/script/variable.h"

namespace reone {

namespace script {

class IRoutines;

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
    Vector,
    VectorIndex,

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
    LessThanOrEqual,

    // END Binary
};

class ExpressionTree {
public:
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

        void append(Expression *e) {
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
        std::string suffix;
        int stackOffset {0}; // input/output

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

    struct FunctionArgument {
        VariableType type;
        int stackOffset;

        FunctionArgument(VariableType type, int stackOffset) :
            type(type),
            stackOffset(stackOffset) {
        }
    };

    struct Function {
        std::string name;
        uint32_t offset {0};
        std::vector<FunctionArgument> inputs;
        std::vector<FunctionArgument> outputs;
        VariableType returnType {VariableType::Void};
        BlockExpression *block {nullptr};
    };

    struct CallExpression : Expression {
        Function *function {nullptr};
        std::vector<ParameterExpression *> arguments;

        CallExpression() :
            Expression(ExpressionType::Call) {
        }
    };

    struct VectorExpression : Expression {
        std::vector<ParameterExpression *> components;

        VectorExpression() :
            Expression(ExpressionType::Vector) {
        }
    };

    struct VectorIndexExpression : Expression {
        ParameterExpression *vector {nullptr};
        int index {0};

        VectorIndexExpression() :
            Expression(ExpressionType::VectorIndex) {
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

    static ExpressionTree fromProgram(const ScriptProgram &program, IRoutines &routines);

private:
    struct CallStackFrame {
        Function *function;
        int stackSizeOnEnter;

        CallStackFrame(Function *function, int stackSizeOnEnter) :
            function(function),
            stackSizeOnEnter(stackSizeOnEnter) {
        }
    };

    struct StackFrame {
        ParameterExpression *param;
        Function *allocatedBy;

        StackFrame(
            ParameterExpression *param,
            Function *allocatedBy) :
            param(param),
            allocatedBy(allocatedBy) {
        }

        StackFrame(const StackFrame &other) {
            param = other.param;
            allocatedBy = other.allocatedBy;
        }

        StackFrame withAllocatedBy(Function &allocatedBy) {
            auto copy = StackFrame(*this);
            copy.allocatedBy = &allocatedBy;
            return std::move(copy);
        }
    };

    struct DecompilationContext {
        const ScriptProgram &program;
        IRoutines &routines;
        const std::unordered_map<uint32_t, LabelExpression *> &labels;
        std::map<uint32_t, std::shared_ptr<Function>> &functions;
        std::vector<std::shared_ptr<Expression>> &expressions;

        std::vector<CallStackFrame> callStack;
        std::vector<StackFrame> stack;
        int numGlobals {0};
        int prevNumGlobals {0};
        BlockExpression *savedAction {nullptr};

        std::map<int, ParameterExpression *> *inputs {nullptr};
        std::map<int, ParameterExpression *> *outputs {nullptr};
        std::map<uint32_t, std::shared_ptr<DecompilationContext>> *branches {nullptr};

        DecompilationContext(
            const ScriptProgram &compiled,
            IRoutines &routines,
            const std::unordered_map<uint32_t, LabelExpression *> &labels,
            std::map<uint32_t, std::shared_ptr<Function>> &functions,
            std::vector<std::shared_ptr<Expression>> &expressions) :
            program(compiled),
            routines(routines),
            labels(labels),
            functions(functions),
            expressions(expressions) {
        }

        DecompilationContext(const DecompilationContext &other) :
            program(other.program),
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

        void pushCallStack(Function *function) {
            callStack.push_back(CallStackFrame(function, static_cast<int>(stack.size())));
        }

        void pushStack(ParameterExpression *param) {
            stack.push_back(StackFrame(param, topCall().function));
        }

        CallStackFrame &topCall() {
            return callStack.back();
        }

        VectorExpression *appendVectorCompose(
            uint32_t offset,
            BlockExpression &block,
            ParameterExpression &x,
            ParameterExpression &y,
            ParameterExpression &z);

        void appendVectorDecompose(
            uint32_t offset,
            BlockExpression &block,
            ParameterExpression &vec,
            ParameterExpression *&outX,
            ParameterExpression *&outY,
            ParameterExpression *&outZ);
    };

    std::vector<std::shared_ptr<Function>> _functions;
    std::vector<std::shared_ptr<Expression>> _expressions;
    std::set<const ParameterExpression *> _globals;

    static BlockExpression *decompile(uint32_t start, std::shared_ptr<DecompilationContext> ctx);
    static BlockExpression *decompileSafely(uint32_t start, std::shared_ptr<DecompilationContext> ctx);

    static std::unique_ptr<ConstantExpression> constantExpression(const Instruction &ins);
    static std::unique_ptr<ParameterExpression> parameterExpression(const Instruction &ins);
};

} // namespace script

} // namespace reone
