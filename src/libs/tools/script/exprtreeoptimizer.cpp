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
#include "reone/system/logutil.h"

namespace reone {

namespace script {

void ExpressionTreeOptimizer::optimize(ExpressionTree &tree) {
    auto ctx = std::make_unique<OptimizationContext>();
    analyze(tree, *ctx);
    compact(tree, *ctx);
}

void ExpressionTreeOptimizer::analyze(ExpressionTree &tree, OptimizationContext &ctx) {
    for (auto &func : tree.functions()) {
        analyzeFunction(*func, ctx);
    }
}

void ExpressionTreeOptimizer::analyzeFunction(Function &func, OptimizationContext &ctx) {
    auto exprToAnalyze = std::stack<std::tuple<Expression *, Expression *, int>>();
    exprToAnalyze.push(std::make_tuple(func.block, nullptr, -1));
    auto analyzedBlocks = std::set<BlockExpression *>();

    while (!exprToAnalyze.empty()) {
        auto [expr, container, binaryDir] = exprToAnalyze.top();
        exprToAnalyze.pop();

        if (expr->type == ExpressionType::Block) {
            auto blockExpr = static_cast<BlockExpression *>(expr);
            if (analyzedBlocks.count(blockExpr) == 0) {
                for (auto it = blockExpr->expressions.rbegin(); it != blockExpr->expressions.rend(); ++it) {
                    exprToAnalyze.push(std::make_tuple(*it, blockExpr, binaryDir));
                }
                analyzedBlocks.insert(blockExpr);
                ctx.blocksToCompact.push(blockExpr);
            }
        } else if (expr->type == ExpressionType::Parameter && container && container->type != ExpressionType::Block) {
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
            exprToAnalyze.push(std::make_tuple(conditionalExpr->test, conditionalExpr, binaryDir));
            if (conditionalExpr->ifTrue) {
                exprToAnalyze.push(std::make_tuple(conditionalExpr->ifTrue, conditionalExpr, binaryDir));
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
                    exprToAnalyze.push(std::make_tuple(blockArg, actionExpr, binaryDir));
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
                    if (callExpr->function->arguments[i].pointer) {
                        ctx.parameters[paramArg].writes.push_back(ParameterWriteEvent(callExpr, nullptr));
                    }
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
                exprToAnalyze.push(std::make_tuple(binaryExpr->left, binaryExpr, -1));
            }
            exprToAnalyze.push(std::make_tuple(binaryExpr->right, binaryExpr, 1));
        }
    }
}

void ExpressionTreeOptimizer::compact(ExpressionTree &tree, OptimizationContext &ctx) {
    tree.functions().erase(tree.functions().begin()); // __start
    if (!tree.globals().empty()) {
        auto &globalsFunc = tree.functions().front();
        for (auto it = tree.globals().begin(); it != tree.globals().end();) {
            auto &globalEvents = ctx.parameters[it->param];
            if (globalEvents.reads.empty()) {
                debug(str(boost::format("Unread global variable at %08x removed") % it->param->offset));
                it = tree.globals().erase(it);
                continue;
            }
            it->value = evaluate(*it->param, *globalsFunc, ctx);
            ++it;
        }
        tree.functions().erase(tree.functions().begin()); // __globals
    }

    for (auto &func : tree.functions()) {
        auto retValArg = func->arguments.end();
        for (auto argIter = func->arguments.begin(); argIter != func->arguments.end(); ++argIter) {
            auto &argEvents = ctx.parameters[argIter->param];
            if (argEvents.reads.empty() && !argEvents.writes.empty()) {
                retValArg = argIter;
            }
        }
        ParameterExpression *retVal = nullptr;
        int retValArgIdx = -1;
        if (retValArg != func->arguments.end()) {
            retVal = retValArg->param;
            retVal->locality = ParameterLocality::ReturnValue;
            retValArgIdx = std::distance(func->arguments.begin(), retValArg);
            func->returnType = retVal->variableType;
            func->retValStackOffset = retValArg->stackOffset;
            func->arguments.erase(retValArg);
        }
        if (func->returnType == VariableType::Void) {
            if (!func->block->expressions.empty() && func->block->expressions.back()->type == ExpressionType::Return) {
                debug(str(boost::format("Trailing return at %08x removed") % func->block->expressions.back()->offset));
                func->block->expressions.pop_back();
            }
        } else {
            func->block->expressions.insert(func->block->expressions.begin(), retVal);
            for (auto &expr : tree.expressions()) {
                if (expr->type == ExpressionType::Return) {
                    auto retExpr = static_cast<ReturnExpression *>(expr.get());
                    if (!func->contains(retExpr->offset)) {
                        continue;
                    }
                    retExpr->value = retVal;
                } else if (expr->type == ExpressionType::Call) {
                    auto callExpr = static_cast<CallExpression *>(expr.get());
                    if (callExpr->function != func.get()) {
                        continue;
                    }
                    Expression *destination = callExpr->arguments[retValArgIdx];
                    auto argToErase = callExpr->arguments.begin();
                    std::advance(argToErase, retValArgIdx);
                    callExpr->arguments.erase(argToErase);
                    if (destination && destination->type == ExpressionType::Parameter) {
                        ctx.callDestinations[callExpr] = static_cast<ParameterExpression *>(destination);
                        continue;
                    }
                }
            }
            debug(str(boost::format("Argument %d in function at %08x converted to return value") % retValArgIdx % func->start));
        }
    }

    auto &mainFunc = tree.functions().front();
    if (mainFunc->returnType != VariableType::Void) {
        mainFunc->name = "StartingConditional";
    } else {
        mainFunc->name = "main";
    }

    while (!ctx.blocksToCompact.empty()) {
        auto block = ctx.blocksToCompact.top();
        ctx.blocksToCompact.pop();

        for (auto it = block->expressions.begin(); it != block->expressions.end();) {
            auto expr = *it;
            if (expr->type == ExpressionType::Parameter) {
                auto paramExpr = static_cast<ParameterExpression *>(expr);
                if (paramExpr->locality == ParameterLocality::ReturnValue) {
                    ++it;
                    continue;
                }
                auto &paramEvents = ctx.parameters[paramExpr];
                if (!paramEvents.writes.empty()) {
                    auto &write = paramEvents.writes.front();
                    if (write.writeExpr->type == ExpressionType::Assign) {
                        debug(str(boost::format("Write-once variable declaration at %08x merged with initialization") % paramExpr->offset));
                        auto assignExpr = static_cast<BinaryExpression *>(write.writeExpr);
                        assignExpr->declareLeft = true;
                        it = block->expressions.erase(it);
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
                            blockArg->expressions.back()->type == ExpressionType::Return &&
                            (blockArg->expressions.front()->type == ExpressionType::Action || blockArg->expressions.front()->type == ExpressionType::Call)) {
                            int argIdx = std::distance(actionExpr->arguments.begin(), argIter);
                            debug(str(boost::format("Degenerate block argument %d in action call at %08x collapsed") % argIdx % blockArg->offset));
                            argIter = actionExpr->arguments.erase(argIter);
                            argIter = actionExpr->arguments.insert(argIter, blockArg->expressions.front());
                        }
                    }
                }
            } else if (expr->type == ExpressionType::Call) {
                auto callExpr = static_cast<CallExpression *>(expr);
                if (ctx.callDestinations.count(callExpr) > 0) {
                    auto destination = ctx.callDestinations.at(callExpr);
                    ctx.callDestinations.erase(callExpr);
                    debug(str(boost::format("Return value stored to variable at %08x in function call at %08x") % destination->offset % callExpr->offset));
                    auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignExpr->offset = callExpr->offset;
                    assignExpr->left = destination;
                    assignExpr->right = callExpr;
                    auto &destEvents = ctx.parameters[destination];
                    for (auto readIter = destEvents.reads.begin(); readIter != destEvents.reads.end();) {
                        auto &read = *readIter;
                        if (read.expression == callExpr) {
                            destEvents.reads.erase(readIter);
                            break;
                        }
                        ++readIter;
                    }
                    for (auto &write : destEvents.writes) {
                        if (write.writeExpr == callExpr && !write.value) {
                            write.writeExpr = assignExpr.get();
                            write.value = callExpr;
                            break;
                        }
                    }
                    it = block->expressions.erase(it);
                    block->expressions.insert(it, assignExpr.get());
                    it = block->expressions.begin();
                    tree.expressions().push_back(std::move(assignExpr));
                    continue;
                }
            } else if (ExpressionTree::isBinaryExpression(expr->type)) {
                auto binaryExpr = static_cast<BinaryExpression *>(expr);
                if (binaryExpr->type == ExpressionType::Assign && binaryExpr->declareLeft) {
                    auto leftParam = static_cast<ParameterExpression *>(binaryExpr->left);
                    if (leftParam->locality == ParameterLocality::ReturnValue) {
                        ++it;
                        continue;
                    }
                    auto &paramEvents = ctx.parameters[leftParam];
                    if (paramEvents.writes.size() == 1ll && paramEvents.reads.size() == 1ll) {
                        auto &read = paramEvents.reads.front();
                        auto &write = paramEvents.writes.front();
                        if (!write.value) {
                            ++it;
                            continue;
                        }
                        if (read.expression->type == ExpressionType::Action) {
                            debug(str(boost::format("Write-once / read-once variable at %08x inlined as argument %d in action call at %08x") % leftParam->offset % read.actionArgIdx % read.expression->offset));
                            auto readAction = static_cast<ActionExpression *>(read.expression);
                            readAction->arguments[read.actionArgIdx] = write.value;
                            it = block->expressions.erase(it);
                            continue;
                        } else if (read.expression->type == ExpressionType::Call) {
                            debug(str(boost::format("Write-once / read-once variable at %08x inlined as argument %d in function call at %08x") % leftParam->offset % read.callArgIdx % read.expression->offset));
                            auto readCall = static_cast<CallExpression *>(read.expression);
                            readCall->arguments[read.callArgIdx] = write.value;
                            it = block->expressions.erase(it);
                            continue;
                        } else if (ExpressionTree::isUnaryExpression(read.expression->type)) {
                            debug(str(boost::format("Write-once / read-once variable at %08x inlined as operand in unary expression at %08x") % leftParam->offset % read.expression->offset));
                            auto readUnary = static_cast<UnaryExpression *>(read.expression);
                            readUnary->operand = write.value;
                            it = block->expressions.erase(it);
                            continue;
                        } else if (ExpressionTree::isBinaryExpression(read.expression->type)) {
                            auto readBinary = static_cast<BinaryExpression *>(read.expression);
                            if (read.binaryDir == -1) {
                                debug(str(boost::format("Write-once / read-once variable at %08x inlined as left in binary expression at %08x") % leftParam->offset % read.expression->offset));
                                readBinary->left = write.value;
                            } else {
                                debug(str(boost::format("Write-once / read-once variable at %08x inlined as right in binary expression at %08x") % leftParam->offset % read.expression->offset));
                                readBinary->right = write.value;
                                if (readBinary->type == ExpressionType::Assign && readBinary->declareLeft) {
                                    auto destination = static_cast<ParameterExpression *>(readBinary->left);
                                    auto &destEvents = ctx.parameters[destination];
                                    destEvents.writes.front().value = write.value;
                                }
                            }
                            it = block->expressions.erase(it);
                            continue;
                        }
                    } else if (paramEvents.reads.empty()) {
                        debug(str(boost::format("Unread variable at %08x removed") % leftParam->offset));
                        it = block->expressions.erase(it);
                        if (binaryExpr->right->type == ExpressionType::Action ||
                            binaryExpr->right->type == ExpressionType::Call ||
                            ExpressionTree::isUnaryExpression(binaryExpr->right->type) ||
                            ExpressionTree::isBinaryExpression(binaryExpr->right->type)) {
                            it = ++block->expressions.insert(it, binaryExpr->right);
                        }
                        continue;
                    }
                }
            }
            ++it;
        }
    }
}

Variable ExpressionTreeOptimizer::evaluate(Expression &expr, Function &globalsFunc, OptimizationContext &ctx) {
    if (!globalsFunc.contains(expr.offset)) {
        return Variable::ofNull();
    }
    if (expr.type == ExpressionType::Constant) {
        return static_cast<ConstantExpression *>(&expr)->value;
    }
    if (expr.type == ExpressionType::Parameter) {
        auto paramExpr = static_cast<ParameterExpression *>(&expr);
        auto &paramEvents = ctx.parameters.at(paramExpr);
        if (!paramEvents.writes.empty()) {
            auto &write = paramEvents.writes.front();
            return evaluate(*write.value, globalsFunc, ctx);
        } else {
            return Variable::ofNull();
        }
    }
    if (expr.type == ExpressionType::Negate) {
        auto negateExpr = static_cast<UnaryExpression *>(&expr);
        return -evaluate(*negateExpr->operand, globalsFunc, ctx);
    }
    throw NotImplementedException();
}

} // namespace script

} // namespace reone
