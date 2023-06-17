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
        analyzeBlock(*func->block, ctx);
        for (auto &[offset, branch] : func->branches) {
            analyzeBlock(*branch.block, ctx);
        }
    }
}

void ExpressionTreeOptimizer::analyzeBlock(BlockExpression &block, OptimizationContext &ctx) {
    for (auto &expr : block.expressions) {
        analyze(*expr, ctx, block);
    }
}

void ExpressionTreeOptimizer::analyze(Expression &expr, OptimizationContext &ctx, Expression &container, int binaryDir) {
    if (expr.type == ExpressionType::Goto) {
        auto gotoExpr = static_cast<GotoExpression *>(&expr);
        ctx.labels[gotoExpr->label].jumps.push_back(LabelJumpEvent(gotoExpr));
    } else if (expr.type == ExpressionType::Conditional) {
        auto conditionalExpr = static_cast<ConditionalExpression *>(&expr);
        if (conditionalExpr->ifTrue) {
            analyzeBlock(*conditionalExpr->ifTrue, ctx);
        }
        if (conditionalExpr->ifFalse) {
            analyzeBlock(*conditionalExpr->ifFalse, ctx);
        }
    } else if (expr.type == ExpressionType::Parameter) {
        auto paramExpr = static_cast<ParameterExpression *>(&expr);
        auto read = ParameterReadEvent(&container);
        read.binaryDir = binaryDir;
        ctx.parameters[paramExpr].reads.push_back(std::move(read));
    } else if (expr.type == ExpressionType::Return) {
        auto returnExpr = static_cast<ReturnExpression *>(&expr);
        if (returnExpr->value && returnExpr->value->type == ExpressionType::Parameter) {
            auto paramValue = static_cast<ParameterExpression *>(returnExpr->value);
            ctx.parameters[paramValue].reads.push_back(ParameterReadEvent(returnExpr));
        }
    } else if (expr.type == ExpressionType::Conditional) {
        auto conditionalExpr = static_cast<ConditionalExpression *>(&expr);
        analyze(*conditionalExpr->test, ctx, *conditionalExpr);
        if (conditionalExpr->ifTrue) {
            analyzeBlock(*conditionalExpr->ifTrue, ctx);
        }
        if (conditionalExpr->ifFalse) {
            analyzeBlock(*conditionalExpr->ifFalse, ctx);
        }
    } else if (expr.type == ExpressionType::Action) {
        auto actionExpr = static_cast<ActionExpression *>(&expr);
        for (size_t i = 0; i < actionExpr->arguments.size(); ++i) {
            auto arg = actionExpr->arguments[i];
            if (arg->type == ExpressionType::Parameter) {
                auto paramArg = static_cast<ParameterExpression *>(arg);
                auto read = ParameterReadEvent(actionExpr);
                read.actionArgIdx = static_cast<int>(i);
                ctx.parameters[paramArg].reads.push_back(std::move(read));
            } else if (arg->type == ExpressionType::Block) {
                auto blockArg = static_cast<BlockExpression *>(arg);
                analyzeBlock(*blockArg, ctx);
            }
        }
    } else if (expr.type == ExpressionType::Call) {
        auto callExpr = static_cast<CallExpression *>(&expr);
        for (auto &arg : callExpr->arguments) {
            ctx.parameters[arg].reads.push_back(ParameterReadEvent(callExpr));
        }
    } else if (expr.type == ExpressionType::Vector) {
        auto vectorExpr = static_cast<VectorExpression *>(&expr);
        for (auto &component : vectorExpr->components) {
            ctx.parameters[component].reads.push_back(ParameterReadEvent(vectorExpr));
        }
    } else if (expr.type == ExpressionType::VectorIndex) {
        auto vectorIdxExpr = static_cast<VectorIndexExpression *>(&expr);
        ctx.parameters[vectorIdxExpr->vector].reads.push_back(ParameterReadEvent(vectorIdxExpr));
    } else if (ExpressionTree::isUnaryExpression(expr.type)) {
        auto unaryExpr = static_cast<UnaryExpression *>(&expr);
        if (unaryExpr->operand->type != ExpressionType::Parameter) {
            throw ValidationException("Unary expression operand must be parameter");
        }
        auto paramOperand = static_cast<ParameterExpression *>(unaryExpr->operand);
        ctx.parameters[paramOperand].reads.push_back(ParameterReadEvent(unaryExpr));
    } else if (ExpressionTree::isBinaryExpression(expr.type)) {
        auto binaryExpr = static_cast<BinaryExpression *>(&expr);
        if (binaryExpr->type == ExpressionType::Assign) {
            if (binaryExpr->left->type != ExpressionType::Parameter) {
                throw ValidationException("Left of assign expression must be parameter");
            }
            auto leftParam = static_cast<ParameterExpression *>(binaryExpr->left);
            ctx.parameters[leftParam].writes.push_back(ParameterWriteEvent(binaryExpr, binaryExpr->right));
        } else {
            analyze(*binaryExpr->left, ctx, *binaryExpr, -1);
        }
        analyze(*binaryExpr->right, ctx, *binaryExpr, 1);
    }
}

void ExpressionTreeOptimizer::compact(ExpressionTree &tree, OptimizationContext &ctx) {
    auto &functions = tree.functions();
    auto &startFunc = functions.front();
    if (startFunc->block->expressions.size() == 2ll &&
        startFunc->block->expressions.front()->type == ExpressionType::Call &&
        startFunc->block->expressions.back()->type == ExpressionType::Return) {
        functions.erase(functions.begin());
    }
    for (auto &func : functions) {
        auto branches = func->branches;
        for (auto &[offset, branch] : branches) {
            compactBlock(*func, *branch.block, ctx);
        }
        compactBlock(*func, *func->block, ctx);
    }
}

void ExpressionTreeOptimizer::compactBlock(Function &func, BlockExpression &block, OptimizationContext &ctx) {
    for (auto it = block.expressions.begin(); it != block.expressions.end(); ++it) {
        auto expr = *it;
        if (expr->type == ExpressionType::Parameter) {
            auto paramExpr = static_cast<ParameterExpression *>(expr);
            if (ctx.parameters[paramExpr].reads.empty()) {
                it = --block.expressions.erase(it);
            }
        } else if (expr->type == ExpressionType::Goto) {
            auto gotoExpr = static_cast<GotoExpression *>(expr);
            auto &labelEvents = ctx.labels[gotoExpr->label];
            auto &branch = func.branches.at(gotoExpr->label->offset);
            if (labelEvents.jumps.size() == 1ll) {
                auto branchOffset = gotoExpr->label->offset;
                auto maybeBranch = func.branches.find(branchOffset);
                if (maybeBranch != func.branches.end()) {
                    it = block.expressions.erase(it);
                    auto &branch = maybeBranch->second;
                    for (auto &branchExpr : branch.block->expressions) {
                        if (branchExpr == gotoExpr->label) {
                            continue;
                        }
                        it = ++block.expressions.insert(it, branchExpr);
                    }
                    func.branches.erase(branchOffset);
                    --it;
                }
            }
        } else if (expr->type == ExpressionType::Action) {
            auto actionExpr = static_cast<ActionExpression *>(expr);
            for (auto &arg : actionExpr->arguments) {
                if (arg->type == ExpressionType::Block) {
                    auto blockArg = static_cast<BlockExpression *>(arg);
                    compactBlock(func, *blockArg, ctx);
                }
            }
        } else if (ExpressionTree::isBinaryExpression(expr->type)) {
            auto binaryExpr = static_cast<BinaryExpression *>(expr);
            if (binaryExpr->type == ExpressionType::Assign && binaryExpr->declareLeft) {
                auto leftParam = static_cast<ParameterExpression *>(binaryExpr->left);
                auto &paramEvents = ctx.parameters[leftParam];
                if (paramEvents.reads.size() == 1ll && paramEvents.writes.size() == 1ll) {
                    auto &read = paramEvents.reads.front();
                    auto &write = paramEvents.writes.front();
                    if (read.expression->type == ExpressionType::Action) {
                        it = --block.expressions.erase(it);
                        auto readAction = static_cast<ActionExpression *>(read.expression);
                        readAction->arguments[read.actionArgIdx] = write.value;
                    } else if (ExpressionTree::isBinaryExpression(read.expression->type)) {
                        it = --block.expressions.erase(it);
                        auto readBinary = static_cast<BinaryExpression *>(read.expression);
                        if (read.binaryDir == -1) {
                            readBinary->left = write.value;
                        } else {
                            readBinary->right = write.value;
                        }
                    }
                } else if (paramEvents.reads.empty() && binaryExpr->right->type == ExpressionType::Action) {
                    it = block.expressions.erase(it);
                    it = block.expressions.insert(it, binaryExpr->right);
                }
            }
        }
    }
}

} // namespace script

} // namespace reone
