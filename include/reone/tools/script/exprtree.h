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

#pragma once

#include "reone/script/program.h"
#include "reone/script/variable.h"

namespace reone {

namespace script {

class IExpressionTreeOptimizer;
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

enum class ParameterLocality {
    Global,
    Local,
    Argument,
    ReturnValue,
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

    int outerStackOffset {0};
    bool outerRead {false};
    bool outerModified {false};

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
    ParameterExpression *param {nullptr};
    VariableType type;
    int stackOffset;
    bool pointer {false};

    FunctionArgument(ParameterExpression *param,
                     VariableType type,
                     int stackOffset,
                     bool pointer) :
        type(type),
        param(param),
        stackOffset(stackOffset),
        pointer(pointer) {
    }
};

struct Function {
    std::string name;
    uint32_t start {0};
    uint32_t end {0};
    std::vector<FunctionArgument> arguments;
    VariableType returnType {VariableType::Void};
    int retValStackOffset {0};
    BlockExpression *block {nullptr};

    bool contains(uint32_t offset) const {
        return start <= offset && offset <= end;
    }
};

struct CallExpression : Expression {
    Function *function {nullptr};
    std::vector<Expression *> arguments;

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

struct GlobalVariable {
    ParameterExpression *param {nullptr};
    Variable value;

    GlobalVariable() = default;

    GlobalVariable(ParameterExpression *param, Variable value) :
        param(param),
        value(value) {
    }
};

class ExpressionTree {
public:
    ExpressionTree(
        std::vector<GlobalVariable> globals,
        std::vector<std::shared_ptr<Function>> functions,
        std::vector<std::shared_ptr<Expression>> expressions) :
        _globals(std::move(globals)),
        _functions(std::move(functions)),
        _expressions(std::move(expressions)) {
    }

    std::vector<GlobalVariable> &globals() {
        return _globals;
    }

    std::vector<std::shared_ptr<Function>> &functions() {
        return _functions;
    }

    std::vector<std::shared_ptr<Expression>> &expressions() {
        return _expressions;
    }

    static ExpressionTree fromProgram(const ScriptProgram &program, IRoutines &routines, IExpressionTreeOptimizer &optimizer);

    static bool isUnaryExpression(ExpressionType type);
    static bool isBinaryExpression(ExpressionType type);

private:
    struct CallStackFragmentrame {
        Function *function;
        int stackSizeOnEnter;

        CallStackFragmentrame(Function *function, int stackSizeOnEnter) :
            function(function),
            stackSizeOnEnter(stackSizeOnEnter) {
        }
    };

    struct StackFragmentrame {
        ParameterExpression *param;
        Function *allocatedBy;

        StackFragmentrame(
            ParameterExpression *param,
            Function *allocatedBy) :
            param(param),
            allocatedBy(allocatedBy) {
        }
    };

    struct DecompilationContext {
        const ScriptProgram &program;
        IRoutines &routines;
        const std::unordered_map<uint32_t, LabelExpression *> &labels;
        std::map<uint32_t, std::shared_ptr<Function>> &functions;
        std::vector<std::shared_ptr<Expression>> &expressions;

        std::vector<CallStackFragmentrame> callStack;
        std::vector<StackFragmentrame> stack;
        int numGlobals {0};
        int prevNumGlobals {0};
        BlockExpression *savedAction {nullptr};

        std::map<int, ParameterExpression *, std::greater<int>> *outerParams {nullptr};

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
            outerParams(other.outerParams) {
        }

        void pushCallStack(Function *function) {
            callStack.push_back(CallStackFragmentrame(function, static_cast<int>(stack.size())));
        }

        void pushStack(ParameterExpression *param) {
            stack.push_back(StackFragmentrame(param, topCall().function));
        }

        CallStackFragmentrame &topCall() {
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

    std::vector<GlobalVariable> _globals;
    std::vector<std::shared_ptr<Function>> _functions;
    std::vector<std::shared_ptr<Expression>> _expressions;

    static void decompileFunction(Function &func, std::shared_ptr<DecompilationContext> ctx);

    static std::unique_ptr<ConstantExpression> constantExpression(const Instruction &ins);
    static std::unique_ptr<ParameterExpression> parameterExpression(const Instruction &ins);
};

} // namespace script

} // namespace reone
