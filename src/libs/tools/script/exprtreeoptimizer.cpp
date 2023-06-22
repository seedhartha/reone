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

#include "reone/tools/script/exprtreeoptimizer.h"
#include "reone/system/exception/validation.h"

using namespace std;

namespace reone {

namespace script {

void ExpressionTreeOptimizer::optimize(ExpressionTree &tree) {
    auto ctx = make_unique<OptimizationContext>();
    analyze(tree, *ctx);
    compact(tree, *ctx);
}

void ExpressionTreeOptimizer::analyze(ExpressionTree &tree, OptimizationContext &ctx) {
    for (auto &func : tree.functions()) {
        analyzeFunction(*func, ctx);
    }
}

void ExpressionTreeOptimizer::analyzeFunction(Function &func, OptimizationContext &ctx) {
    auto exprToAnalyze = stack<tuple<Expression *, Expression *, int>>();
    exprToAnalyze.push(make_tuple(func.block, nullptr, -1));
    auto analyzedBlocks = set<BlockExpression *>();

    while (!exprToAnalyze.empty()) {
        auto [expr, container, binaryDir] = exprToAnalyze.top();
        exprToAnalyze.pop();

        if (expr->type == ExpressionType::Block) {
            auto blockExpr = static_cast<BlockExpression *>(expr);
            if (analyzedBlocks.count(blockExpr) == 0) {
                for (auto it = blockExpr->expressions.rbegin(); it != blockExpr->expressions.rend(); ++it) {
                    exprToAnalyze.push(make_tuple(*it, blockExpr, binaryDir));
                }
                analyzedBlocks.insert(blockExpr);
                ctx.blocksToCompact.push(blockExpr);
            }
        } else if (expr->type == ExpressionType::Parameter) {
            auto paramExpr = static_cast<ParameterExpression *>(expr);
            auto read = ParameterReadEvent(container);
            read.binaryDir = binaryDir;
            ctx.parameters[paramExpr].reads.push_back(std::move(read));
        } else if (expr->type == ExpressionType::Goto) {
            auto gotoExpr = static_cast<GotoExpression *>(expr);
            ctx.labels[gotoExpr->label].jumps.push_back(LabelJumpEvent(gotoExpr));
        } else if (expr->type == ExpressionType::Return) {
            auto returnExpr = static_cast<ReturnExpression *>(expr);
            if (returnExpr->value && returnExpr->value->type == ExpressionType::Parameter) {
                auto paramValue = static_cast<ParameterExpression *>(returnExpr->value);
                ctx.parameters[paramValue].reads.push_back(ParameterReadEvent(returnExpr));
            }
        } else if (expr->type == ExpressionType::Conditional) {
            auto conditionalExpr = static_cast<ConditionalExpression *>(expr);
            exprToAnalyze.push(make_tuple(conditionalExpr->test, conditionalExpr, binaryDir));
            if (conditionalExpr->ifTrue) {
                exprToAnalyze.push(make_tuple(conditionalExpr->ifTrue, conditionalExpr, binaryDir));
            }
        } else if (expr->type == ExpressionType::Action) {
            auto actionExpr = static_cast<ActionExpression *>(expr);
            for (size_t i = 0; i < actionExpr->arguments.size(); ++i) {
                auto arg = actionExpr->arguments[i];
                if (arg->type == ExpressionType::Parameter) {
                    auto paramArg = static_cast<ParameterExpression *>(arg);
                    auto read = ParameterReadEvent(actionExpr);
                    read.actionArgIdx = static_cast<int>(i);
                    ctx.parameters[paramArg].reads.push_back(std::move(read));
                } else if (arg->type == ExpressionType::Block) {
                    auto blockArg = static_cast<BlockExpression *>(arg);
                    exprToAnalyze.push(make_tuple(blockArg, actionExpr, binaryDir));
                }
            }
        } else if (expr->type == ExpressionType::Call) {
            auto callExpr = static_cast<CallExpression *>(expr);
            for (size_t i = 0; i < callExpr->arguments.size(); ++i) {
                auto arg = callExpr->arguments[i];
                if (arg->type == ExpressionType::Parameter) {
                    auto paramArg = static_cast<ParameterExpression *>(arg);
                    auto read = ParameterReadEvent(callExpr);
                    read.callArgIdx = static_cast<int>(i);
                    ctx.parameters[paramArg].reads.push_back(std::move(read));
                }
            }
        } else if (expr->type == ExpressionType::Vector) {
            auto vectorExpr = static_cast<VectorExpression *>(expr);
            for (auto &component : vectorExpr->components) {
                ctx.parameters[component].reads.push_back(ParameterReadEvent(vectorExpr));
            }
        } else if (expr->type == ExpressionType::VectorIndex) {
            auto vectorIdxExpr = static_cast<VectorIndexExpression *>(expr);
            ctx.parameters[vectorIdxExpr->vector].reads.push_back(ParameterReadEvent(vectorIdxExpr));
        } else if (ExpressionTree::isUnaryExpression(expr->type)) {
            auto unaryExpr = static_cast<UnaryExpression *>(expr);
            if (unaryExpr->operand->type != ExpressionType::Parameter) {
                throw ValidationException("Unary expression operand must be parameter");
            }
            auto paramOperand = static_cast<ParameterExpression *>(unaryExpr->operand);
            ctx.parameters[paramOperand].reads.push_back(ParameterReadEvent(unaryExpr));
            if (unaryExpr->type == ExpressionType::Increment || unaryExpr->type == ExpressionType::Decrement) {
                ctx.parameters[paramOperand].writes.push_back(ParameterWriteEvent(unaryExpr, paramOperand));
            }
        } else if (ExpressionTree::isBinaryExpression(expr->type)) {
            auto binaryExpr = static_cast<BinaryExpression *>(expr);
            if (binaryExpr->type == ExpressionType::Assign) {
                if (binaryExpr->left->type != ExpressionType::Parameter) {
                    throw ValidationException("Left of assign expression must be parameter");
                }
                auto leftParam = static_cast<ParameterExpression *>(binaryExpr->left);
                ctx.parameters[leftParam].writes.push_back(ParameterWriteEvent(binaryExpr, binaryExpr->right));
            } else {
                exprToAnalyze.push(make_tuple(binaryExpr->left, binaryExpr, -1));
            }
            exprToAnalyze.push(make_tuple(binaryExpr->right, binaryExpr, 1));
        }
    }
}

void ExpressionTreeOptimizer::compact(ExpressionTree &tree, OptimizationContext &ctx) {
    tree.functions().erase(tree.functions().begin()); // __start
    if (!tree.globals().empty()) {
        for (auto &global : tree.globals()) {
            global.value = evaluate(global.param, ctx);
        }
        tree.functions().erase(tree.functions().begin()); // __globals
    }

    while (!ctx.blocksToCompact.empty()) {
        auto block = ctx.blocksToCompact.top();
        ctx.blocksToCompact.pop();

        for (auto it = block->expressions.begin(); it != block->expressions.end();) {
            auto expr = *it;
            if (expr->type == ExpressionType::Parameter) {
                auto paramExpr = static_cast<ParameterExpression *>(expr);
                if (ctx.parameters[paramExpr].writes.size() == 1ll) {
                    auto &write = ctx.parameters[paramExpr].writes.front();
                    if (write.writeExpr->type == ExpressionType::Assign) {
                        it = block->expressions.erase(it);
                        static_cast<BinaryExpression *>(write.writeExpr)->declareLeft = true;
                        continue;
                    }
                }
            } else if (expr->type == ExpressionType::Action) {
                auto actionExpr = static_cast<ActionExpression *>(expr);
                for (auto argIter = actionExpr->arguments.begin(); argIter != actionExpr->arguments.end(); ++argIter) {
                    auto &arg = *argIter;
                    if (arg->type == ExpressionType::Block) {
                        auto blockArg = static_cast<BlockExpression *>(arg);
                        if (blockArg->expressions.size() == 2ll &&
                            blockArg->expressions.front()->type == ExpressionType::Action &&
                            blockArg->expressions.back()->type == ExpressionType::Return) {
                            argIter = actionExpr->arguments.erase(argIter);
                            argIter = actionExpr->arguments.insert(argIter, blockArg->expressions.front());
                        }
                    }
                }
            } else if (ExpressionTree::isBinaryExpression(expr->type)) {
                auto binaryExpr = static_cast<BinaryExpression *>(expr);
                if (binaryExpr->type == ExpressionType::Assign && binaryExpr->declareLeft) {
                    auto leftParam = static_cast<ParameterExpression *>(binaryExpr->left);
                    auto &paramEvents = ctx.parameters[leftParam];
                    if (paramEvents.writes.size() == 1ll && paramEvents.reads.size() == 1ll) {
                        auto &read = paramEvents.reads.front();
                        auto &write = paramEvents.writes.front();
                        if (read.expression->type == ExpressionType::Return && write.writeExpr->type == ExpressionType::Constant) {
                            it = block->expressions.erase(it);
                            auto readReturn = static_cast<ReturnExpression *>(read.expression);
                            readReturn->value = write.value;
                            continue;
                        } else if (read.expression->type == ExpressionType::Action) {
                            it = block->expressions.erase(it);
                            auto readAction = static_cast<ActionExpression *>(read.expression);
                            readAction->arguments[read.actionArgIdx] = write.value;
                            continue;
                        } else if (read.expression->type == ExpressionType::Call) {
                            it = block->expressions.erase(it);
                            auto readCall = static_cast<CallExpression *>(read.expression);
                            readCall->arguments[read.callArgIdx] = write.value;
                            continue;
                        } else if (ExpressionTree::isBinaryExpression(read.expression->type)) {
                            it = block->expressions.erase(it);
                            auto readBinary = static_cast<BinaryExpression *>(read.expression);
                            if (read.binaryDir == -1) {
                                readBinary->left = write.value;
                            } else {
                                readBinary->right = write.value;
                            }
                            continue;
                        }
                    } else if (paramEvents.reads.empty()) {
                        if (binaryExpr->right->type == ExpressionType::Parameter) {
                            it = block->expressions.erase(it);
                            continue;
                        } else if (binaryExpr->right->type == ExpressionType::Action) {
                            it = block->expressions.erase(it);
                            it = block->expressions.insert(it, binaryExpr->right);
                            continue;
                        }
                    }
                }
            }
            ++it;
        }
    }
}

Variable ExpressionTreeOptimizer::evaluate(Expression &expr, OptimizationContext &ctx) {
    if (expr.type == ExpressionType::Constant) {
        return static_cast<ConstantExpression *>(&expr)->value;
    }
    if (expr.type == ExpressionType::Parameter) {
        auto paramExpr = static_cast<ParameterExpression *>(&expr);
        auto &paramEvents = ctx.parameters.at(paramExpr);
        if (!paramEvents.writes.empty()) {
            auto &write = paramEvents.writes.front();
            return evaluate(*write.value, ctx);
        } else {
            return Variable::ofNull();
        }
    }
    if (expr.type == ExpressionType::Negate) {
        auto negateExpr = static_cast<UnaryExpression *>(&expr);
        return -evaluate(*negateExpr->operand, ctx);
    }
    throw NotImplementedException();
}

} // namespace script

} // namespace reone
