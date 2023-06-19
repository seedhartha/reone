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

#include "exprtree.h"

namespace reone {

namespace script {

struct ParameterWriteEvent {
    Expression *writeExpr {nullptr};
    Expression *value {nullptr};

    ParameterWriteEvent() = default;

    ParameterWriteEvent(Expression *writeExpr, Expression *value) :
        writeExpr(writeExpr),
        value(value) {
    }
};

struct ParameterReadEvent {
    Expression *expression {nullptr};
    int actionArgIdx {0};
    int callArgIdx {0};
    int binaryDir {-1};

    ParameterReadEvent() = default;

    ParameterReadEvent(Expression *expression) :
        expression(expression) {
    }
};

struct ParameterEvents {
    std::list<ParameterWriteEvent> writes;
    std::list<ParameterReadEvent> reads;
};

struct LabelJumpEvent {
    Expression *expression {nullptr};

    LabelJumpEvent() = default;

    LabelJumpEvent(Expression *expression) :
        expression(expression) {
    }
};

struct LabelEvents {
    std::list<LabelJumpEvent> jumps;
};

struct OptimizationContext {
    std::map<ParameterExpression *, ParameterEvents> parameters;
    std::map<LabelExpression *, LabelEvents> labels;
    std::stack<BlockExpression *> blocksToCompact;
};

class IExpressionTreeOptimizer {
public:
    virtual ~IExpressionTreeOptimizer() = default;

    virtual void optimize(ExpressionTree &tree) = 0;
};

class ExpressionTreeOptimizer : public IExpressionTreeOptimizer, boost::noncopyable {
public:
    void optimize(ExpressionTree &tree) override;

private:
    void analyze(ExpressionTree &tree, OptimizationContext &ctx);
    void analyzeFunction(Function &func, OptimizationContext &ctx);

    void compact(OptimizationContext &ctx);
};

} // namespace script

} // namespace reone
