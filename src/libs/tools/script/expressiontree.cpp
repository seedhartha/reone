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

#include "reone/tools/script/expressiontree.h"

#include "reone/script/instrutil.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"
#include "reone/script/variableutil.h"
#include "reone/system/exception/argument.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"

using namespace std;

namespace reone {

namespace script {

ExpressionTree ExpressionTree::fromProgram(const ScriptProgram &program, IRoutines &routines) {
    auto startFunc = make_shared<Function>();
    startFunc->name = "_start";
    startFunc->offset = 13;

    auto functions = map<uint32_t, shared_ptr<Function>>();
    auto expressions = vector<shared_ptr<Expression>>();

    auto labels = unordered_map<uint32_t, LabelExpression *>();
    for (auto &ins : program.instructions()) {
        if (ins.type == InstructionType::JMP ||
            ins.type == InstructionType::JZ ||
            ins.type == InstructionType::JNZ ||
            ins.type == InstructionType::STORE_STATE) {
            auto offset = ins.offset + (ins.type == InstructionType::STORE_STATE ? 0x10 : ins.jumpOffset);
            auto label = make_shared<LabelExpression>();
            label->offset = offset;
            labels[offset] = label.get();
            expressions.push_back(std::move(label));
        }
    }

    auto ctx = make_shared<DecompilationContext>(program, routines, labels, functions, expressions);
    ctx->pushCallStack(startFunc.get());
    startFunc->block = decompileSafely(13, ctx);

    auto globals = set<const ParameterExpression *>();
    for (auto &expression : expressions) {
        if (expression->type != ExpressionType::Parameter) {
            continue;
        }
        auto paramExpr = static_cast<const ParameterExpression *>(expression.get());
        if (paramExpr->locality == ParameterLocality::Global) {
            globals.insert(paramExpr);
        }
    }

    ctx->functions[startFunc->offset] = std::move(startFunc);

    auto functionsVec = vector<shared_ptr<Function>>();
    for (auto it = ctx->functions.rbegin(); it != ctx->functions.rend(); ++it) {
        functionsVec.push_back(it->second);
    }

    return ExpressionTree(
        std::move(functionsVec),
        ctx->expressions,
        std::move(globals));
}

ExpressionTree::BlockExpression *ExpressionTree::decompileSafely(uint32_t start, shared_ptr<DecompilationContext> ctx) {
    try {
        return decompile(start, ctx);
    } catch (const logic_error &e) {
        error(boost::format("Block decompilation failed at %08x: %s") % start % string(e.what()));
        auto emptyBlock = make_shared<BlockExpression>();
        ctx->expressions.push_back(emptyBlock);
        return emptyBlock.get();
    }
}

ExpressionTree::BlockExpression *ExpressionTree::decompile(uint32_t start, shared_ptr<DecompilationContext> ctx) {
    debug(boost::format("Begin decompiling block at %08x") % start);

    auto block = make_shared<BlockExpression>();
    block->offset = start;

    for (uint32_t offset = start; offset < ctx->program.length();) {
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
            auto retExpr = make_shared<ReturnExpression>();
            retExpr->offset = ins.offset;

            if (ctx->callStack.size() == 1ll && !ctx->stack.empty()) {
                auto retVal = ctx->stack.back().param;
                retExpr->value = retVal;
                auto startFunc = ctx->topCall().function;
                startFunc->returnType = retVal->variableType;
            }

            block->append(retExpr.get());
            ctx->expressions.push_back(std::move(retExpr));
            break;

        } else if (ins.type == InstructionType::JMP) {
            auto absJumpOffset = ins.offset + ins.jumpOffset;

            auto gotoExpr = make_shared<GotoExpression>();
            gotoExpr->offset = ins.offset;
            gotoExpr->label = ctx->labels.at(absJumpOffset);

            if (ctx->branches->count(absJumpOffset) == 0) {
                ctx->branches->insert(make_pair(absJumpOffset, make_shared<DecompilationContext>(*ctx)));
            }

            block->append(gotoExpr.get());
            ctx->expressions.push_back(std::move(gotoExpr));
            break;

        } else if (ins.type == InstructionType::JSR) {
            auto absJumpOffset = ins.offset + ins.jumpOffset;
            shared_ptr<Function> sub;

            if (ctx->functions.count(absJumpOffset) == 0) {
                sub = make_shared<Function>();
                sub->offset = absJumpOffset;

                ctx->functions[sub->offset] = sub;

                auto inputs = map<int, ParameterExpression *>();
                auto outputs = map<int, ParameterExpression *>();
                auto branches = map<uint32_t, shared_ptr<DecompilationContext>>();

                auto subCtx = make_shared<DecompilationContext>(*ctx);
                subCtx->inputs = &inputs;
                subCtx->outputs = &outputs;
                subCtx->branches = &branches;
                subCtx->pushCallStack(sub.get());
                sub->block = decompileSafely(absJumpOffset, subCtx);

                for (auto &[branchOffset, branchCtx] : branches) {
                    auto branchBlock = decompileSafely(branchOffset, branchCtx);
                    for (auto &expression : branchBlock->expressions) {
                        sub->block->append(expression);
                    }
                }

                bool isMain = false;
                if (subCtx->callStack.size() == 2ll) {
                    if (subCtx->numGlobals > 0) {
                        sub->name = "_globals";
                    } else {
                        isMain = true;
                    }
                } else if (subCtx->callStack.size() == 3ll && ctx->numGlobals > 0) {
                    isMain = true;
                }
                if (isMain) {
                    sub->name = !subCtx->outputs->empty() ? "StartingConditional" : "main";
                }

                for (auto &[stackOffset, param] : inputs) {
                    sub->inputs.push_back(FunctionArgument(param->variableType, stackOffset));
                }
                for (auto &[stackOffset, param] : outputs) {
                    sub->outputs.push_back(FunctionArgument(param->variableType, stackOffset));
                }

            } else {
                sub = ctx->functions.at(absJumpOffset);
            }

            auto callExpr = make_shared<CallExpression>();
            callExpr->offset = ins.offset;
            callExpr->function = sub.get();
            block->append(callExpr.get());

            for (auto &argument : sub->inputs) {
                auto param = ctx->stack[ctx->stack.size() + argument.stackOffset].param;
                callExpr->arguments.push_back(param);
            }
            for (auto &argument : sub->outputs) {
                auto param = ctx->stack[ctx->stack.size() + argument.stackOffset].param;
                callExpr->arguments.push_back(param);
            }

            ctx->expressions.push_back(std::move(callExpr));

        } else if (ins.type == InstructionType::JZ ||
                   ins.type == InstructionType::JNZ) {
            auto absJumpOffset = ins.offset + ins.jumpOffset;

            auto leftExpr = ctx->stack.back().param;
            leftExpr->reads.push_back(ins.offset);
            ctx->stack.pop_back();

            auto rightExpr = make_shared<ConstantExpression>();
            rightExpr->offset = ins.offset;
            rightExpr->value = Variable::ofInt(0);

            auto testExpr = make_shared<BinaryExpression>(ins.type == InstructionType::JZ ? ExpressionType::Equal : ExpressionType::NotEqual);
            testExpr->offset = ins.offset;
            testExpr->left = leftExpr;
            testExpr->right = rightExpr.get();

            auto ifTrueGotoExpr = make_shared<GotoExpression>();
            ifTrueGotoExpr->offset = ins.offset;
            ifTrueGotoExpr->label = ctx->labels.at(absJumpOffset);

            auto ifTrueBlockExpr = make_shared<BlockExpression>();
            ifTrueBlockExpr->offset = ins.offset;
            ifTrueBlockExpr->append(ifTrueGotoExpr.get());

            auto condExpr = make_shared<ConditionalExpression>();
            condExpr->test = testExpr.get();
            condExpr->ifTrue = ifTrueBlockExpr.get();
            block->append(condExpr.get());

            if (ctx->branches->count(absJumpOffset) == 0) {
                ctx->branches->insert(make_pair(absJumpOffset, make_shared<DecompilationContext>(*ctx)));
            }

            ctx->expressions.push_back(std::move(rightExpr));
            ctx->expressions.push_back(std::move(testExpr));
            ctx->expressions.push_back(std::move(ifTrueGotoExpr));
            ctx->expressions.push_back(std::move(ifTrueBlockExpr));
            ctx->expressions.push_back(std::move(condExpr));

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

            auto paramExpr = make_shared<ParameterExpression>();
            paramExpr->offset = ins.offset;
            paramExpr->variableType = constExpr->value.type;
            block->append(paramExpr.get());

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = paramExpr.get();
            assignExpr->right = constExpr.get();
            block->append(assignExpr.get());
            paramExpr->assignments.push_back(ins.offset);
            paramExpr->assignedConst = constExpr.get();

            ctx->pushStack(paramExpr.get());
            ctx->expressions.push_back(std::move(constExpr));
            ctx->expressions.push_back(std::move(paramExpr));
            ctx->expressions.push_back(std::move(assignExpr));

        } else if (ins.type == InstructionType::ACTION) {
            auto &routine = ctx->routines.get(ins.routine);

            vector<Expression *> arguments;
            for (int i = 0; i < ins.argCount; ++i) {
                Expression *argument;
                auto argType = routine.getArgumentType(i);
                if (argType == VariableType::Vector) {
                    auto argZ = ctx->stack.back().param;
                    argZ->reads.push_back(ins.offset);
                    ctx->stack.pop_back();
                    auto argY = ctx->stack.back().param;
                    argY->reads.push_back(ins.offset);
                    ctx->stack.pop_back();
                    auto argX = ctx->stack.back().param;
                    argX->reads.push_back(ins.offset);
                    ctx->stack.pop_back();
                    argument = ctx->appendVectorCompose(ins.offset, *block, *argX, *argY, *argZ);
                } else if (argType == VariableType::Action) {
                    argument = ctx->savedAction;
                } else {
                    argument = ctx->stack.back().param;
                    static_cast<ParameterExpression *>(argument)->reads.push_back(ins.offset);
                    ctx->stack.pop_back();
                }
                if (!argument) {
                    throw ValidationException("Unable not extract action argument from stack");
                }
                arguments.push_back(argument);
            }

            auto actionExpr = make_shared<ActionExpression>();
            actionExpr->offset = ins.offset;
            actionExpr->action = ins.routine;
            actionExpr->arguments = std::move(arguments);

            if (routine.returnType() != VariableType::Void) {
                auto returnValue = make_shared<ParameterExpression>();
                returnValue->offset = ins.offset;
                returnValue->variableType = routine.returnType();
                block->append(returnValue.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = returnValue.get();
                assignExpr->right = actionExpr.get();
                block->append(assignExpr.get());
                returnValue->assignments.push_back(ins.offset);

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
            if (startIdx < 0) {
                throw ValidationException("Out of bounds stack access: " + to_string(startIdx));
            }
            auto numFrames = ins.size / 4;
            for (int i = 0; i < numFrames; ++i) {
                auto &left = ctx->stack[startIdx + numFrames - i - 1];
                auto &right = ctx->stack[stackSize - i - 1];
                right.param->reads.push_back(ins.offset);

                ParameterExpression *destination;
                if (left.allocatedBy != ctx->topCall().function && left.param->locality != ParameterLocality::Global) {
                    auto stackOffset = (ins.stackOffset / 4) + (stackSize - ctx->topCall().stackSizeOnEnter) + i;
                    if (ctx->outputs->count(stackOffset) == 0) {
                        (*ctx->outputs)[stackOffset] = left.param;
                    }
                    auto destExpr = make_shared<ParameterExpression>();
                    destExpr->offset = ins.offset;
                    destExpr->variableType = left.param->variableType;
                    destExpr->locality = ParameterLocality::Output;
                    destExpr->stackOffset = stackOffset;
                    destination = destExpr.get();
                    ctx->expressions.push_back(std::move(destExpr));
                } else {
                    destination = left.param;
                }

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = destination;
                assignExpr->right = right.param;
                block->append(assignExpr.get());
                destination->assignments.push_back(ins.offset);

                left = right.withAllocatedBy(*left.allocatedBy);
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
                throw ValidationException("Out of bounds stack access: " + to_string(startIdx));
            }
            auto numFrames = ins.size / 4;
            for (int i = 0; i < numFrames; ++i) {
                auto &frame = ctx->stack[startIdx + numFrames - i - 1];

                ParameterExpression *source;
                if (frame.allocatedBy != ctx->topCall().function && frame.param->locality != ParameterLocality::Global) {
                    auto stackOffset = (ins.stackOffset / 4) + (stackSize - ctx->topCall().stackSizeOnEnter) + i;
                    if (ctx->inputs->count(stackOffset) == 0) {
                        (*ctx->inputs)[stackOffset] = frame.param;
                    }
                    auto sourceExpr = make_shared<ParameterExpression>();
                    sourceExpr->offset = ins.offset;
                    sourceExpr->variableType = frame.param->variableType;
                    sourceExpr->locality = ParameterLocality::Input;
                    sourceExpr->stackOffset = stackOffset;
                    source = sourceExpr.get();
                    ctx->expressions.push_back(std::move(sourceExpr));
                } else {
                    source = frame.param;
                }
                source->reads.push_back(ins.offset);

                auto paramExpr = make_shared<ParameterExpression>();
                paramExpr->offset = ins.offset;
                paramExpr->variableType = source->variableType;
                paramExpr->suffix = to_string(i);
                block->append(paramExpr.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = paramExpr.get();
                assignExpr->right = source;
                block->append(assignExpr.get());
                paramExpr->assignments.push_back(ins.offset);

                auto frameCopy = StackFrame(frame);
                frameCopy.allocatedBy = ctx->topCall().function;
                frameCopy.param = paramExpr.get();
                ctx->stack.push_back(std::move(frameCopy));

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
            value->reads.push_back(ins.offset);
            ctx->stack.pop_back();

            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = value->variableType;
            block->append(resultExpr.get());

            ExpressionType type;
            if (ins.type == InstructionType::NEGI ||
                ins.type == InstructionType::NEGF) {
                type = ExpressionType::Negate;
            } else if (ins.type == InstructionType::COMPI) {
                type = ExpressionType::OnesComplement;
            } else if (ins.type == InstructionType::NOTI) {
                type = ExpressionType::Not;
            }
            auto unaryExpr = make_shared<UnaryExpression>(type);
            unaryExpr->offset = ins.offset;
            unaryExpr->operand = value;

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = resultExpr.get();
            assignExpr->right = unaryExpr.get();
            block->append(assignExpr.get());
            resultExpr->assignments.push_back(ins.offset);

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
            right->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto left = ctx->stack.back().param;
            left->reads.push_back(ins.offset);
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
            auto binaryExpr = make_shared<BinaryExpression>(type);
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
            auto result = make_shared<ParameterExpression>();
            result->offset = ins.offset;
            result->variableType = varType;
            block->append(result.get());

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = result.get();
            assignExpr->right = binaryExpr.get();
            block->append(assignExpr.get());
            result->assignments.push_back(ins.offset);

            ctx->pushStack(result.get());
            ctx->expressions.push_back(std::move(result));
            ctx->expressions.push_back(std::move(binaryExpr));
            ctx->expressions.push_back(std::move(assignExpr));

        } else if (ins.type == InstructionType::ADDVV ||
                   ins.type == InstructionType::SUBVV) {
            auto rightZ = ctx->stack.back().param;
            rightZ->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto rightY = ctx->stack.back().param;
            rightY->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto rightX = ctx->stack.back().param;
            rightX->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto right = ctx->appendVectorCompose(ins.offset, *block, *rightX, *rightY, *rightZ);

            auto leftZ = ctx->stack.back().param;
            leftZ->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto leftY = ctx->stack.back().param;
            leftY->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto leftX = ctx->stack.back().param;
            leftX->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto left = ctx->appendVectorCompose(ins.offset, *block, *leftX, *leftY, *leftZ);

            auto type = (ins.type == InstructionType::ADDVV) ? ExpressionType::Add : ExpressionType::Subtract;
            auto binaryExpr = make_shared<BinaryExpression>(type);
            binaryExpr->offset = ins.offset;
            binaryExpr->left = left;
            binaryExpr->right = right;

            auto result = make_shared<ParameterExpression>();
            result->offset = ins.offset;
            result->variableType = VariableType::Vector;
            block->append(result.get());

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = result.get();
            assignExpr->right = binaryExpr.get();
            block->append(assignExpr.get());
            result->assignments.push_back(ins.offset);

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
            rightZ->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto rightY = ctx->stack.back().param;
            rightY->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto rightX = ctx->stack.back().param;
            rightX->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto right = ctx->appendVectorCompose(ins.offset, *block, *rightX, *rightY, *rightZ);

            auto left = ctx->stack.back().param;
            left->reads.push_back(ins.offset);
            ctx->stack.pop_back();

            auto type = (ins.type == InstructionType::DIVFV) ? ExpressionType::Divide : ExpressionType::Multiply;
            auto binaryExpr = make_shared<BinaryExpression>(type);
            binaryExpr->offset = ins.offset;
            binaryExpr->left = left;
            binaryExpr->right = right;

            auto result = make_shared<ParameterExpression>();
            result->offset = ins.offset;
            result->variableType = VariableType::Vector;
            block->append(result.get());

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = result.get();
            assignExpr->right = binaryExpr.get();
            block->append(assignExpr.get());
            result->assignments.push_back(ins.offset);

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
            right->reads.push_back(ins.offset);
            ctx->stack.pop_back();

            auto leftZ = ctx->stack.back().param;
            leftZ->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto leftY = ctx->stack.back().param;
            leftY->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto leftX = ctx->stack.back().param;
            leftX->reads.push_back(ins.offset);
            ctx->stack.pop_back();
            auto left = ctx->appendVectorCompose(ins.offset, *block, *leftX, *leftY, *leftZ);

            auto type = (ins.type == InstructionType::DIVVF) ? ExpressionType::Divide : ExpressionType::Multiply;
            auto binaryExpr = make_shared<BinaryExpression>(type);
            binaryExpr->offset = ins.offset;
            binaryExpr->left = left;
            binaryExpr->right = right;

            auto result = make_shared<ParameterExpression>();
            result->offset = ins.offset;
            result->variableType = VariableType::Vector;
            block->append(result.get());

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = result.get();
            assignExpr->right = binaryExpr.get();
            block->append(assignExpr.get());
            result->assignments.push_back(ins.offset);

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
            vector<StackFrame> rightFrames;
            for (int i = 0; i < numFrames; ++i) {
                rightFrames.push_back(ctx->stack.back());
                rightFrames.back().param->reads.push_back(ins.offset);
                ctx->stack.pop_back();
            }
            vector<StackFrame> leftFrames;
            for (int i = 0; i < numFrames; ++i) {
                leftFrames.push_back(ctx->stack.back());
                leftFrames.back().param->reads.push_back(ins.offset);
                ctx->stack.pop_back();
            }

            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = VariableType::Int;
            block->append(resultExpr.get());

            for (int i = 0; i < numFrames; ++i) {
                auto firstType = (ins.type == InstructionType::EQUALTT) ? ExpressionType::Equal : ExpressionType::NotEqual;
                auto compExpr = make_shared<BinaryExpression>(firstType);
                compExpr->offset = ins.offset;
                compExpr->left = leftFrames[i].param;
                compExpr->right = rightFrames[i].param;

                auto secondType = (ins.type == InstructionType::EQUALTT) ? ExpressionType::LogicalAnd : ExpressionType::LogicalOr;
                auto andOrExpression = make_shared<BinaryExpression>(secondType);
                andOrExpression->offset = ins.offset;
                andOrExpression->left = resultExpr.get();
                andOrExpression->right = compExpr.get();

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = resultExpr.get();
                assignExpr->right = andOrExpression.get();
                block->append(assignExpr.get());
                resultExpr->assignments.push_back(ins.offset);

                ctx->expressions.push_back(std::move(compExpr));
                ctx->expressions.push_back(std::move(andOrExpression));
                ctx->expressions.push_back(std::move(assignExpr));
            }

            ctx->pushStack(resultExpr.get());
            ctx->expressions.push_back(std::move(resultExpr));

        } else if (ins.type == InstructionType::STORE_STATE) {
            auto absJumpOffset = ins.offset + 0x10;

            if (ctx->branches->count(absJumpOffset) == 0) {
                ctx->branches->insert(make_pair(absJumpOffset, make_shared<DecompilationContext>(*ctx)));
            }

            auto gotoExpr = make_shared<GotoExpression>();
            gotoExpr->offset = ins.offset;
            gotoExpr->label = ctx->labels.at(absJumpOffset);

            auto innerBlock = make_shared<BlockExpression>();
            innerBlock->offset = ins.offset;
            innerBlock->append(gotoExpr.get());

            ctx->savedAction = innerBlock.get();

            ctx->expressions.push_back(std::move(gotoExpr));
            ctx->expressions.push_back(std::move(innerBlock));

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
            if (frame.allocatedBy != ctx->topCall().function) {
                auto stackOffset = (ins.stackOffset / 4) + (stackSize - ctx->topCall().stackSizeOnEnter);
                if (ctx->outputs->count(stackOffset) == 0) {
                    (*ctx->outputs)[stackOffset] = frame.param;
                }
                auto destExpr = make_shared<ParameterExpression>();
                destExpr->offset = ins.offset;
                destExpr->variableType = frame.param->variableType;
                destExpr->locality = ParameterLocality::Output;
                destExpr->stackOffset = stackOffset;
                destination = destExpr.get();
                ctx->expressions.push_back(std::move(destExpr));
            } else {
                destination = frame.param;
            }
            destination->reads.push_back(ins.offset);

            ExpressionType type;
            if (ins.type == InstructionType::DECISP ||
                ins.type == InstructionType::DECIBP) {
                type = ExpressionType::Decrement;
            } else if (ins.type == InstructionType::INCISP ||
                       ins.type == InstructionType::INCIBP) {
                type = ExpressionType::Increment;
            }

            auto unaryExpr = make_shared<UnaryExpression>(type);
            unaryExpr->offset = ins.offset;
            unaryExpr->operand = destination;
            block->append(unaryExpr.get());
            destination->assignments.push_back(ins.offset);

            ctx->expressions.push_back(std::move(unaryExpr));

        } else if (ins.type == InstructionType::DESTRUCT) {
            auto numFrames = ins.size / 4;
            auto startNoDestroy = static_cast<int>(ctx->stack.size()) - numFrames + (ins.stackOffset / 4);
            auto numFramesNoDestroy = ins.sizeNoDestroy / 4;

            vector<StackFrame> framesNoDestroy;
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
            throw NotImplementedException("Cannot decompile expression of type " + to_string(static_cast<int>(ins.type)));
        }

        offset = ins.nextOffset;
    }

    // Cleanup block expressions
    for (auto it = block->expressions.begin(); it != block->expressions.end();) {
        auto expr = *it;
        if (expr->type == ExpressionType::Parameter) {
            // Remove redundant declaration
            it = block->expressions.erase(it);
            continue;
        } else if (expr->type == ExpressionType::Assign) {
            auto assignExpr = static_cast<BinaryExpression *>(expr);
            if (assignExpr->left->type == ExpressionType::Parameter && assignExpr->right->type == ExpressionType::Constant) {
                // Remove param to constant assignment if it's the only one and param is read less than twice
                auto leftParam = static_cast<ParameterExpression *>(assignExpr->left);
                if (leftParam->assignments.size() == 1l &&
                    leftParam->assignments.front() == expr->offset &&
                    leftParam->reads.size() < 2ll) {
                    it = block->expressions.erase(it);
                    continue;
                }
            } else if (assignExpr->left->type == ExpressionType::Parameter && assignExpr->right->type == ExpressionType::Action) {
                // Remove param to function return value assignment if param is never read
                auto leftParam = static_cast<ParameterExpression *>(assignExpr->left);
                if (leftParam->reads.empty()) {
                    it = block->expressions.erase(it);
                    it = block->expressions.insert(it, assignExpr->right);
                    continue;
                }
            }
        }
        ++it;
    }
    if (!block->expressions.empty() && block->expressions.back()->type == ExpressionType::Return) {
        block->expressions.pop_back();
    }

    debug(boost::format("End decompiling block at %08x") % start);
    ctx->expressions.push_back(block);

    return block.get();
}

unique_ptr<ExpressionTree::ConstantExpression> ExpressionTree::constantExpression(const Instruction &ins) {
    switch (ins.type) {
    case InstructionType::CONSTI:
    case InstructionType::CONSTF:
    case InstructionType::CONSTS:
    case InstructionType::CONSTO: {
        auto constExpr = make_unique<ConstantExpression>();
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
        return std::move(constExpr);
    }
    default:
        throw ArgumentException("Instruction is not of CONSTx type: " + to_string(static_cast<int>(ins.type)));
    }
}

unique_ptr<ExpressionTree::ParameterExpression> ExpressionTree::parameterExpression(const Instruction &ins) {
    switch (ins.type) {
    case InstructionType::RSADDI:
    case InstructionType::RSADDF:
    case InstructionType::RSADDS:
    case InstructionType::RSADDO:
    case InstructionType::RSADDEFF:
    case InstructionType::RSADDEVT:
    case InstructionType::RSADDLOC:
    case InstructionType::RSADDTAL: {
        auto paramExpr = make_unique<ParameterExpression>();
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
        return std::move(paramExpr);
    }
    default:
        throw ArgumentException("Instruction is not of RSADDx type: " + to_string(static_cast<int>(ins.type)));
    }
}

ExpressionTree::VectorExpression *ExpressionTree::DecompilationContext::appendVectorCompose(
    uint32_t offset,
    BlockExpression &block,
    ParameterExpression &x,
    ParameterExpression &y,
    ParameterExpression &z) {

    if ((x.variableType != VariableType::Float) ||
        (y.variableType != VariableType::Float) ||
        (z.variableType != VariableType::Float)) {
        throw ArgumentException("Cannot compose a vector of non-floats");
    }

    auto vecExpr = make_shared<VectorExpression>();
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

    vec.reads.push_back(offset);

    // X

    auto xIndexExpr = make_shared<VectorIndexExpression>();
    xIndexExpr->offset = offset;
    xIndexExpr->vector = &vec;
    xIndexExpr->index = 0;

    auto xParamExpr = make_shared<ParameterExpression>();
    xParamExpr->offset = offset;
    xParamExpr->variableType = VariableType::Float;
    xParamExpr->suffix = "x";
    block.append(xParamExpr.get());

    auto xAssignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
    xAssignExpr->offset = offset;
    xAssignExpr->left = xParamExpr.get();
    xAssignExpr->right = xIndexExpr.get();
    block.append(xAssignExpr.get());
    xParamExpr->assignments.push_back(offset);

    outX = xParamExpr.get();

    expressions.push_back(std::move(xParamExpr));
    expressions.push_back(std::move(xIndexExpr));
    expressions.push_back(std::move(xAssignExpr));

    // Y

    auto yIndexExpr = make_shared<VectorIndexExpression>();
    yIndexExpr->offset = offset;
    yIndexExpr->vector = &vec;
    yIndexExpr->index = 1;

    auto yParamExpr = make_shared<ParameterExpression>();
    yParamExpr->offset = offset;
    yParamExpr->variableType = VariableType::Float;
    yParamExpr->suffix = "y";
    block.append(yParamExpr.get());

    auto yAssignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
    yAssignExpr->offset = offset;
    yAssignExpr->left = yParamExpr.get();
    yAssignExpr->right = yIndexExpr.get();
    block.append(yAssignExpr.get());
    yParamExpr->assignments.push_back(offset);

    outY = yParamExpr.get();

    expressions.push_back(std::move(yParamExpr));
    expressions.push_back(std::move(yIndexExpr));
    expressions.push_back(std::move(yAssignExpr));

    // Z

    auto zIndexExpr = make_shared<VectorIndexExpression>();
    zIndexExpr->offset = offset;
    zIndexExpr->vector = &vec;
    zIndexExpr->index = 2;

    auto zParamExpr = make_shared<ParameterExpression>();
    zParamExpr->offset = offset;
    zParamExpr->variableType = VariableType::Float;
    zParamExpr->suffix = "z";
    block.append(zParamExpr.get());

    auto zAssignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
    zAssignExpr->offset = offset;
    zAssignExpr->left = zParamExpr.get();
    zAssignExpr->right = zIndexExpr.get();
    block.append(zAssignExpr.get());
    zParamExpr->assignments.push_back(offset);

    outZ = zParamExpr.get();

    expressions.push_back(std::move(zParamExpr));
    expressions.push_back(std::move(zIndexExpr));
    expressions.push_back(std::move(zAssignExpr));
}

} // namespace script

} // namespace reone
