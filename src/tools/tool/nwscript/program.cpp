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

#include "program.h"

#include "../../../common/exception/argument.h"
#include "../../../common/exception/notimplemented.h"
#include "../../../common/exception/validation.h"
#include "../../../script/routine.h"
#include "../../../script/routines.h"

using namespace std;

using namespace reone::script;

namespace reone {

NwscriptProgram NwscriptProgram::fromCompiled(const ScriptProgram &compiled, const IRoutines &routines) {
    auto funcMain = make_shared<Function>();
    funcMain->name = "main";
    funcMain->offset = 13;

    auto functions = vector<shared_ptr<Function>>();
    auto expressions = vector<shared_ptr<Expression>>();

    auto labels = unordered_map<uint32_t, LabelExpression *>();
    for (auto &ins : compiled.instructions()) {
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

    auto ctx = DecompilationContext(compiled, routines, labels, functions, expressions);

    ctx.callStack.push_back(CallStackFrame {funcMain.get()});

    funcMain->block = decompile(13, ctx);

    ctx.functions.push_back(move(funcMain));

    return NwscriptProgram(
        ctx.functions,
        ctx.expressions);
}

NwscriptProgram::BlockExpression *NwscriptProgram::decompile(uint32_t start, DecompilationContext &ctx) {
    auto block = make_shared<BlockExpression>();
    block->offset = start;

    for (uint32_t offset = start; offset < ctx.compiled.length();) {
        auto maybeLabel = ctx.labels.find(offset);
        if (maybeLabel != ctx.labels.end()) {
            block->expressions.push_back(maybeLabel->second);
        }

        auto &ins = ctx.compiled.getInstruction(offset);

        if (ins.type == InstructionType::RETN) {
            auto retExpr = make_shared<ReturnExpression>();
            if (ctx.callStack.size() == 1ll && !ctx.stack.empty()) {
                auto retVal = ctx.stack.back().param;
                retExpr->value = retVal;
                auto funcMain = ctx.callStack.back().function;
                funcMain->returnType = retVal->variableType;
            }
            block->expressions.push_back(retExpr.get());
            ctx.expressions.push_back(move(retExpr));
            break;

        } else if (ins.type == InstructionType::JMP) {
            auto gotoExpr = make_shared<GotoExpression>();
            gotoExpr->offset = ins.offset;
            gotoExpr->label = ctx.labels.at(ins.offset + ins.jumpOffset);
            offset = ins.offset + ins.jumpOffset;

        } else if (ins.type == InstructionType::JSR) {
            if (ins.jumpOffset < 0) {
                throw NotImplementedException("Negative jump offsets are not supported yet");
            }
            auto sub = make_shared<Function>();
            sub->offset = ins.offset + ins.jumpOffset;

            auto subCtx = DecompilationContext(ctx);
            subCtx.callStack.push_back(CallStackFrame {sub.get()});
            sub->block = decompile(ins.offset + ins.jumpOffset, subCtx);

            auto callExpr = make_shared<CallExpression>();
            callExpr->offset = ins.offset;
            callExpr->function = sub.get();
            block->expressions.push_back(callExpr.get());

            for (auto &input : subCtx.callStack.back().inputs) {
                sub->inArgumentTypes.push_back(input->variableType);
                callExpr->arguments.push_back(input);
            }
            for (auto &output : subCtx.callStack.back().outputs) {
                sub->outArgumentTypes.push_back(output->variableType);
                callExpr->arguments.push_back(output);
            }
            subCtx.callStack.pop_back();

            ctx.functions.push_back(move(sub));
            ctx.expressions.push_back(move(callExpr));

        } else if (ins.type == InstructionType::JZ ||
                   ins.type == InstructionType::JNZ) {
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
            ifTrueGotoExpr->label = ctx.labels.at(ins.offset + ins.jumpOffset);

            auto ifTrueBlockExpr = make_shared<BlockExpression>();
            ifTrueBlockExpr->offset = ins.offset;
            ifTrueBlockExpr->expressions.push_back(ifTrueGotoExpr.get());

            auto condExpr = make_shared<ConditionalExpression>();
            condExpr->test = testExpr.get();
            condExpr->ifTrue = ifTrueBlockExpr.get();
            block->expressions.push_back(condExpr.get());

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
            block->expressions.push_back(expression.get());
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, expression.get(), 0));
            ctx.expressions.push_back(move(expression));

        } else if (ins.type == InstructionType::CONSTI ||
                   ins.type == InstructionType::CONSTF ||
                   ins.type == InstructionType::CONSTS ||
                   ins.type == InstructionType::CONSTO) {
            auto constExpr = constantExpression(ins);

            auto paramExpr = make_shared<ParameterExpression>();
            paramExpr->offset = ins.offset;
            paramExpr->variableType = constExpr->value.type;
            block->expressions.push_back(paramExpr.get());

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = paramExpr.get();
            assignExpr->right = constExpr.get();
            block->expressions.push_back(assignExpr.get());

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, paramExpr.get(), 0));
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
                    auto xParam = ctx.stack.back();
                    ctx.stack.pop_back();
                    auto yParam = ctx.stack.back();
                    ctx.stack.pop_back();
                    auto zParam = ctx.stack.back();
                    ctx.stack.pop_back();
                    if ((xParam.param->type != ExpressionType::Parameter || xParam.component != 0) ||
                        (yParam.param->type != ExpressionType::Parameter || yParam.component != 1) ||
                        (zParam.param->type != ExpressionType::Parameter || zParam.component != 2)) {
                        throw ValidationException("Not a vector on top of the stack");
                    }
                    argument = xParam.param;
                } else if (argType == VariableType::Action) {
                    argument = ctx.stack.back().block;
                    ctx.stack.pop_back();
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
                block->expressions.push_back(retValExpr.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = retValExpr.get();
                assignExpr->right = actionExpr.get();
                block->expressions.push_back(assignExpr.get());

                if (routine.returnType() == VariableType::Vector) {
                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValExpr.get(), 2));
                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValExpr.get(), 1));
                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValExpr.get(), 0));
                } else {
                    ctx.stack.push_back(StackFrame(ctx.callStack.back().function, retValExpr.get(), 0));
                }
                ctx.expressions.push_back(move(retValExpr));
                ctx.expressions.push_back(move(assignExpr));

            } else {
                block->expressions.push_back(actionExpr.get());
            }

            ctx.expressions.push_back(move(actionExpr));

        } else if (ins.type == InstructionType::CPDOWNSP ||
                   ins.type == InstructionType::CPDOWNBP) {
            auto stackSize = static_cast<int>(ctx.stack.size());
            if (ins.stackOffset >= 0) {
                throw ValidationException("Non-negative stack offsets are not supported");
            }
            auto startIdx = (ins.type == InstructionType::CPDOWNSP ? stackSize : ctx.numGlobals) + (ins.stackOffset / 4);
            auto numFrames = ins.size / 4;
            for (int i = 0; i < numFrames; ++i) {
                auto &left = ctx.stack[startIdx + numFrames - i - 1];
                auto &right = ctx.stack[stackSize - i - 1];

                ParameterExpression *destination;
                auto &csFrame = ctx.callStack.back();
                if (left.allocatedBy != csFrame.function) {
                    auto destExpr = make_shared<ParameterExpression>();
                    destExpr->offset = ins.offset;
                    destExpr->variableType = left.param->variableType;
                    destExpr->locality = ParameterLocality::Output;
                    destExpr->index = csFrame.outputs.size();
                    csFrame.outputs.push_back(left.param);
                    destination = destExpr.get();
                    ctx.expressions.push_back(move(destExpr));
                } else {
                    destination = left.param;
                }

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = destination;
                assignExpr->right = right.param;
                block->expressions.push_back(assignExpr.get());

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
            auto numFrames = ins.size / 4;
            for (int i = 0; i < numFrames; ++i) {
                auto &frame = ctx.stack[startIdx + numFrames - i - 1];

                ParameterExpression *source;
                auto &csFrame = ctx.callStack.back();
                if (frame.allocatedBy != csFrame.function) {
                    auto sourceExpr = make_shared<ParameterExpression>();
                    sourceExpr->offset = ins.offset;
                    sourceExpr->variableType = frame.param->variableType;
                    sourceExpr->locality = ParameterLocality::Input;
                    sourceExpr->index = csFrame.inputs.size();
                    csFrame.inputs.push_back(frame.param);
                    source = sourceExpr.get();
                    ctx.expressions.push_back(move(sourceExpr));
                } else {
                    source = frame.param;
                }

                auto paramExpr = make_shared<ParameterExpression>();
                paramExpr->offset = ins.offset;
                paramExpr->variableType = source->variableType;
                paramExpr->index = i;
                block->expressions.push_back(paramExpr.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = paramExpr.get();
                assignExpr->right = source;
                block->expressions.push_back(assignExpr.get());

                auto frameCopy = StackFrame(frame);
                frameCopy.allocatedBy = ctx.callStack.back().function;
                frameCopy.param = paramExpr.get();
                frameCopy.component = frame.component;
                ctx.stack.push_back(move(frameCopy));

                ctx.expressions.push_back(move(paramExpr));
                ctx.expressions.push_back(move(assignExpr));
            }

        } else if (ins.type == InstructionType::MOVSP) {
            if (ins.stackOffset >= 0) {
                throw NotImplementedException("Non-negative stack offsets are not supported yet");
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
            block->expressions.push_back(resultExpr.get());

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
            block->expressions.push_back(assignExpr.get());

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultExpr.get(), 0));
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
                   ins.type == InstructionType::SHRIGHTII) {
            auto &left = ctx.stack.back();
            ctx.stack.pop_back();
            auto &right = ctx.stack.back();
            ctx.stack.pop_back();

            auto resultExpr = make_shared<ParameterExpression>();
            resultExpr->offset = ins.offset;
            resultExpr->variableType = VariableType::Int;
            block->expressions.push_back(resultExpr.get());

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
            }
            auto binaryExpr = make_shared<BinaryExpression>(type);
            binaryExpr->offset = ins.offset;
            binaryExpr->left = left.param;
            binaryExpr->right = right.param;

            auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
            assignExpr->offset = ins.offset;
            assignExpr->left = resultExpr.get();
            assignExpr->right = binaryExpr.get();
            block->expressions.push_back(assignExpr.get());

            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, resultExpr.get(), 0));
            ctx.expressions.push_back(move(resultExpr));
            ctx.expressions.push_back(move(binaryExpr));
            ctx.expressions.push_back(move(assignExpr));

        } else if (ins.type == InstructionType::STORE_STATE) {
            auto innerCtx = DecompilationContext(ctx);
            auto innerBlock = decompile(ins.offset + 0x10, innerCtx);
            ctx.stack.push_back(StackFrame(ctx.callStack.back().function, innerBlock));

        } else if (ins.type == InstructionType::SAVEBP) {
            ctx.prevNumGlobals = ctx.numGlobals;
            ctx.numGlobals = static_cast<int>(ctx.stack.size());

        } else if (ins.type == InstructionType::RESTOREBP) {
            ctx.numGlobals = ctx.prevNumGlobals;

        } else {
            throw NotImplementedException("Cannot decompile expression of type " + to_string(static_cast<int>(ins.type)));
        }

        offset = ins.nextOffset;
    }

    ctx.expressions.push_back(block);

    return block.get();
}

unique_ptr<NwscriptProgram::ConstantExpression> NwscriptProgram::constantExpression(const Instruction &ins) {
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

unique_ptr<NwscriptProgram::ParameterExpression> NwscriptProgram::parameterExpression(const Instruction &ins) {
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

} // namespace reone