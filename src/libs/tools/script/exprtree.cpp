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

#include "reone/tools/script/exprtree.h"

#include "reone/script/instrutil.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"
#include "reone/script/variableutil.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"
#include "reone/tools/script/exprtreeoptimizer.h"

namespace reone {

namespace script {

ExpressionTree ExpressionTree::fromProgram(const ScriptProgram &program, IRoutines &routines, IExpressionTreeOptimizer &optimizer) {
    auto expressions = std::vector<std::shared_ptr<Expression>>();
    auto offsetToFunc = std::map<uint32_t, std::shared_ptr<Function>>();
    auto labels = std::unordered_map<uint32_t, LabelExpression *>();
    for (auto &ins : program.instructions()) {
        if ((ins.type == InstructionType::JMP ||
             ins.type == InstructionType::JZ ||
             ins.type == InstructionType::JNZ) &&
            ins.jumpOffset < 0) {
            auto offset = ins.offset + ins.jumpOffset;
            auto label = std::make_shared<LabelExpression>();
            label->offset = offset;
            labels[offset] = label.get();
            expressions.push_back(std::move(label));
        }
    }

    auto startFunc = std::make_shared<Function>();
    startFunc->name = "__start";
    startFunc->start = 13;

    auto ctx = std::make_shared<DecompilationContext>(program, routines, labels, offsetToFunc, expressions);
    ctx->functions[startFunc->start] = startFunc;
    ctx->pushCallStack(startFunc.get());

    decompileFunction(*startFunc, ctx);

    auto functions = std::vector<std::shared_ptr<Function>>();
    for (auto &[offset, func] : offsetToFunc) {
        functions.push_back(func);
    }
    auto globals = std::vector<GlobalVariable>();
    for (auto &expression : expressions) {
        if (expression->type != ExpressionType::Parameter) {
            continue;
        }
        auto paramExpr = static_cast<ParameterExpression *>(expression.get());
        if (paramExpr->locality == ParameterLocality::Global) {
            globals.push_back(GlobalVariable(paramExpr, Variable::ofNull()));
        }
    }
    int mainFuncIdx;
    if (!globals.empty()) {
        auto &globalsFunc = functions[1];
        globalsFunc->name = "__globals";
        for (auto it = globalsFunc->block->expressions.begin(); it != globalsFunc->block->expressions.end(); ++it) {
            auto expr = *it;
            if (expr->type != ExpressionType::Parameter) {
                continue;
            }
            auto paramExpr = static_cast<ParameterExpression *>(expr);
            if (paramExpr->locality == ParameterLocality::Global) {
                it = globalsFunc->block->expressions.erase(it);
                continue;
            }
        }
        mainFuncIdx = 2;
    } else {
        mainFuncIdx = 1;
    }
    auto &mainFunc = functions[mainFuncIdx];
    mainFunc->name = "main";

    auto tree = ExpressionTree(
        std::move(globals),
        std::move(functions),
        ctx->expressions);

    optimizer.optimize(tree);

    return tree;
}

void ExpressionTree::decompileFunction(Function &func, std::shared_ptr<DecompilationContext> ctx) {
    debug(boost::format("Decompiling function at %08x") % func.start);

    auto mainBlock = std::make_shared<BlockExpression>();
    mainBlock->offset = func.start;

    auto blocksToDecompile = std::stack<std::pair<BlockExpression *, std::shared_ptr<DecompilationContext>>>();
    blocksToDecompile.push(std::make_pair(mainBlock.get(), ctx));
    auto decompiledBlocks = std::map<std::pair<uint32_t, size_t>, BlockExpression *>();

    func.block = mainBlock.get();
    ctx->expressions.push_back(std::move(mainBlock));

    uint32_t maxOffset = func.start;

    while (!blocksToDecompile.empty()) {
        auto [block, ctx] = blocksToDecompile.top();
        blocksToDecompile.pop();
        decompiledBlocks[std::make_pair(block->offset, ctx->stack.size())] = block;

        try {
            debug(boost::format("Begin decompiling block at %08x") % block->offset);

            for (uint32_t offset = block->offset; offset < ctx->program.length();) {
                maxOffset = std::max(maxOffset, offset);

                auto maybeLabel = ctx->labels.find(offset);
                if (maybeLabel != ctx->labels.end()) {
                    block->append(maybeLabel->second);
                }

                // debug(boost::format("Stack: size=%d") % ctx->stack.size());
                // for (auto it = ctx->stack.rbegin(); it != ctx->stack.rend(); ++it) {
                //     auto type = describeVariableType(it->param->variableType);
                //     debug("    " + type);
                // }

                auto &ins = ctx->program.getInstruction(offset);
                debug(boost::format("Decompiling instruction at %08x of type %s") % offset % describeInstructionType(ins.type));

                if (ins.type == InstructionType::NOP ||
                    ins.type == InstructionType::NOP2) {

                } else if (ins.type == InstructionType::RETN) {
                    auto retExpr = std::make_shared<ReturnExpression>();
                    retExpr->offset = ins.offset;
                    block->append(retExpr.get());
                    ctx->expressions.push_back(std::move(retExpr));
                    break;

                } else if (ins.type == InstructionType::JMP) {
                    auto absJumpOffset = ins.offset + ins.jumpOffset;
                    if (ins.jumpOffset < 0) {
                        auto gotoExpr = std::make_shared<GotoExpression>();
                        gotoExpr->offset = ins.offset;
                        gotoExpr->label = ctx->labels.at(absJumpOffset);
                        block->append(gotoExpr.get());
                        ctx->expressions.push_back(std::move(gotoExpr));
                        break;
                    }
                    offset = absJumpOffset;
                    continue;

                } else if (ins.type == InstructionType::JSR) {
                    auto absJumpOffset = ins.offset + ins.jumpOffset;
                    std::shared_ptr<Function> sub;

                    if (ctx->functions.count(absJumpOffset) == 0) {
                        sub = std::make_shared<Function>();
                        sub->start = absJumpOffset;

                        ctx->functions[sub->start] = sub;

                        auto outerParams = std::map<int, ParameterExpression *, std::greater<int>>();

                        auto subCtx = std::make_shared<DecompilationContext>(*ctx);
                        subCtx->outerParams = &outerParams;
                        subCtx->pushCallStack(sub.get());

                        decompileFunction(*sub, subCtx);

                        for (auto &[stackOffset, param] : outerParams) {
                            sub->arguments.push_back(FunctionArgument(param, param->variableType, stackOffset, param->outerModified));
                        }
                    } else {
                        sub = ctx->functions.at(absJumpOffset);
                    }

                    auto callExpr = std::make_shared<CallExpression>();
                    callExpr->offset = ins.offset;
                    callExpr->function = sub.get();

                    for (auto &argument : sub->arguments) {
                        auto stackIdx = static_cast<int>(ctx->stack.size()) + argument.stackOffset / 4;
                        callExpr->arguments.push_back(ctx->stack[stackIdx].param);
                    }

                    block->append(callExpr.get());
                    ctx->expressions.push_back(std::move(callExpr));

                } else if (ins.type == InstructionType::JZ ||
                           ins.type == InstructionType::JNZ) {
                    auto ifTrueOffset = ins.offset + ins.jumpOffset;
                    auto ifFalseOffset = ins.nextOffset;

                    auto leftExpr = ctx->stack.back().param;
                    ctx->stack.pop_back();

                    auto rightExpr = std::make_shared<ConstantExpression>();
                    rightExpr->offset = ins.offset;
                    rightExpr->value = Variable::ofInt(0);

                    auto testExpr = std::make_shared<BinaryExpression>(ins.type == InstructionType::JZ ? ExpressionType::Equal : ExpressionType::NotEqual);
                    testExpr->offset = ins.offset;
                    testExpr->left = leftExpr;
                    testExpr->right = rightExpr.get();

                    BlockExpression *ifTrueBlockPtr;
                    if (ins.jumpOffset > 0) {
                        auto blockKey = std::make_pair(ifTrueOffset, ctx->stack.size());
                        if (decompiledBlocks.count(blockKey) > 0) {
                            ifTrueBlockPtr = decompiledBlocks.at(blockKey);
                        } else {
                            auto ifTrueCtx = std::make_shared<DecompilationContext>(*ctx);
                            auto ifTrueBlock = std::make_shared<BlockExpression>();
                            ifTrueBlock->offset = ifTrueOffset;

                            blocksToDecompile.push(std::make_pair(ifTrueBlock.get(), ifTrueCtx));
                            ifTrueBlockPtr = ifTrueBlock.get();

                            ctx->expressions.push_back(std::move(ifTrueBlock));
                        }

                    } else {
                        auto gotoExpr = std::make_shared<GotoExpression>();
                        gotoExpr->offset = ins.offset;
                        gotoExpr->label = ctx->labels.at(ifTrueOffset);

                        auto gotoBlockExpr = std::make_shared<BlockExpression>();
                        gotoBlockExpr->offset = ins.offset;
                        gotoBlockExpr->append(gotoExpr.get());
                        ifTrueBlockPtr = gotoBlockExpr.get();

                        ctx->expressions.push_back(std::move(gotoExpr));
                        ctx->expressions.push_back(std::move(gotoBlockExpr));
                    }

                    auto condExpr = std::make_shared<ConditionalExpression>();
                    condExpr->test = testExpr.get();
                    condExpr->ifTrue = ifTrueBlockPtr;
                    block->append(condExpr.get());

                    ctx->expressions.push_back(std::move(rightExpr));
                    ctx->expressions.push_back(std::move(testExpr));
                    ctx->expressions.push_back(std::move(condExpr));
                    offset = ins.nextOffset;
                    continue;

                } else if (ins.type == InstructionType::RSADDI ||
                           ins.type == InstructionType::RSADDF ||
                           ins.type == InstructionType::RSADDS ||
                           ins.type == InstructionType::RSADDO ||
                           ins.type == InstructionType::RSADDEFF ||
                           ins.type == InstructionType::RSADDEVT ||
                           ins.type == InstructionType::RSADDLOC ||
                           ins.type == InstructionType::RSADDTAL) {
                    auto expression = parameterExpression(ins);
                    block->append(expression.get());
                    ctx->pushStack(expression.get());
                    ctx->expressions.push_back(std::move(expression));

                } else if (ins.type == InstructionType::CONSTI ||
                           ins.type == InstructionType::CONSTF ||
                           ins.type == InstructionType::CONSTS ||
                           ins.type == InstructionType::CONSTO) {
                    auto constExpr = constantExpression(ins);

                    auto paramExpr = std::make_shared<ParameterExpression>();
                    paramExpr->offset = ins.offset;
                    paramExpr->variableType = constExpr->value.type;

                    auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignExpr->offset = ins.offset;
                    assignExpr->left = paramExpr.get();
                    assignExpr->right = constExpr.get();
                    assignExpr->declareLeft = true;
                    block->append(assignExpr.get());

                    ctx->pushStack(paramExpr.get());
                    ctx->expressions.push_back(std::move(assignExpr));
                    ctx->expressions.push_back(std::move(constExpr));
                    ctx->expressions.push_back(std::move(paramExpr));

                } else if (ins.type == InstructionType::ACTION) {
                    auto &routine = ctx->routines.get(ins.routine);

                    std::vector<Expression *> arguments;
                    for (int i = 0; i < ins.argCount; ++i) {
                        Expression *argument;
                        auto argType = routine.getArgumentType(i);
                        if (argType == VariableType::Vector) {
                            auto argZ = ctx->stack.back().param;
                            ctx->stack.pop_back();
                            auto argY = ctx->stack.back().param;
                            ctx->stack.pop_back();
                            auto argX = ctx->stack.back().param;
                            ctx->stack.pop_back();
                            argument = ctx->appendVectorCompose(ins.offset, *block, *argX, *argY, *argZ);
                        } else if (argType == VariableType::Action) {
                            argument = ctx->savedAction;
                        } else {
                            argument = ctx->stack.back().param;
                            ctx->stack.pop_back();
                        }
                        if (!argument) {
                            throw ValidationException("Unable to extract action argument from stack");
                        }
                        arguments.push_back(argument);
                    }

                    auto actionExpr = std::make_shared<ActionExpression>();
                    actionExpr->offset = ins.offset;
                    actionExpr->action = ins.routine;
                    actionExpr->arguments = std::move(arguments);

                    if (routine.returnType() != VariableType::Void) {
                        auto returnValue = std::make_shared<ParameterExpression>();
                        returnValue->offset = ins.offset;
                        returnValue->variableType = routine.returnType();

                        auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                        assignExpr->offset = ins.offset;
                        assignExpr->left = returnValue.get();
                        assignExpr->right = actionExpr.get();
                        assignExpr->declareLeft = true;
                        block->append(assignExpr.get());

                        if (routine.returnType() == VariableType::Vector) {
                            ParameterExpression *retValX = nullptr;
                            ParameterExpression *retValY = nullptr;
                            ParameterExpression *retValZ = nullptr;
                            ctx->appendVectorDecompose(ins.offset, *block, *returnValue, retValX, retValY, retValZ);
                            ctx->pushStack(retValX);
                            ctx->pushStack(retValY);
                            ctx->pushStack(retValZ);
                        } else {
                            ctx->pushStack(returnValue.get());
                        }
                        ctx->expressions.push_back(std::move(returnValue));
                        ctx->expressions.push_back(std::move(assignExpr));

                    } else {
                        block->append(actionExpr.get());
                    }

                    ctx->expressions.push_back(std::move(actionExpr));

                } else if (ins.type == InstructionType::CPDOWNSP ||
                           ins.type == InstructionType::CPDOWNBP) {
                    auto stackSize = static_cast<int>(ctx->stack.size());
                    if (ins.stackOffset >= 0) {
                        throw ValidationException("Non-negative stack offsets are not supported");
                    }
                    auto startIdx = (ins.type == InstructionType::CPDOWNSP ? stackSize : ctx->numGlobals) + (ins.stackOffset / 4);
                    auto numFrames = ins.size / 4;
                    for (int i = 0; i < numFrames; ++i) {
                        auto leftIdx = startIdx + numFrames - i - 1;
                        if (leftIdx < 0) {
                            continue;
                        }
                        auto &left = ctx->stack[leftIdx];
                        auto &right = ctx->stack[stackSize - i - 1];

                        ParameterExpression *destination;
                        if (left.allocatedBy != ctx->topCall().function && left.param->locality != ParameterLocality::Global) {
                            auto stackOffset = ins.stackOffset + 4 * (stackSize - ctx->topCall().stackSizeOnEnter + i);
                            if (ctx->outerParams->count(stackOffset) > 0) {
                                destination = ctx->outerParams->at(stackOffset);
                                destination->outerModified = true;
                            } else {
                                auto destExpr = std::make_shared<ParameterExpression>();
                                destExpr->offset = ins.offset;
                                destExpr->variableType = left.param->variableType;
                                destExpr->locality = ParameterLocality::Argument;
                                destExpr->outerStackOffset = stackOffset;
                                destExpr->outerModified = true;
                                destination = destExpr.get();
                                ctx->outerParams->insert(std::make_pair(stackOffset, destination));
                                ctx->expressions.push_back(std::move(destExpr));
                            }
                        } else {
                            destination = left.param;
                        }

                        auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                        assignExpr->offset = ins.offset;
                        assignExpr->left = destination;
                        assignExpr->right = right.param;
                        block->append(assignExpr.get());

                        destination->variableType = right.param->variableType;
                        ctx->expressions.push_back(std::move(assignExpr));
                    }
                } else if (ins.type == InstructionType::CPTOPSP ||
                           ins.type == InstructionType::CPTOPBP) {
                    auto stackSize = static_cast<int>(ctx->stack.size());
                    if (ins.stackOffset >= 0) {
                        throw ValidationException("Non-negative stack offsets are not supported");
                    }
                    auto startIdx = (ins.type == InstructionType::CPTOPSP ? stackSize : ctx->numGlobals) + (ins.stackOffset / 4);
                    if (startIdx < 0) {
                        throw ValidationException("Out of bounds stack access: " + std::to_string(startIdx));
                    }
                    auto numFrames = ins.size / 4;
                    for (int i = 0; i < numFrames; ++i) {
                        auto &frame = ctx->stack[startIdx + numFrames - i - 1];

                        ParameterExpression *source;
                        if (frame.allocatedBy != ctx->topCall().function && frame.param->locality != ParameterLocality::Global) {
                            auto stackOffset = ins.stackOffset + 4 * (stackSize - ctx->topCall().stackSizeOnEnter + i);
                            if (ctx->outerParams->count(stackOffset) > 0) {
                                source = ctx->outerParams->at(stackOffset);
                                source->outerRead = true;
                            } else {
                                auto sourceExpr = std::make_shared<ParameterExpression>();
                                sourceExpr->offset = ins.offset;
                                sourceExpr->variableType = frame.param->variableType;
                                sourceExpr->locality = ParameterLocality::Argument;
                                sourceExpr->outerStackOffset = stackOffset;
                                sourceExpr->outerRead = true;
                                source = sourceExpr.get();
                                ctx->outerParams->insert(std::make_pair(stackOffset, source));
                                ctx->expressions.push_back(std::move(sourceExpr));
                            }
                        } else {
                            source = frame.param;
                        }

                        auto paramExpr = std::make_shared<ParameterExpression>();
                        paramExpr->offset = ins.offset;
                        paramExpr->variableType = source->variableType;
                        if (numFrames > 1) {
                            paramExpr->suffix = std::to_string(i);
                        }

                        auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                        assignExpr->offset = ins.offset;
                        assignExpr->left = paramExpr.get();
                        assignExpr->right = source;
                        assignExpr->declareLeft = true;
                        block->append(assignExpr.get());

                        ctx->stack.push_back(StackFragmentrame(paramExpr.get(), ctx->topCall().function));
                        ctx->expressions.push_back(std::move(paramExpr));
                        ctx->expressions.push_back(std::move(assignExpr));
                    }
                } else if (ins.type == InstructionType::MOVSP) {
                    if (ins.stackOffset >= 0) {
                        throw ValidationException("Non-negative stack offsets are not supported");
                    }
                    for (int i = 0; i < -ins.stackOffset / 4; ++i) {
                        ctx->stack.pop_back();
                    }
                } else if (ins.type == InstructionType::NEGI ||
                           ins.type == InstructionType::NEGF ||
                           ins.type == InstructionType::COMPI ||
                           ins.type == InstructionType::NOTI) {
                    auto value = ctx->stack.back().param;
                    ctx->stack.pop_back();

                    ExpressionType type;
                    if (ins.type == InstructionType::NEGI ||
                        ins.type == InstructionType::NEGF) {
                        type = ExpressionType::Negate;
                    } else if (ins.type == InstructionType::COMPI) {
                        type = ExpressionType::OnesComplement;
                    } else if (ins.type == InstructionType::NOTI) {
                        type = ExpressionType::Not;
                    }
                    auto unaryExpr = std::make_shared<UnaryExpression>(type);
                    unaryExpr->offset = ins.offset;
                    unaryExpr->operand = value;

                    auto resultExpr = std::make_shared<ParameterExpression>();
                    resultExpr->offset = ins.offset;
                    resultExpr->variableType = value->variableType;

                    auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignExpr->offset = ins.offset;
                    assignExpr->left = resultExpr.get();
                    assignExpr->right = unaryExpr.get();
                    assignExpr->declareLeft = true;
                    block->append(assignExpr.get());

                    ctx->pushStack(resultExpr.get());
                    ctx->expressions.push_back(std::move(resultExpr));
                    ctx->expressions.push_back(std::move(unaryExpr));
                    ctx->expressions.push_back(std::move(assignExpr));

                } else if (ins.type == InstructionType::ADDII ||
                           ins.type == InstructionType::ADDIF ||
                           ins.type == InstructionType::ADDFI ||
                           ins.type == InstructionType::ADDFF ||
                           ins.type == InstructionType::ADDSS ||
                           ins.type == InstructionType::SUBII ||
                           ins.type == InstructionType::SUBIF ||
                           ins.type == InstructionType::SUBFI ||
                           ins.type == InstructionType::SUBFF ||
                           ins.type == InstructionType::MULII ||
                           ins.type == InstructionType::MULIF ||
                           ins.type == InstructionType::MULFI ||
                           ins.type == InstructionType::MULFF ||
                           ins.type == InstructionType::DIVII ||
                           ins.type == InstructionType::DIVIF ||
                           ins.type == InstructionType::DIVFI ||
                           ins.type == InstructionType::DIVFF ||
                           ins.type == InstructionType::MODII ||
                           ins.type == InstructionType::LOGANDII ||
                           ins.type == InstructionType::LOGORII ||
                           ins.type == InstructionType::INCORII ||
                           ins.type == InstructionType::EXCORII ||
                           ins.type == InstructionType::BOOLANDII ||
                           ins.type == InstructionType::EQUALII ||
                           ins.type == InstructionType::EQUALFF ||
                           ins.type == InstructionType::EQUALSS ||
                           ins.type == InstructionType::EQUALOO ||
                           ins.type == InstructionType::EQUALEFFEFF ||
                           ins.type == InstructionType::EQUALEVTEVT ||
                           ins.type == InstructionType::EQUALLOCLOC ||
                           ins.type == InstructionType::EQUALTALTAL ||
                           ins.type == InstructionType::NEQUALII ||
                           ins.type == InstructionType::NEQUALFF ||
                           ins.type == InstructionType::NEQUALSS ||
                           ins.type == InstructionType::NEQUALOO ||
                           ins.type == InstructionType::NEQUALEFFEFF ||
                           ins.type == InstructionType::NEQUALEVTEVT ||
                           ins.type == InstructionType::NEQUALLOCLOC ||
                           ins.type == InstructionType::NEQUALTALTAL ||
                           ins.type == InstructionType::GEQII ||
                           ins.type == InstructionType::GEQFF ||
                           ins.type == InstructionType::GTII ||
                           ins.type == InstructionType::GTFF ||
                           ins.type == InstructionType::LTII ||
                           ins.type == InstructionType::LTFF ||
                           ins.type == InstructionType::LEQII ||
                           ins.type == InstructionType::LEQFF ||
                           ins.type == InstructionType::SHLEFTII ||
                           ins.type == InstructionType::SHRIGHTII ||
                           ins.type == InstructionType::USHRIGHTII) {
                    auto right = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto left = ctx->stack.back().param;
                    ctx->stack.pop_back();

                    ExpressionType type;
                    if (ins.type == InstructionType::ADDII ||
                        ins.type == InstructionType::ADDIF ||
                        ins.type == InstructionType::ADDFI ||
                        ins.type == InstructionType::ADDFF ||
                        ins.type == InstructionType::ADDSS) {
                        type = ExpressionType::Add;
                    } else if (ins.type == InstructionType::SUBII ||
                               ins.type == InstructionType::SUBIF ||
                               ins.type == InstructionType::SUBFI ||
                               ins.type == InstructionType::SUBFF) {
                        type = ExpressionType::Subtract;
                    } else if (ins.type == InstructionType::MULII ||
                               ins.type == InstructionType::MULIF ||
                               ins.type == InstructionType::MULFI ||
                               ins.type == InstructionType::MULFF) {
                        type = ExpressionType::Multiply;
                    } else if (ins.type == InstructionType::DIVII ||
                               ins.type == InstructionType::DIVIF ||
                               ins.type == InstructionType::DIVFI ||
                               ins.type == InstructionType::DIVFF) {
                        type = ExpressionType::Divide;
                    } else if (ins.type == InstructionType::MODII) {
                        type = ExpressionType::Modulo;
                    } else if (ins.type == InstructionType::LOGANDII) {
                        type = ExpressionType::LogicalAnd;
                    } else if (ins.type == InstructionType::LOGORII) {
                        type = ExpressionType::LogicalOr;
                    } else if (ins.type == InstructionType::INCORII) {
                        type = ExpressionType::BitwiseOr;
                    } else if (ins.type == InstructionType::EXCORII) {
                        type = ExpressionType::BitwiseExlusiveOr;
                    } else if (ins.type == InstructionType::BOOLANDII) {
                        type = ExpressionType::BitwiseAnd;
                    } else if (ins.type == InstructionType::EQUALII ||
                               ins.type == InstructionType::EQUALFF ||
                               ins.type == InstructionType::EQUALSS ||
                               ins.type == InstructionType::EQUALOO ||
                               ins.type == InstructionType::EQUALEFFEFF ||
                               ins.type == InstructionType::EQUALEVTEVT ||
                               ins.type == InstructionType::EQUALLOCLOC ||
                               ins.type == InstructionType::EQUALTALTAL) {
                        type = ExpressionType::Equal;
                    } else if (ins.type == InstructionType::NEQUALII ||
                               ins.type == InstructionType::NEQUALFF ||
                               ins.type == InstructionType::NEQUALSS ||
                               ins.type == InstructionType::NEQUALOO ||
                               ins.type == InstructionType::NEQUALEFFEFF ||
                               ins.type == InstructionType::NEQUALEVTEVT ||
                               ins.type == InstructionType::NEQUALLOCLOC ||
                               ins.type == InstructionType::NEQUALTALTAL) {
                        type = ExpressionType::NotEqual;
                    } else if (ins.type == InstructionType::GEQII ||
                               ins.type == InstructionType::GEQFF) {
                        type = ExpressionType::GreaterThanOrEqual;
                    } else if (ins.type == InstructionType::GTII ||
                               ins.type == InstructionType::GTFF) {
                        type = ExpressionType::GreaterThan;
                    } else if (ins.type == InstructionType::LTII ||
                               ins.type == InstructionType::LTFF) {
                        type = ExpressionType::LessThan;
                    } else if (ins.type == InstructionType::LEQII ||
                               ins.type == InstructionType::LEQFF) {
                        type = ExpressionType::LessThanOrEqual;
                    } else if (ins.type == InstructionType::SHLEFTII) {
                        type = ExpressionType::LeftShift;
                    } else if (ins.type == InstructionType::SHRIGHTII) {
                        type = ExpressionType::RightShift;
                    } else if (ins.type == InstructionType::USHRIGHTII) {
                        type = ExpressionType::RightShiftUnsigned;
                    }
                    auto binaryExpr = std::make_shared<BinaryExpression>(type);
                    binaryExpr->offset = ins.offset;
                    binaryExpr->left = left;
                    binaryExpr->right = right;

                    VariableType varType;
                    if (ins.type == InstructionType::ADDIF ||
                        ins.type == InstructionType::ADDFI ||
                        ins.type == InstructionType::ADDFF ||
                        ins.type == InstructionType::SUBIF ||
                        ins.type == InstructionType::SUBFI ||
                        ins.type == InstructionType::SUBFF ||
                        ins.type == InstructionType::MULIF ||
                        ins.type == InstructionType::MULFI ||
                        ins.type == InstructionType::MULFF ||
                        ins.type == InstructionType::DIVIF ||
                        ins.type == InstructionType::DIVFI ||
                        ins.type == InstructionType::DIVFF) {
                        varType = VariableType::Float;
                    } else {
                        varType = VariableType::Int;
                    }
                    auto result = std::make_shared<ParameterExpression>();
                    result->offset = ins.offset;
                    result->variableType = varType;

                    auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignExpr->offset = ins.offset;
                    assignExpr->left = result.get();
                    assignExpr->right = binaryExpr.get();
                    assignExpr->declareLeft = true;
                    block->append(assignExpr.get());

                    ctx->pushStack(result.get());
                    ctx->expressions.push_back(std::move(result));
                    ctx->expressions.push_back(std::move(binaryExpr));
                    ctx->expressions.push_back(std::move(assignExpr));

                } else if (ins.type == InstructionType::ADDVV ||
                           ins.type == InstructionType::SUBVV) {
                    auto rightZ = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto rightY = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto rightX = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto right = ctx->appendVectorCompose(ins.offset, *block, *rightX, *rightY, *rightZ);

                    auto leftZ = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto leftY = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto leftX = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto left = ctx->appendVectorCompose(ins.offset, *block, *leftX, *leftY, *leftZ);

                    auto type = (ins.type == InstructionType::ADDVV) ? ExpressionType::Add : ExpressionType::Subtract;
                    auto binaryExpr = std::make_shared<BinaryExpression>(type);
                    binaryExpr->offset = ins.offset;
                    binaryExpr->left = left;
                    binaryExpr->right = right;

                    auto result = std::make_shared<ParameterExpression>();
                    result->offset = ins.offset;
                    result->variableType = VariableType::Vector;

                    auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignExpr->offset = ins.offset;
                    assignExpr->left = result.get();
                    assignExpr->right = binaryExpr.get();
                    assignExpr->declareLeft = true;
                    block->append(assignExpr.get());

                    ParameterExpression *resultX = nullptr;
                    ParameterExpression *resultY = nullptr;
                    ParameterExpression *resultZ = nullptr;
                    ctx->appendVectorDecompose(ins.offset, *block, *result, resultX, resultY, resultZ);
                    ctx->pushStack(resultX);
                    ctx->pushStack(resultY);
                    ctx->pushStack(resultZ);

                    ctx->expressions.push_back(std::move(result));
                    ctx->expressions.push_back(std::move(binaryExpr));
                    ctx->expressions.push_back(std::move(assignExpr));

                } else if (ins.type == InstructionType::DIVFV ||
                           ins.type == InstructionType::MULFV) {
                    auto rightZ = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto rightY = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto rightX = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto right = ctx->appendVectorCompose(ins.offset, *block, *rightX, *rightY, *rightZ);

                    auto left = ctx->stack.back().param;
                    ctx->stack.pop_back();

                    auto type = (ins.type == InstructionType::DIVFV) ? ExpressionType::Divide : ExpressionType::Multiply;
                    auto binaryExpr = std::make_shared<BinaryExpression>(type);
                    binaryExpr->offset = ins.offset;
                    binaryExpr->left = left;
                    binaryExpr->right = right;

                    auto result = std::make_shared<ParameterExpression>();
                    result->offset = ins.offset;
                    result->variableType = VariableType::Vector;

                    auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignExpr->offset = ins.offset;
                    assignExpr->left = result.get();
                    assignExpr->right = binaryExpr.get();
                    assignExpr->declareLeft = true;
                    block->append(assignExpr.get());

                    ParameterExpression *resultX = nullptr;
                    ParameterExpression *resultY = nullptr;
                    ParameterExpression *resultZ = nullptr;
                    ctx->appendVectorDecompose(ins.offset, *block, *result, resultX, resultY, resultZ);
                    ctx->pushStack(resultX);
                    ctx->pushStack(resultY);
                    ctx->pushStack(resultZ);

                    ctx->expressions.push_back(std::move(result));
                    ctx->expressions.push_back(std::move(binaryExpr));
                    ctx->expressions.push_back(std::move(assignExpr));

                } else if (ins.type == InstructionType::DIVVF ||
                           ins.type == InstructionType::MULVF) {
                    auto right = ctx->stack.back().param;
                    ctx->stack.pop_back();

                    auto leftZ = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto leftY = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto leftX = ctx->stack.back().param;
                    ctx->stack.pop_back();
                    auto left = ctx->appendVectorCompose(ins.offset, *block, *leftX, *leftY, *leftZ);

                    auto type = (ins.type == InstructionType::DIVVF) ? ExpressionType::Divide : ExpressionType::Multiply;
                    auto binaryExpr = std::make_shared<BinaryExpression>(type);
                    binaryExpr->offset = ins.offset;
                    binaryExpr->left = left;
                    binaryExpr->right = right;

                    auto result = std::make_shared<ParameterExpression>();
                    result->offset = ins.offset;
                    result->variableType = VariableType::Vector;

                    auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignExpr->offset = ins.offset;
                    assignExpr->left = result.get();
                    assignExpr->right = binaryExpr.get();
                    assignExpr->declareLeft = true;
                    block->append(assignExpr.get());

                    ParameterExpression *resultX = nullptr;
                    ParameterExpression *resultY = nullptr;
                    ParameterExpression *resultZ = nullptr;
                    ctx->appendVectorDecompose(ins.offset, *block, *result, resultX, resultY, resultZ);
                    ctx->pushStack(resultX);
                    ctx->pushStack(resultY);
                    ctx->pushStack(resultZ);

                    ctx->expressions.push_back(std::move(binaryExpr));
                    ctx->expressions.push_back(std::move(result));
                    ctx->expressions.push_back(std::move(assignExpr));

                } else if (ins.type == InstructionType::EQUALTT ||
                           ins.type == InstructionType::NEQUALTT) {
                    auto numFrames = ins.size / 4;
                    std::vector<StackFragmentrame> rightFrames;
                    for (int i = 0; i < numFrames; ++i) {
                        rightFrames.push_back(ctx->stack.back());
                        ctx->stack.pop_back();
                    }
                    std::vector<StackFragmentrame> leftFrames;
                    for (int i = 0; i < numFrames; ++i) {
                        leftFrames.push_back(ctx->stack.back());
                        ctx->stack.pop_back();
                    }

                    auto resultExpr = std::make_shared<ParameterExpression>();
                    resultExpr->offset = ins.offset;
                    resultExpr->variableType = VariableType::Int;

                    for (int i = 0; i < numFrames; ++i) {
                        auto firstType = (ins.type == InstructionType::EQUALTT) ? ExpressionType::Equal : ExpressionType::NotEqual;
                        auto compExpr = std::make_shared<BinaryExpression>(firstType);
                        compExpr->offset = ins.offset;
                        compExpr->left = leftFrames[i].param;
                        compExpr->right = rightFrames[i].param;

                        auto secondType = (ins.type == InstructionType::EQUALTT) ? ExpressionType::LogicalAnd : ExpressionType::LogicalOr;
                        auto andOrExpression = std::make_shared<BinaryExpression>(secondType);
                        andOrExpression->offset = ins.offset;
                        andOrExpression->left = resultExpr.get();
                        andOrExpression->right = compExpr.get();

                        auto assignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
                        assignExpr->offset = ins.offset;
                        assignExpr->left = resultExpr.get();
                        assignExpr->right = andOrExpression.get();
                        assignExpr->declareLeft = true;
                        block->append(assignExpr.get());

                        ctx->expressions.push_back(std::move(compExpr));
                        ctx->expressions.push_back(std::move(andOrExpression));
                        ctx->expressions.push_back(std::move(assignExpr));
                    }

                    ctx->pushStack(resultExpr.get());
                    ctx->expressions.push_back(std::move(resultExpr));

                } else if (ins.type == InstructionType::STORE_STATE) {
                    auto absJumpOffset = ins.offset + 0x10;
                    auto blockKey = std::make_pair(absJumpOffset, ctx->stack.size());
                    if (decompiledBlocks.count(blockKey) > 0) {
                        ctx->savedAction = decompiledBlocks.at(blockKey);
                    } else {
                        auto actionBlock = std::make_shared<BlockExpression>();
                        actionBlock->offset = absJumpOffset;
                        auto actionCtx = std::make_shared<DecompilationContext>(*ctx);

                        blocksToDecompile.push(std::make_pair(actionBlock.get(), actionCtx));
                        ctx->savedAction = actionBlock.get();

                        ctx->expressions.push_back(std::move(actionBlock));
                    }
                } else if (ins.type == InstructionType::SAVEBP) {
                    ctx->prevNumGlobals = ctx->numGlobals;
                    ctx->numGlobals = static_cast<int>(ctx->stack.size());
                    for (int i = 0; i < ctx->numGlobals; ++i) {
                        ctx->stack[i].param->locality = ParameterLocality::Global;
                    }

                } else if (ins.type == InstructionType::RESTOREBP) {
                    // ctx.numGlobals = ctx.prevNumGlobals;

                } else if (ins.type == InstructionType::DECISP ||
                           ins.type == InstructionType::DECIBP ||
                           ins.type == InstructionType::INCISP ||
                           ins.type == InstructionType::INCIBP) {
                    if (ins.stackOffset >= 0) {
                        throw ValidationException("Non-negative stack offsets are not supported");
                    }
                    auto stackSize = static_cast<int>(ctx->stack.size());
                    auto frameIdx = ((ins.type == InstructionType::DECISP || ins.type == InstructionType::INCISP) ? stackSize : ctx->numGlobals) + (ins.stackOffset / 4);
                    auto &frame = ctx->stack[frameIdx];

                    ParameterExpression *destination;
                    if (frame.allocatedBy != ctx->topCall().function && frame.param->locality != ParameterLocality::Global) {
                        auto stackOffset = ins.stackOffset + 4 * (stackSize - ctx->topCall().stackSizeOnEnter);
                        if (ctx->outerParams->count(stackOffset) > 0) {
                            destination = ctx->outerParams->at(stackOffset);
                            destination->outerRead = true;
                            destination->outerModified = true;
                        } else {
                            auto destExpr = std::make_shared<ParameterExpression>();
                            destExpr->offset = ins.offset;
                            destExpr->variableType = frame.param->variableType;
                            destExpr->locality = ParameterLocality::Argument;
                            destExpr->outerStackOffset = stackOffset;
                            destExpr->outerRead = true;
                            destExpr->outerModified = true;
                            destination = destExpr.get();
                            ctx->outerParams->insert(std::make_pair(stackOffset, destination));
                            ctx->expressions.push_back(std::move(destExpr));
                        }
                    } else {
                        destination = frame.param;
                    }

                    ExpressionType type;
                    if (ins.type == InstructionType::DECISP ||
                        ins.type == InstructionType::DECIBP) {
                        type = ExpressionType::Decrement;
                    } else if (ins.type == InstructionType::INCISP ||
                               ins.type == InstructionType::INCIBP) {
                        type = ExpressionType::Increment;
                    }

                    auto unaryExpr = std::make_shared<UnaryExpression>(type);
                    unaryExpr->offset = ins.offset;
                    unaryExpr->operand = destination;
                    block->append(unaryExpr.get());

                    ctx->expressions.push_back(std::move(unaryExpr));

                } else if (ins.type == InstructionType::DESTRUCT) {
                    auto numFrames = ins.size / 4;
                    auto startNoDestroy = static_cast<int>(ctx->stack.size()) - numFrames + (ins.stackOffset / 4);
                    auto numFramesNoDestroy = ins.sizeNoDestroy / 4;

                    std::vector<StackFragmentrame> framesNoDestroy;
                    for (int i = 0; i < numFramesNoDestroy; ++i) {
                        auto &frame = ctx->stack[startNoDestroy + i];
                        framesNoDestroy.push_back(frame);
                    }
                    for (int i = 0; i < numFrames - numFramesNoDestroy; ++i) {
                        ctx->stack.pop_back();
                    }
                    for (auto &frame : framesNoDestroy) {
                        ctx->stack.push_back(frame);
                    }
                } else {
                    throw NotImplementedException("Cannot decompile expression of type " + std::to_string(static_cast<int>(ins.type)));
                }

                offset = ins.nextOffset;
            }

            debug(boost::format("End decompiling block at %08x") % block->offset);

        } catch (const std::logic_error &e) {
            error(boost::format("Error decompiling block at %08x: %s") % block->offset % std::string(e.what()));
        }
    }

    func.end = maxOffset;

    debug(boost::format("End decompiling function at %08x") % func.start);
}

std::unique_ptr<ConstantExpression> ExpressionTree::constantExpression(const Instruction &ins) {
    switch (ins.type) {
    case InstructionType::CONSTI:
    case InstructionType::CONSTF:
    case InstructionType::CONSTS:
    case InstructionType::CONSTO: {
        auto constExpr = std::make_unique<ConstantExpression>();
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
        return constExpr;
    }
    default:
        throw std::invalid_argument("Instruction is not of CONSTx type: " + std::to_string(static_cast<int>(ins.type)));
    }
}

std::unique_ptr<ParameterExpression> ExpressionTree::parameterExpression(const Instruction &ins) {
    switch (ins.type) {
    case InstructionType::RSADDI:
    case InstructionType::RSADDF:
    case InstructionType::RSADDS:
    case InstructionType::RSADDO:
    case InstructionType::RSADDEFF:
    case InstructionType::RSADDEVT:
    case InstructionType::RSADDLOC:
    case InstructionType::RSADDTAL: {
        auto paramExpr = std::make_unique<ParameterExpression>();
        paramExpr->offset = ins.offset;
        if (ins.type == InstructionType::RSADDI) {
            paramExpr->variableType = VariableType::Int;
        } else if (ins.type == InstructionType::RSADDF) {
            paramExpr->variableType = VariableType::Float;
        } else if (ins.type == InstructionType::RSADDS) {
            paramExpr->variableType = VariableType::String;
        } else if (ins.type == InstructionType::RSADDO) {
            paramExpr->variableType = VariableType::Object;
        } else if (ins.type == InstructionType::RSADDEFF) {
            paramExpr->variableType = VariableType::Effect;
        } else if (ins.type == InstructionType::RSADDEVT) {
            paramExpr->variableType = VariableType::Event;
        } else if (ins.type == InstructionType::RSADDLOC) {
            paramExpr->variableType = VariableType::Location;
        } else if (ins.type == InstructionType::RSADDTAL) {
            paramExpr->variableType = VariableType::Talent;
        }
        return paramExpr;
    }
    default:
        throw std::invalid_argument("Instruction is not of RSADDx type: " + std::to_string(static_cast<int>(ins.type)));
    }
}

VectorExpression *ExpressionTree::DecompilationContext::appendVectorCompose(
    uint32_t offset,
    BlockExpression &block,
    ParameterExpression &x,
    ParameterExpression &y,
    ParameterExpression &z) {

    if ((x.variableType != VariableType::Float) ||
        (y.variableType != VariableType::Float) ||
        (z.variableType != VariableType::Float)) {
        throw std::invalid_argument("Cannot compose a vector of non-floats");
    }

    auto vecExpr = std::make_shared<VectorExpression>();
    vecExpr->offset = offset;
    vecExpr->components.push_back(&x);
    vecExpr->components.push_back(&y);
    vecExpr->components.push_back(&z);

    expressions.push_back(vecExpr);

    return vecExpr.get();
}

void ExpressionTree::DecompilationContext::appendVectorDecompose(
    uint32_t offset,
    BlockExpression &block,
    ParameterExpression &vec,
    ParameterExpression *&outX,
    ParameterExpression *&outY,
    ParameterExpression *&outZ) {

    // X

    auto xIndexExpr = std::make_shared<VectorIndexExpression>();
    xIndexExpr->offset = offset;
    xIndexExpr->vector = &vec;
    xIndexExpr->index = 0;

    auto xParamExpr = std::make_shared<ParameterExpression>();
    xParamExpr->offset = offset;
    xParamExpr->variableType = VariableType::Float;
    xParamExpr->suffix = "x";

    auto xAssignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
    xAssignExpr->offset = offset;
    xAssignExpr->left = xParamExpr.get();
    xAssignExpr->right = xIndexExpr.get();
    xAssignExpr->declareLeft = true;
    block.append(xAssignExpr.get());

    outX = xParamExpr.get();

    expressions.push_back(std::move(xParamExpr));
    expressions.push_back(std::move(xIndexExpr));
    expressions.push_back(std::move(xAssignExpr));

    // Y

    auto yIndexExpr = std::make_shared<VectorIndexExpression>();
    yIndexExpr->offset = offset;
    yIndexExpr->vector = &vec;
    yIndexExpr->index = 1;

    auto yParamExpr = std::make_shared<ParameterExpression>();
    yParamExpr->offset = offset;
    yParamExpr->variableType = VariableType::Float;
    yParamExpr->suffix = "y";

    auto yAssignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
    yAssignExpr->offset = offset;
    yAssignExpr->left = yParamExpr.get();
    yAssignExpr->right = yIndexExpr.get();
    yAssignExpr->declareLeft = true;
    block.append(yAssignExpr.get());

    outY = yParamExpr.get();

    expressions.push_back(std::move(yParamExpr));
    expressions.push_back(std::move(yIndexExpr));
    expressions.push_back(std::move(yAssignExpr));

    // Z

    auto zIndexExpr = std::make_shared<VectorIndexExpression>();
    zIndexExpr->offset = offset;
    zIndexExpr->vector = &vec;
    zIndexExpr->index = 2;

    auto zParamExpr = std::make_shared<ParameterExpression>();
    zParamExpr->offset = offset;
    zParamExpr->variableType = VariableType::Float;
    zParamExpr->suffix = "z";

    auto zAssignExpr = std::make_shared<BinaryExpression>(ExpressionType::Assign);
    zAssignExpr->offset = offset;
    zAssignExpr->left = zParamExpr.get();
    zAssignExpr->right = zIndexExpr.get();
    zAssignExpr->declareLeft = true;
    block.append(zAssignExpr.get());

    outZ = zParamExpr.get();

    expressions.push_back(std::move(zParamExpr));
    expressions.push_back(std::move(zIndexExpr));
    expressions.push_back(std::move(zAssignExpr));
}

bool ExpressionTree::isUnaryExpression(ExpressionType type) {
    switch (type) {
    case Negate:
    case Not:
    case OnesComplement:
    case Increment:
    case Decrement:
        return true;
    default:
        return false;
    }
}

bool ExpressionTree::isBinaryExpression(ExpressionType type) {
    switch (type) {
    case Assign:
    case Add:
    case Subtract:
    case Multiply:
    case Divide:
    case Modulo:
    case LogicalAnd:
    case LogicalOr:
    case BitwiseOr:
    case BitwiseExlusiveOr:
    case BitwiseAnd:
    case LeftShift:
    case RightShift:
    case RightShiftUnsigned:
    case Equal:
    case NotEqual:
    case GreaterThanOrEqual:
    case GreaterThan:
    case LessThan:
    case LessThanOrEqual:
        return true;
    default:
        return false;
    }
}

} // namespace script

} // namespace reone
