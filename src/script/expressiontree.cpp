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

#include "expressiontree.h"

#include "../common/exception/argument.h"
#include "../common/exception/notimplemented.h"
#include "../common/exception/validation.h"
#include "../common/logutil.h"

#include "instrutil.h"
#include "routine.h"
#include "routines.h"
#include "variableutil.h"

using namespace std;

namespace reone {

namespace script {

ExpressionTree ExpressionTree::fromProgram(const ScriptProgram &program, const IRoutines &routines) {
    auto startFunc = make_shared<Function>();
    startFunc->name = "_start";
    startFunc->offset = 13;

    auto functions = vector<shared_ptr<Function>>();
    auto expressions = vector<shared_ptr<Expression>>();

    auto labels = unordered_map<uint32_t, LabelExpression *>();
    for (auto &ins : program.instructions()) {
        if (ins.type == InstructionType::JMP ||
            ins.type == InstructionType::JZ ||
            ins.type == InstructionType::JNZ) {
            auto offset = ins.offset + ins.jumpOffset;
            auto label = make_shared<LabelExpression>();
            label->offset = offset;
            labels[offset] = label.get();
            expressions.push_back(move(label));
        }
    }

    auto ctx = DecompilationContext(program, routines, labels, functions, expressions);
    ctx.callStack.push_back(CallStackFrame(startFunc.get()));
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

    ctx.functions.push_back(move(startFunc));

    return ExpressionTree(
        ctx.functions,
        ctx.expressions,
        move(globals));
}

ExpressionTree::BlockExpression *ExpressionTree::decompileSafely(uint32_t start, DecompilationContext &ctx) {
    try {
        return decompile(start, ctx);
    } catch (const ValidationException &e) {
        error(boost::format("Block decompilation failed at %08x: %s") % start % string(e.what()));
        auto emptyBlock = make_shared<BlockExpression>();
        ctx.expressions.push_back(emptyBlock);
        return emptyBlock.get();
    }
}

ExpressionTree::BlockExpression *ExpressionTree::decompile(uint32_t start, DecompilationContext &ctx) {
    debug(boost::format("Begin decompiling block at %08x") % start);

    auto block = make_shared<BlockExpression>();
    block->offset = start;

    for (uint32_t offset = start; offset < ctx.program.length();) {
        auto maybeLabel = ctx.labels.find(offset);
        if (maybeLabel != ctx.labels.end()) {
            block->insert(maybeLabel->second);
        }

        debug(boost::format("Stack: size=%d") % ctx.stack.size());
        for (auto it = ctx.stack.rbegin(); it != ctx.stack.rend(); ++it) {
            auto type = describeVariableType(it->param->variableType);
            debug("    " + type);
        }

        auto &ins = ctx.program.getInstruction(offset);
        debug(boost::format("Decompiling instruction at %08x of type %s") % offset % describeInstructionType(ins.type));

        if (ins.type == InstructionType::NOP ||
            ins.type == InstructionType::NOP2) {

        } else if (ins.type == InstructionType::RETN) {
            auto retExpr = make_shared<ReturnExpression>();
            retExpr->offset = ins.offset;

            if (ctx.callStack.size() == 1ll && !ctx.stack.empty()) {
                auto retVal = ctx.stack.back().param;
                retExpr->value = retVal;
                auto startFunc = ctx.callStack.back().function;
                startFunc->returnType = retVal->variableType;
            }

            block->insert(retExpr.get());
            ctx.expressions.push_back(move(retExpr));
            break;

        } else if (ins.type == InstructionType::JMP) {
            auto absJumpOffset = ins.offset + ins.jumpOffset;

            auto gotoExpr = make_shared<GotoExpression>();
            gotoExpr->offset = ins.offset;
            gotoExpr->label = ctx.labels.at(absJumpOffset);

            if (ctx.branches->count(absJumpOffset) == 0 && ins.jumpOffset > 0) {
                auto branchCtx = DecompilationContext(ctx);
                ctx.branches->insert(make_pair(absJumpOffset, decompile(absJumpOffset, branchCtx)));
            }

            block->insert(gotoExpr.get());
            ctx.expressions.push_back(move(gotoExpr));
            break;

        } else if (ins.type == InstructionType::JSR) {
            auto absJumpOffset = ins.offset + ins.jumpOffset;

            auto sub = make_shared<Function>();
            sub->offset = absJumpOffset;

            auto subCtx = DecompilationContext(ctx);
            subCtx.callStack.push_back(CallStackFrame(sub.get()));
            auto inputs = vector<ParameterExpression *>();
            subCtx.inputs = &inputs;
            auto outputs = vector<ParameterExpression *>();
            subCtx.outputs = &outputs;
            auto branches = map<uint32_t, BlockExpression *>();
            subCtx.branches = &branches;

            sub->block = decompileSafely(absJumpOffset, subCtx);
            for (auto &[branchOffset, branchBlock] : branches) {
                if (sub->block->contains(branchOffset)) {
                    continue;
                }
                for (auto &expression : branchBlock->expressions) {
                    sub->block->insert(expression);
                }
            }

            bool isMain = false;
            if (subCtx.callStack.size() == 2ll) {
                if (subCtx.numGlobals > 0) {
                    sub->name = "_globals";
                } else {
                    isMain = true;
                }
            } else if (subCtx.callStack.size() == 3ll && ctx.numGlobals > 0) {
                isMain = true;
            }
            if (isMain) {
                sub->name = !subCtx.outputs->empty() ? "StartingConditional" : "main";
            }

            auto callExpr = make_shared<CallExpression>();
            callExpr->offset = ins.offset;
            callExpr->function = sub.get();
            block->insert(callExpr.get());

            for (auto &input : *subCtx.inputs) {
                sub->inArgumentTypes.push_back(input->variableType);
                callExpr->arguments.push_back(input);
            }
            for (auto &output : *subCtx.outputs) {
                sub->outArgumentTypes.push_back(output->variableType);
                callExpr->arguments.push_back(output);
            }
            subCtx.callStack.pop_back();

            ctx.functions.push_back(move(sub));
            ctx.expressions.push_back(move(callExpr));

        } else if (ins.type == InstructionType::JZ ||
                   ins.type == InstructionType::JNZ) {
            auto absJumpOffset = ins.offset + ins.jumpOffset;

            auto leftExpr = ctx.stack.back().param;
            ctx.stack.pop_back();

            auto rightExpr = make_shared<ConstantExpression>();
            rightExpr->offset = ins.offset;
            rightExpr->value = Variable::ofInt(0);

            auto testExpr = make_shared<BinaryExpression>(ins.type == InstructionType::JZ ? ExpressionType::Equal : ExpressionType::NotEqual);
            testExpr->offset = ins.offset;
            testExpr->left = leftExpr;
            testExpr->right = rightExpr.get();

            auto ifTrueGotoExpr = make_shared<GotoExpression>();
            ifTrueGotoExpr->offset = ins.offset;
            ifTrueGotoExpr->label = ctx.labels.at(absJumpOffset);

            auto ifTrueBlockExpr = make_shared<BlockExpression>();
            ifTrueBlockExpr->offset = ins.offset;
            ifTrueBlockExpr->insert(ifTrueGotoExpr.get());

            auto condExpr = make_shared<ConditionalExpression>();
            condExpr->test = testExpr.get();
            condExpr->ifTrue = ifTrueBlockExpr.get();
            block->insert(condExpr.get());

            if (ctx.branches->count(absJumpOffset) == 0 && ins.jumpOffset > 0) {
                auto branchCtx = DecompilationContext(ctx);
                ctx.branches->insert(make_pair(absJumpOffset, decompileSafely(absJumpOffset, branchCtx)));
            }

            ctx.expressions.push_back(move(rightExpr));
            ctx.expressions.push_back(move(testExpr));
            ctx.expressions.push_back(move(ifTrueGotoExpr));
            ctx.expressions.push_back(move(ifTrueBlockExpr));
            ctx.expressions.push_back(move(condExpr));

        } else if (ins.type == InstructionType::RSADDI ||
                   ins.type == InstructionType::RSADDF ||
                   ins.type == InstructionType::RSADDS ||
                   ins.type == InstructionType::RSADDO ||
                   ins.type == InstructionType::RSADDEFF ||
                   ins.type == InstructionType::RSADDEVT ||
                   ins.type == InstructionType::RSADDLOC ||
                   ins.type == InstructionType::RSADDTAL) {
            auto expression = parameterExpression(ins);
            block->insert(expression.get());
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, expression.get()));
            ctx.expressions.push_back(move(expression));

        } else if (ins.type == InstructionType::CONSTI ||
                   ins.type == InstructionType::CONSTF ||
                   ins.type == InstructionType::CONSTS ||
                   ins.type == InstructionType::CONSTO) {
            auto constExpr = constantExpression(ins);

            auto paramExpr = make_shared<ParameterExpression>();
            paramExpr->offset = ins.offset;
            paramExpr->variableType = constExpr->value.type;
            block->insert(paramExpr.get());

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = paramExpr.get();
            assignExpr->right = constExpr.get();
            block->insert(assignExpr.get());

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, paramExpr.get()));
            ctx.expressions.push_back(move(constExpr));
            ctx.expressions.push_back(move(paramExpr));
            ctx.expressions.push_back(move(assignExpr));

        } else if (ins.type == InstructionType::ACTION) {
            auto &routine = ctx.routines.get(ins.routine);

            vector<Expression *> arguments;
            for (int i = 0; i < ins.argCount; ++i) {
                Expression *argument;
                auto argType = routine.getArgumentType(i);
                if (argType == VariableType::Vector) {
                    auto xComp = ctx.stack.back().param;
                    ctx.stack.pop_back();
                    auto yComp = ctx.stack.back().param;
                    ctx.stack.pop_back();
                    auto zComp = ctx.stack.back().param;
                    ctx.stack.pop_back();
                    if (xComp->variableType != VariableType::Float ||
                        yComp->variableType != VariableType::Float ||
                        zComp->variableType != VariableType::Float) {
                        throw ValidationException("Not a vector on top of the stack");
                    }
                    auto vecExpr = make_shared<VectorExpression>();
                    vecExpr->offset = ins.offset;
                    vecExpr->components.push_back(xComp);
                    vecExpr->components.push_back(yComp);
                    vecExpr->components.push_back(zComp);
                    argument = vecExpr.get();
                    ctx.expressions.push_back(move(vecExpr));
                } else if (argType == VariableType::Action) {
                    argument = ctx.savedAction;
                } else {
                    argument = ctx.stack.back().param;
                    ctx.stack.pop_back();
                }
                if (!argument) {
                    throw ValidationException("Unable not extract action argument from stack");
                }
                arguments.push_back(argument);
            }

            auto actionExpr = make_shared<ActionExpression>();
            actionExpr->offset = ins.offset;
            actionExpr->action = ins.routine;
            actionExpr->arguments = move(arguments);

            if (routine.returnType() != VariableType::Void) {
                auto retValExpr = make_shared<ParameterExpression>();
                retValExpr->offset = ins.offset;
                retValExpr->variableType = routine.returnType();
                block->insert(retValExpr.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = retValExpr.get();
                assignExpr->right = actionExpr.get();
                block->insert(assignExpr.get());

                if (routine.returnType() == VariableType::Vector) {
                    // X

                    auto indexXExpr = make_shared<VectorIndexExpression>();
                    indexXExpr->offset = ins.offset;
                    indexXExpr->vector = retValExpr.get();
                    indexXExpr->index = 0;

                    auto retValXExpr = make_shared<ParameterExpression>();
                    retValXExpr->offset = ins.offset;
                    retValXExpr->variableType = VariableType::Float;
                    retValXExpr->index = 0;
                    block->insert(retValXExpr.get());

                    auto assignXExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignXExpr->offset = ins.offset;
                    assignXExpr->left = retValXExpr.get();
                    assignXExpr->right = indexXExpr.get();
                    block->insert(assignXExpr.get());

                    // Y

                    auto indexYExpr = make_shared<VectorIndexExpression>();
                    indexYExpr->offset = ins.offset;
                    indexYExpr->vector = retValExpr.get();
                    indexYExpr->index = 1;

                    auto retValYExpr = make_shared<ParameterExpression>();
                    retValYExpr->offset = ins.offset;
                    retValYExpr->variableType = VariableType::Float;
                    retValYExpr->index = 1;
                    block->insert(retValYExpr.get());

                    auto assignYExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignYExpr->offset = ins.offset;
                    assignYExpr->left = retValYExpr.get();
                    assignYExpr->right = indexYExpr.get();
                    block->insert(assignYExpr.get());

                    // Z

                    auto indexZExpr = make_shared<VectorIndexExpression>();
                    indexZExpr->offset = ins.offset;
                    indexZExpr->vector = retValExpr.get();
                    indexZExpr->index = 2;

                    auto retValZExpr = make_shared<ParameterExpression>();
                    retValZExpr->offset = ins.offset;
                    retValZExpr->variableType = VariableType::Float;
                    retValZExpr->index = 2;
                    block->insert(retValZExpr.get());

                    auto assignZExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                    assignZExpr->offset = ins.offset;
                    assignZExpr->left = retValZExpr.get();
                    assignZExpr->right = indexZExpr.get();
                    block->insert(assignZExpr.get());

                    //

                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValZExpr.get()));
                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValYExpr.get()));
                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValXExpr.get()));
                    ctx.expressions.push_back(move(indexXExpr));
                    ctx.expressions.push_back(move(retValXExpr));
                    ctx.expressions.push_back(move(assignXExpr));
                    ctx.expressions.push_back(move(indexYExpr));
                    ctx.expressions.push_back(move(retValYExpr));
                    ctx.expressions.push_back(move(assignYExpr));
                    ctx.expressions.push_back(move(indexZExpr));
                    ctx.expressions.push_back(move(retValZExpr));
                    ctx.expressions.push_back(move(assignZExpr));
                } else {
                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValExpr.get()));
                }
                ctx.expressions.push_back(move(retValExpr));
                ctx.expressions.push_back(move(assignExpr));

            } else {
                block->insert(actionExpr.get());
            }

            ctx.expressions.push_back(move(actionExpr));

        } else if (ins.type == InstructionType::CPDOWNSP ||
                   ins.type == InstructionType::CPDOWNBP) {
            auto stackSize = static_cast<int>(ctx.stack.size());
            if (ins.stackOffset >= 0) {
                throw ValidationException("Non-negative stack offsets are not supported");
            }
            auto startIdx = (ins.type == InstructionType::CPDOWNSP ? stackSize : ctx.numGlobals) + (ins.stackOffset / 4);
            if (startIdx < 0) {
                throw ValidationException("Out of bounds stack access: " + to_string(startIdx));
            }
            auto numFrames = ins.size / 4;
            for (int i = 0; i < numFrames; ++i) {
                auto &left = ctx.stack[startIdx + numFrames - i - 1];
                auto &right = ctx.stack[stackSize - i - 1];

                ParameterExpression *destination;
                if (left.allocatedBy != ctx.callStack.back().function && left.param->locality != ParameterLocality::Global) {
                    int index = -1;
                    for (size_t j = 0; j < ctx.outputs->size(); ++j) {
                        if ((*ctx.outputs)[j] == left.param) {
                            index = j;
                        }
                    }
                    if (index == -1) {
                        index = ctx.outputs->size();
                        ctx.outputs->push_back(left.param);
                    }
                    auto destExpr = make_shared<ParameterExpression>();
                    destExpr->offset = ins.offset;
                    destExpr->variableType = left.param->variableType;
                    destExpr->locality = ParameterLocality::Output;
                    destExpr->index = index;
                    destination = destExpr.get();
                    ctx.expressions.push_back(move(destExpr));
                } else {
                    destination = left.param;
                }

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = destination;
                assignExpr->right = right.param;
                block->insert(assignExpr.get());

                left = right.withAllocatedBy(*left.allocatedBy);
                ctx.expressions.push_back(move(assignExpr));
            }

        } else if (ins.type == InstructionType::CPTOPSP ||
                   ins.type == InstructionType::CPTOPBP) {
            auto stackSize = static_cast<int>(ctx.stack.size());
            if (ins.stackOffset >= 0) {
                throw ValidationException("Non-negative stack offsets are not supported");
            }
            auto startIdx = (ins.type == InstructionType::CPTOPSP ? stackSize : ctx.numGlobals) + (ins.stackOffset / 4);
            if (startIdx < 0) {
                throw ValidationException("Out of bounds stack access: " + to_string(startIdx));
            }
            auto numFrames = ins.size / 4;
            for (int i = 0; i < numFrames; ++i) {
                auto &frame = ctx.stack[startIdx + numFrames - i - 1];

                ParameterExpression *source;
                if (frame.allocatedBy != ctx.callStack.back().function && frame.param->locality != ParameterLocality::Global) {
                    int index = -1;
                    for (size_t j = 0; j < ctx.inputs->size(); ++j) {
                        if ((*ctx.inputs)[j] == frame.param) {
                            index = j;
                        }
                    }
                    if (index == -1) {
                        index = ctx.inputs->size();
                        ctx.inputs->push_back(frame.param);
                    }
                    auto sourceExpr = make_shared<ParameterExpression>();
                    sourceExpr->offset = ins.offset;
                    sourceExpr->variableType = frame.param->variableType;
                    sourceExpr->locality = ParameterLocality::Input;
                    sourceExpr->index = index;
                    source = sourceExpr.get();
                    ctx.expressions.push_back(move(sourceExpr));
                } else {
                    source = frame.param;
                }

                auto paramExpr = make_shared<ParameterExpression>();
                paramExpr->offset = ins.offset;
                paramExpr->variableType = source->variableType;
                paramExpr->index = i;
                block->insert(paramExpr.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = paramExpr.get();
                assignExpr->right = source;
                block->insert(assignExpr.get());

                auto frameCopy = StackFrame(frame);
                frameCopy.allocatedBy = ctx.callStack.back().function;
                frameCopy.param = paramExpr.get();
                ctx.stack.push_back(move(frameCopy));

                ctx.expressions.push_back(move(paramExpr));
                ctx.expressions.push_back(move(assignExpr));
            }

        } else if (ins.type == InstructionType::MOVSP) {
            if (ins.stackOffset >= 0) {
                throw ValidationException("Non-negative stack offsets are not supported");
            }
            for (int i = 0; i < -ins.stackOffset / 4; ++i) {
                ctx.stack.pop_back();
            }
        } else if (ins.type == InstructionType::NEGI ||
                   ins.type == InstructionType::NEGF ||
                   ins.type == InstructionType::COMPI ||
                   ins.type == InstructionType::NOTI) {
            auto value = ctx.stack.back().param;
            ctx.stack.pop_back();

            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = value->variableType;
            block->insert(resultExpr.get());

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
            block->insert(assignExpr.get());

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultExpr.get()));
            ctx.expressions.push_back(move(resultExpr));
            ctx.expressions.push_back(move(unaryExpr));
            ctx.expressions.push_back(move(assignExpr));

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
            auto &right = ctx.stack.back();
            ctx.stack.pop_back();
            auto &left = ctx.stack.back();
            ctx.stack.pop_back();

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
            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = varType;
            block->insert(resultExpr.get());

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
            binaryExpr->left = left.param;
            binaryExpr->right = right.param;

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = resultExpr.get();
            assignExpr->right = binaryExpr.get();
            block->insert(assignExpr.get());

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultExpr.get()));
            ctx.expressions.push_back(move(resultExpr));
            ctx.expressions.push_back(move(binaryExpr));
            ctx.expressions.push_back(move(assignExpr));

        } else if (ins.type == InstructionType::ADDVV ||
                   ins.type == InstructionType::SUBVV) {
            auto &rightX = ctx.stack.back().param;
            ctx.stack.pop_back();
            auto &rightY = ctx.stack.back().param;
            ctx.stack.pop_back();
            auto &rightZ = ctx.stack.back().param;
            ctx.stack.pop_back();
            if ((rightX->variableType != VariableType::Float) ||
                (rightY->variableType != VariableType::Float) ||
                (rightZ->variableType != VariableType::Float)) {
                throw ValidationException("Not a vector on top of the stack");
            }
            auto rightVecExpr = make_shared<VectorExpression>();
            rightVecExpr->offset = ins.offset;
            rightVecExpr->components.push_back(rightX);
            rightVecExpr->components.push_back(rightY);
            rightVecExpr->components.push_back(rightZ);

            auto &leftX = ctx.stack.back().param;
            ctx.stack.pop_back();
            auto &leftY = ctx.stack.back().param;
            ctx.stack.pop_back();
            auto &leftZ = ctx.stack.back().param;
            ctx.stack.pop_back();
            if ((leftX->variableType != VariableType::Float) ||
                (leftY->variableType != VariableType::Float) ||
                (leftZ->variableType != VariableType::Float)) {
                throw ValidationException("Not a vector on top of the stack");
            }
            auto leftVecExpr = make_shared<VectorExpression>();
            leftVecExpr->offset = ins.offset;
            leftVecExpr->components.push_back(leftX);
            leftVecExpr->components.push_back(leftY);
            leftVecExpr->components.push_back(leftZ);

            auto resultVecExpr = make_shared<ParameterExpression>();
            resultVecExpr->offset = ins.offset;
            resultVecExpr->variableType = VariableType::Vector;
            block->insert(resultVecExpr.get());

            auto type = (ins.type == InstructionType::ADDVV) ? ExpressionType::Add : ExpressionType::Subtract;
            auto binaryExpr = make_shared<BinaryExpression>(type);
            binaryExpr->offset = ins.offset;
            binaryExpr->left = leftVecExpr.get();
            binaryExpr->right = rightVecExpr.get();

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = resultVecExpr.get();
            assignExpr->right = binaryExpr.get();
            block->insert(assignExpr.get());

            // X

            auto indexXExpr = make_shared<VectorIndexExpression>();
            indexXExpr->offset = ins.offset;
            indexXExpr->vector = resultVecExpr.get();
            indexXExpr->index = 0;

            auto resultXExpr = make_shared<ParameterExpression>();
            resultXExpr->offset = ins.offset;
            resultXExpr->variableType = VariableType::Float;
            resultXExpr->index = 0;
            block->insert(resultXExpr.get());

            auto assignXExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignXExpr->offset = ins.offset;
            assignXExpr->left = resultXExpr.get();
            assignXExpr->right = indexXExpr.get();
            block->insert(assignXExpr.get());

            // Y

            auto indexYExpr = make_shared<VectorIndexExpression>();
            indexYExpr->offset = ins.offset;
            indexYExpr->vector = resultVecExpr.get();
            indexYExpr->index = 1;

            auto resultYExpr = make_shared<ParameterExpression>();
            resultYExpr->offset = ins.offset;
            resultYExpr->variableType = VariableType::Float;
            resultYExpr->index = 1;
            block->insert(resultYExpr.get());

            auto assignYExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignYExpr->offset = ins.offset;
            assignYExpr->left = resultYExpr.get();
            assignYExpr->right = indexYExpr.get();
            block->insert(assignYExpr.get());

            // Z

            auto indexZExpr = make_shared<VectorIndexExpression>();
            indexZExpr->offset = ins.offset;
            indexZExpr->vector = resultVecExpr.get();
            indexZExpr->index = 2;

            auto resultZExpr = make_shared<ParameterExpression>();
            resultZExpr->offset = ins.offset;
            resultZExpr->variableType = VariableType::Float;
            resultZExpr->index = 2;
            block->insert(resultZExpr.get());

            auto assignZExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignZExpr->offset = ins.offset;
            assignZExpr->left = indexZExpr.get();
            assignZExpr->right = resultZExpr.get();
            block->insert(assignZExpr.get());

            //

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultZExpr.get()));
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultYExpr.get()));
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultXExpr.get()));
            ctx.expressions.push_back(move(rightVecExpr));
            ctx.expressions.push_back(move(leftVecExpr));
            ctx.expressions.push_back(move(resultVecExpr));
            ctx.expressions.push_back(move(binaryExpr));
            ctx.expressions.push_back(move(assignExpr));
            ctx.expressions.push_back(move(indexXExpr));
            ctx.expressions.push_back(move(resultXExpr));
            ctx.expressions.push_back(move(assignXExpr));
            ctx.expressions.push_back(move(indexYExpr));
            ctx.expressions.push_back(move(resultYExpr));
            ctx.expressions.push_back(move(assignYExpr));
            ctx.expressions.push_back(move(indexZExpr));
            ctx.expressions.push_back(move(resultZExpr));
            ctx.expressions.push_back(move(assignZExpr));

        } else if (ins.type == InstructionType::DIVFV ||
                   ins.type == InstructionType::MULFV) {
            auto &rightX = ctx.stack.back();
            ctx.stack.pop_back();
            auto &rightY = ctx.stack.back();
            ctx.stack.pop_back();
            auto &rightZ = ctx.stack.back();
            ctx.stack.pop_back();
            if ((rightX.param->variableType != VariableType::Float) ||
                (rightY.param->variableType != VariableType::Float) ||
                (rightZ.param->variableType != VariableType::Float)) {
                throw ValidationException("Not a vector on top of the stack");
            }

            auto &left = ctx.stack.back();
            ctx.stack.pop_back();
            if (left.param->variableType != VariableType::Float) {
                throw ValidationException("Not a float on top of the stack");
            }

            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = VariableType::Vector;
            block->insert(resultExpr.get());

            auto type = (ins.type == InstructionType::DIVFV) ? ExpressionType::Divide : ExpressionType::Multiply;
            auto binaryExpr = make_shared<BinaryExpression>(type);
            binaryExpr->offset = ins.offset;
            binaryExpr->left = left.param;
            binaryExpr->right = rightX.param;

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = resultExpr.get();
            assignExpr->right = binaryExpr.get();
            block->insert(assignExpr.get());

            // X

            auto indexXExpr = make_shared<VectorIndexExpression>();
            indexXExpr->offset = ins.offset;
            indexXExpr->vector = resultExpr.get();
            indexXExpr->index = 0;

            auto resultXExpr = make_shared<ParameterExpression>();
            resultXExpr->offset = ins.offset;
            resultXExpr->variableType = VariableType::Float;
            resultXExpr->index = 0;
            block->insert(resultXExpr.get());

            auto assignXExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignXExpr->offset = ins.offset;
            assignXExpr->left = resultXExpr.get();
            assignXExpr->right = indexXExpr.get();
            block->insert(assignXExpr.get());

            // Y

            auto indexYExpr = make_shared<VectorIndexExpression>();
            indexYExpr->offset = ins.offset;
            indexYExpr->vector = resultExpr.get();
            indexYExpr->index = 1;

            auto resultYExpr = make_shared<ParameterExpression>();
            resultYExpr->offset = ins.offset;
            resultYExpr->variableType = VariableType::Float;
            resultYExpr->index = 1;
            block->insert(resultYExpr.get());

            auto assignYExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignYExpr->offset = ins.offset;
            assignYExpr->left = resultYExpr.get();
            assignYExpr->right = indexYExpr.get();
            block->insert(assignYExpr.get());

            // Z

            auto indexZExpr = make_shared<VectorIndexExpression>();
            indexZExpr->offset = ins.offset;
            indexZExpr->vector = resultExpr.get();
            indexZExpr->index = 2;

            auto resultZExpr = make_shared<ParameterExpression>();
            resultZExpr->offset = ins.offset;
            resultZExpr->variableType = VariableType::Float;
            resultZExpr->index = 2;
            block->insert(resultZExpr.get());

            auto assignZExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignZExpr->offset = ins.offset;
            assignZExpr->left = indexZExpr.get();
            assignZExpr->right = resultZExpr.get();
            block->insert(assignZExpr.get());

            //

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultZExpr.get()));
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultYExpr.get()));
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultXExpr.get()));
            ctx.expressions.push_back(move(resultExpr));
            ctx.expressions.push_back(move(binaryExpr));
            ctx.expressions.push_back(move(assignExpr));
            ctx.expressions.push_back(move(indexXExpr));
            ctx.expressions.push_back(move(resultXExpr));
            ctx.expressions.push_back(move(assignXExpr));
            ctx.expressions.push_back(move(indexYExpr));
            ctx.expressions.push_back(move(resultYExpr));
            ctx.expressions.push_back(move(assignYExpr));
            ctx.expressions.push_back(move(indexZExpr));
            ctx.expressions.push_back(move(resultZExpr));
            ctx.expressions.push_back(move(assignZExpr));

        } else if (ins.type == InstructionType::DIVVF ||
                   ins.type == InstructionType::MULVF) {
            auto &right = ctx.stack.back();
            ctx.stack.pop_back();
            if (right.param->variableType != VariableType::Float) {
                throw ValidationException("Not a float on top of the stack");
            }

            auto &leftX = ctx.stack.back();
            ctx.stack.pop_back();
            auto &leftY = ctx.stack.back();
            ctx.stack.pop_back();
            auto &leftZ = ctx.stack.back();
            ctx.stack.pop_back();
            if ((leftX.param->variableType != VariableType::Float) ||
                (leftY.param->variableType != VariableType::Float) ||
                (leftZ.param->variableType != VariableType::Float)) {
                throw ValidationException("Not a vector on top of the stack");
            }

            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = VariableType::Vector;
            block->insert(resultExpr.get());

            auto type = (ins.type == InstructionType::DIVVF) ? ExpressionType::Divide : ExpressionType::Multiply;
            auto binaryExpr = make_shared<BinaryExpression>(type);
            binaryExpr->offset = ins.offset;
            binaryExpr->left = leftX.param;
            binaryExpr->right = right.param;

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = resultExpr.get();
            assignExpr->right = binaryExpr.get();
            block->insert(assignExpr.get());

            // X

            auto indexXExpr = make_shared<VectorIndexExpression>();
            indexXExpr->offset = ins.offset;
            indexXExpr->vector = resultExpr.get();
            indexXExpr->index = 0;

            auto resultXExpr = make_shared<ParameterExpression>();
            resultXExpr->offset = ins.offset;
            resultXExpr->variableType = VariableType::Float;
            resultXExpr->index = 0;
            block->insert(resultXExpr.get());

            auto assignXExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignXExpr->offset = ins.offset;
            assignXExpr->left = resultXExpr.get();
            assignXExpr->right = indexXExpr.get();
            block->insert(assignXExpr.get());

            // Y

            auto indexYExpr = make_shared<VectorIndexExpression>();
            indexYExpr->offset = ins.offset;
            indexYExpr->vector = resultExpr.get();
            indexYExpr->index = 1;

            auto resultYExpr = make_shared<ParameterExpression>();
            resultYExpr->offset = ins.offset;
            resultYExpr->variableType = VariableType::Float;
            resultYExpr->index = 1;
            block->insert(resultYExpr.get());

            auto assignYExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignYExpr->offset = ins.offset;
            assignYExpr->left = resultYExpr.get();
            assignYExpr->right = indexYExpr.get();
            block->insert(assignYExpr.get());

            // Z

            auto indexZExpr = make_shared<VectorIndexExpression>();
            indexZExpr->offset = ins.offset;
            indexZExpr->vector = resultExpr.get();
            indexZExpr->index = 2;

            auto resultZExpr = make_shared<ParameterExpression>();
            resultZExpr->offset = ins.offset;
            resultZExpr->variableType = VariableType::Float;
            resultZExpr->index = 2;
            block->insert(resultZExpr.get());

            auto assignZExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignZExpr->offset = ins.offset;
            assignZExpr->left = indexZExpr.get();
            assignZExpr->right = resultZExpr.get();
            block->insert(assignZExpr.get());

            //

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultZExpr.get()));
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultYExpr.get()));
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultXExpr.get()));
            ctx.expressions.push_back(move(resultExpr));
            ctx.expressions.push_back(move(binaryExpr));
            ctx.expressions.push_back(move(assignExpr));
            ctx.expressions.push_back(move(indexXExpr));
            ctx.expressions.push_back(move(resultXExpr));
            ctx.expressions.push_back(move(assignXExpr));
            ctx.expressions.push_back(move(indexYExpr));
            ctx.expressions.push_back(move(resultYExpr));
            ctx.expressions.push_back(move(assignYExpr));
            ctx.expressions.push_back(move(indexZExpr));
            ctx.expressions.push_back(move(resultZExpr));
            ctx.expressions.push_back(move(assignZExpr));

        } else if (ins.type == InstructionType::EQUALTT ||
                   ins.type == InstructionType::NEQUALTT) {
            auto numFrames = ins.size / 4;
            vector<StackFrame> rightFrames;
            for (int i = 0; i < numFrames; ++i) {
                rightFrames.push_back(ctx.stack.back());
                ctx.stack.pop_back();
            }
            vector<StackFrame> leftFrames;
            for (int i = 0; i < numFrames; ++i) {
                leftFrames.push_back(ctx.stack.back());
                ctx.stack.pop_back();
            }

            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = VariableType::Int;
            block->insert(resultExpr.get());

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
                block->insert(assignExpr.get());

                ctx.expressions.push_back(move(compExpr));
                ctx.expressions.push_back(move(andOrExpression));
                ctx.expressions.push_back(move(assignExpr));
            }

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultExpr.get()));
            ctx.expressions.push_back(move(resultExpr));

        } else if (ins.type == InstructionType::STORE_STATE) {
            auto innerCtx = DecompilationContext(ctx);
            ctx.savedAction = decompileSafely(ins.offset + 0x10, innerCtx);

        } else if (ins.type == InstructionType::SAVEBP) {
            ctx.prevNumGlobals = ctx.numGlobals;
            ctx.numGlobals = static_cast<int>(ctx.stack.size());
            for (int i = 0; i < ctx.numGlobals; ++i) {
                ctx.stack[i].param->locality = ParameterLocality::Global;
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
            auto stackSize = static_cast<int>(ctx.stack.size());
            auto frameIdx = ((ins.type == InstructionType::DECISP || ins.type == InstructionType::INCISP) ? stackSize : ctx.numGlobals) + (ins.stackOffset / 4);
            auto &frame = ctx.stack[frameIdx];

            ParameterExpression *destination;
            if (frame.allocatedBy != ctx.callStack.back().function) {
                int index = -1;
                for (size_t j = 0; j < ctx.outputs->size(); ++j) {
                    if ((*ctx.outputs)[j] == frame.param) {
                        index = j;
                    }
                }
                if (index == -1) {
                    index = ctx.outputs->size();
                    ctx.outputs->push_back(frame.param);
                }
                auto destExpr = make_shared<ParameterExpression>();
                destExpr->offset = ins.offset;
                destExpr->variableType = frame.param->variableType;
                destExpr->locality = ParameterLocality::Output;
                destExpr->index = index;
                destination = destExpr.get();
                ctx.expressions.push_back(move(destExpr));
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

            auto unaryExpr = make_shared<UnaryExpression>(type);
            unaryExpr->offset = ins.offset;
            unaryExpr->operand = destination;
            block->insert(unaryExpr.get());

            ctx.expressions.push_back(move(unaryExpr));

        } else if (ins.type == InstructionType::DESTRUCT) {
            auto numFrames = ins.size / 4;
            auto startNoDestroy = static_cast<int>(ctx.stack.size()) - numFrames + (ins.stackOffset / 4);
            auto numFramesNoDestroy = ins.sizeNoDestroy / 4;

            vector<StackFrame> framesNoDestroy;
            for (int i = 0; i < numFramesNoDestroy; ++i) {
                auto &frame = ctx.stack[startNoDestroy + i];
                framesNoDestroy.push_back(frame);
            }
            for (int i = 0; i < numFrames - numFramesNoDestroy; ++i) {
                ctx.stack.pop_back();
            }
            for (auto &frame : framesNoDestroy) {
                ctx.stack.push_back(frame);
            }

        } else {
            throw NotImplementedException("Cannot decompile expression of type " + to_string(static_cast<int>(ins.type)));
        }

        offset = ins.nextOffset;
    }

    for (size_t i = 0; i < block->expressions.size() - 1;) {
        if (block->expressions[i]->type != ExpressionType::Parameter ||
            block->expressions[i + 1]->type != ExpressionType::Assign) {
            i++;
            continue;
        }
        auto paramExpr = static_cast<ParameterExpression *>(block->expressions[i]);
        auto assignExpr = static_cast<BinaryExpression *>(block->expressions[i + 1]);
        if (assignExpr->left != paramExpr) {
            i++;
            continue;
        }
        assignExpr->declareLeft = true;
        for (size_t j = i; j < block->expressions.size() - 1; ++j) {
            block->expressions[j] = block->expressions[j + 1];
        }
        block->expressions.pop_back();
    }

    debug(boost::format("End decompiling block at %08x") % start);
    ctx.expressions.push_back(block);

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
        return move(constExpr);
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
        return move(paramExpr);
    }
    default:
        throw ArgumentException("Instruction is not of RSADDx type: " + to_string(static_cast<int>(ins.type)));
    }
}

} // namespace script

} // namespace reone
