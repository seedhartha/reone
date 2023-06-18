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

#include "reone/tools/script/format/nsswriter.h"

#include "reone/script/routine.h"
#include "reone/script/routines.h"
#include "reone/script/variableutil.h"
#include "reone/system/exception/argument.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/exception/validation.h"
#include "reone/system/stream/bytearrayoutput.h"
#include "reone/system/textwriter.h"

using namespace std;

namespace reone {

namespace script {

void NssWriter::save(IOutputStream &stream) {
    auto writer = TextWriter(stream);

    if (!_program.globals().empty()) {
        auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
        auto ctx = WriteContext();
        for (auto &global : _program.globals()) {
            if (global.value.type != VariableType::Void) {
                writeExpression(0, true, global.param, ctx, writer);
                writer.put(" = ");
                writer.put(describeConstant(global.value));
            } else {
                writeExpression(0, true, global.param, ctx, writer);
            }
            writer.putLine(";");
        }
        writer.put("\n");
    }

    for (auto it = _program.functions().rbegin(); it != _program.functions().rend(); ++it) {
        writeFunction(**it, writer);
    }
}

void NssWriter::writeFunction(const Function &function, TextWriter &writer) {
    auto returnType = describeVariableType(function.returnType);
    auto name = describeFunction(function);
    auto params = vector<string>();
    for (auto &argument : function.arguments) {
        auto type = describeVariableType(argument.type);
        params.push_back(str(boost::format("%s arg_%d") % type % (-argument.stackOffset - 1)));
    }
    writer.putLine(str(boost::format("%s %s(%s)") % returnType % name % boost::join(params, ", ")));

    auto exprToVisit = stack<tuple<Expression *, int>>();
    exprToVisit.push(make_tuple(function.block, 0));
    auto visitedBlocks = set<BlockExpression *>();
    auto blocksToWrite = stack<tuple<BlockExpression *, int>>();

    while (!exprToVisit.empty()) {
        auto [expr, level] = exprToVisit.top();
        exprToVisit.pop();

        if (expr->type == ExpressionType::Block) {
            auto block = static_cast<BlockExpression *>(expr);
            if (visitedBlocks.count(block) > 0) {
                continue;
            }
            visitedBlocks.insert(block);
            for (auto it = block->expressions.rbegin(); it != block->expressions.rend(); ++it) {
                exprToVisit.push(make_tuple(*it, level + 1));
            }
            blocksToWrite.push(make_tuple(block, level));
        } else if (expr->type == ExpressionType::Return) {
            auto returnExpr = static_cast<ReturnExpression *>(expr);
            if (returnExpr->value) {
                exprToVisit.push(make_tuple(returnExpr->value, level));
            }
        } else if (expr->type == ExpressionType::Conditional) {
            auto conditionalExpr = static_cast<ConditionalExpression *>(expr);
            if (conditionalExpr->ifTrue) {
                exprToVisit.push(make_tuple(conditionalExpr->ifTrue, level));
            }
            if (conditionalExpr->test) {
                exprToVisit.push(make_tuple(conditionalExpr->test, level));
            }
        } else if (expr->type == ExpressionType::Action) {
            auto actionExpr = static_cast<ActionExpression *>(expr);
            for (auto argIter = actionExpr->arguments.rbegin(); argIter != actionExpr->arguments.rend(); ++argIter) {
                exprToVisit.push(make_tuple(*argIter, level));
            }
        } else if (ExpressionTree::isUnaryExpression(expr->type)) {
            auto unaryExpr = static_cast<UnaryExpression *>(expr);
        } else if (ExpressionTree::isBinaryExpression(expr->type)) {
            auto binaryExpr = static_cast<BinaryExpression *>(expr);
            exprToVisit.push(make_tuple(binaryExpr->left, level));
            exprToVisit.push(make_tuple(binaryExpr->right, level));
        }
    }

    auto ctx = WriteContext();

    while (!blocksToWrite.empty()) {
        auto [block, level] = blocksToWrite.top();
        blocksToWrite.pop();

        auto blockBytes = ByteArray();
        auto blockStream = ByteArrayOutputStream(blockBytes);
        auto blockWriter = TextWriter(blockStream);

        writeBlock(level, *block, ctx, blockWriter);

        ctx.writtenBlocks[block] = std::move(blockBytes);
    }

    if (ctx.writtenBlocks.count(function.block) > 0) {
        writer.put(ctx.writtenBlocks.at(function.block));
    }

    writer.putLine("");
    writer.putLine("");
}

void NssWriter::writeBlock(int level, const BlockExpression &block, WriteContext &ctx, TextWriter &writer) {
    auto innerLevel = 1 + level;
    auto indent = indentAtLevel(level);
    auto innerIndent = indentAtLevel(innerLevel);

    writer.putLine(indent + string("{"));
    for (auto &innerExpr : block.expressions) {
        if (innerExpr->type == ExpressionType::Parameter &&
            static_cast<const ParameterExpression *>(innerExpr)->locality == ParameterLocality::Global) {
            continue;
        }
        if (innerExpr->type == ExpressionType::Label) {
            writer.put(indent);
        } else {
            writer.put(innerIndent);
        }
        writeExpression(innerLevel, true, *innerExpr, ctx, writer);
        if (innerExpr->type != ExpressionType::Label &&
            innerExpr->type != ExpressionType::Conditional) {
            writer.put(";");
        }
        if (innerExpr->type != ExpressionType::Conditional) {
            writer.put("\n");
        }
    }
    writer.put(indent + string("}"));
}

void NssWriter::writeExpression(int blockLevel, bool declare, const Expression &expression, WriteContext &ctx, TextWriter &writer) {
    auto indent = indentAtLevel(blockLevel);

    if (expression.type == ExpressionType::Label) {
        auto &labelExpr = static_cast<const LabelExpression &>(expression);
        auto name = describeLabel(labelExpr);
        writer.put(name + ":");

    } else if (expression.type == ExpressionType::Goto) {
        auto &gotoExpr = static_cast<const GotoExpression &>(expression);
        auto name = describeLabel(*gotoExpr.label);
        writer.put("goto " + name);

    } else if (expression.type == ExpressionType::Return) {
        auto &returnExpr = static_cast<const ReturnExpression &>(expression);
        writer.put("return");
        if (returnExpr.value) {
            writer.put(" ");
            writeExpression(blockLevel, false, *returnExpr.value, ctx, writer);
        }

    } else if (expression.type == ExpressionType::Constant) {
        auto &constExpr = static_cast<const ConstantExpression &>(expression);
        auto value = describeConstant(constExpr);
        writer.put(value);

    } else if (expression.type == ExpressionType::Parameter) {
        auto &paramExpr = static_cast<const ParameterExpression &>(expression);
        auto name = describeParameter(paramExpr);
        if (declare) {
            auto type = describeVariableType(paramExpr.variableType);
            writer.put(str(boost::format("%s %s") % type % name));
        } else {
            writer.put(name);
        }

    } else if (expression.type == ExpressionType::Call) {
        auto &callExpr = static_cast<const CallExpression &>(expression);
        auto name = describeFunction(*callExpr.function);
        auto params = vector<string>();
        for (auto &param : callExpr.arguments) {
            auto name = describeParameter(*param);
            params.push_back(name);
        }
        writer.put(str(boost::format("%s(%s)") % name % boost::join(params, ", ")));

    } else if (expression.type == ExpressionType::Action) {
        auto &actionExpr = static_cast<const ActionExpression &>(expression);
        auto name = describeAction(actionExpr);
        writer.put(name + "(");
        for (size_t i = 0; i < actionExpr.arguments.size(); ++i) {
            if (i > 0) {
                writer.put(", ");
            }
            auto argExpr = actionExpr.arguments[i];
            if (argExpr->type == ExpressionType::Block) {
                auto blockArg = static_cast<BlockExpression *>(argExpr);
                writer.putLine("");
                if (ctx.writtenBlocks.count(blockArg) > 0) {
                    writer.put(ctx.writtenBlocks.at(blockArg));
                }
            } else {
                writeExpression(blockLevel, false, *argExpr, ctx, writer);
            }
        }
        writer.put(")");

    } else if (expression.type == ExpressionType::Negate ||
               expression.type == ExpressionType::OnesComplement ||
               expression.type == ExpressionType::Not ||
               expression.type == ExpressionType::Increment ||
               expression.type == ExpressionType::Decrement) {
        auto &unaryExpr = static_cast<const UnaryExpression &>(expression);
        auto name = describeParameter(*static_cast<ParameterExpression *>(unaryExpr.operand));
        if (expression.type == ExpressionType::Negate) {
            writer.put("-" + name);
        } else if (expression.type == ExpressionType::OnesComplement) {
            writer.put("~" + name);
        } else if (expression.type == ExpressionType::Not) {
            writer.put("!" + name);
        } else if (expression.type == ExpressionType::Increment) {
            writer.put(name + "++");
        } else if (expression.type == ExpressionType::Decrement) {
            writer.put(name + "--");
        }

    } else if (expression.type == ExpressionType::Assign ||
               expression.type == ExpressionType::Add ||
               expression.type == ExpressionType::Subtract ||
               expression.type == ExpressionType::Multiply ||
               expression.type == ExpressionType::Divide ||
               expression.type == ExpressionType::Modulo ||
               expression.type == ExpressionType::LogicalAnd ||
               expression.type == ExpressionType::LogicalOr ||
               expression.type == ExpressionType::BitwiseOr ||
               expression.type == ExpressionType::BitwiseExlusiveOr ||
               expression.type == ExpressionType::BitwiseAnd ||
               expression.type == ExpressionType::LeftShift ||
               expression.type == ExpressionType::RightShift ||
               expression.type == ExpressionType::Equal ||
               expression.type == ExpressionType::NotEqual ||
               expression.type == ExpressionType::GreaterThanOrEqual ||
               expression.type == ExpressionType::GreaterThan ||
               expression.type == ExpressionType::LessThan ||
               expression.type == ExpressionType::LessThanOrEqual) {
        auto &binaryExpr = static_cast<const BinaryExpression &>(expression);
        string operation;
        bool declareLeft = false;
        if (binaryExpr.type == ExpressionType::Assign) {
            operation = "=";
            declareLeft = binaryExpr.declareLeft;
        } else if (binaryExpr.type == ExpressionType::Add) {
            operation = "+";
        } else if (binaryExpr.type == ExpressionType::Subtract) {
            operation = "-";
        } else if (binaryExpr.type == ExpressionType::Multiply) {
            operation = "*";
        } else if (binaryExpr.type == ExpressionType::Divide) {
            operation = "/";
        } else if (binaryExpr.type == ExpressionType::Modulo) {
            operation = "%";
        } else if (binaryExpr.type == ExpressionType::LogicalAnd) {
            operation = "&&";
        } else if (binaryExpr.type == ExpressionType::LogicalOr) {
            operation = "||";
        } else if (binaryExpr.type == ExpressionType::BitwiseOr) {
            operation = "|";
        } else if (binaryExpr.type == ExpressionType::BitwiseExlusiveOr) {
            operation = "^";
        } else if (binaryExpr.type == ExpressionType::BitwiseAnd) {
            operation = "&";
        } else if (binaryExpr.type == ExpressionType::LeftShift) {
            operation = "<<";
        } else if (binaryExpr.type == ExpressionType::RightShift ||
                   binaryExpr.type == ExpressionType::RightShiftUnsigned) {
            operation = ">>";
        } else if (binaryExpr.type == ExpressionType::Equal) {
            operation = "==";
        } else if (binaryExpr.type == ExpressionType::NotEqual) {
            operation = "!=";
        } else if (binaryExpr.type == ExpressionType::GreaterThanOrEqual) {
            operation = ">=";
        } else if (binaryExpr.type == ExpressionType::GreaterThan) {
            operation = ">";
        } else if (binaryExpr.type == ExpressionType::LessThan) {
            operation = "<";
        } else if (binaryExpr.type == ExpressionType::LessThanOrEqual) {
            operation = "<=";
        }
        if (binaryExpr.type == ExpressionType::RightShiftUnsigned) {
            writer.put("(unsigned int)");
        }
        if (ExpressionTree::isBinaryExpression(binaryExpr.left->type)) {
            writer.put("(");
        }
        writeExpression(blockLevel, declareLeft, *binaryExpr.left, ctx, writer);
        if (ExpressionTree::isBinaryExpression(binaryExpr.left->type)) {
            writer.put(")");
        }
        writer.put(str(boost::format(" %s ") % operation));
        if (ExpressionTree::isBinaryExpression(binaryExpr.right->type)) {
            writer.put("(");
        }
        writeExpression(blockLevel, false, *binaryExpr.right, ctx, writer);
        if (ExpressionTree::isBinaryExpression(binaryExpr.right->type)) {
            writer.put(")");
        }

    } else if (expression.type == ExpressionType::Conditional) {
        auto &condExpr = static_cast<const ConditionalExpression &>(expression);
        writer.put("if(");
        writeExpression(blockLevel, false, *condExpr.test, ctx, writer);
        writer.putLine(")");
        if (ctx.writtenBlocks.count(condExpr.ifTrue) > 0) {
            writer.put(ctx.writtenBlocks.at(condExpr.ifTrue));
        }
        writer.putLine("");

    } else if (expression.type == ExpressionType::Vector) {
        auto &vecExpr = static_cast<const VectorExpression &>(expression);
        auto xComp = describeParameter(*vecExpr.components[0]);
        auto yComp = describeParameter(*vecExpr.components[1]);
        auto zComp = describeParameter(*vecExpr.components[2]);
        writer.put(str(boost::format("vector(%s, %s, %s)") % xComp % yComp % zComp));

    } else if (expression.type == ExpressionType::VectorIndex) {
        auto &indexExpr = static_cast<const VectorIndexExpression &>(expression);
        auto name = describeParameter(*indexExpr.vector);
        writer.put(str(boost::format("%s[%d]") % name % indexExpr.index));

    } else {
        throw NotImplementedException("Cannot write expression of type: " + to_string(static_cast<int>(expression.type)));
    }
}

string NssWriter::indentAtLevel(int level) {
    return string(4 * level, ' ');
}

string NssWriter::describeFunction(const Function &function) {
    return !function.name.empty() ? function.name : str(boost::format("fun_%08x") % function.offset);
}

string NssWriter::describeLabel(const LabelExpression &labelExpr) {
    return str(boost::format("loc_%08x") % labelExpr.offset);
}

string NssWriter::describeConstant(const ConstantExpression &constExpr) {
    return describeConstant(constExpr.value);
}

string NssWriter::describeConstant(const Variable &value) {
    if (value.type == VariableType::Int) {
        return to_string(value.intValue);
    } else if (value.type == VariableType::Float) {
        return str(boost::format("%f") % value.floatValue);
    } else if (value.type == VariableType::String) {
        return str(boost::format("\"%s\"") % value.strValue);
    } else if (value.type == VariableType::Object) {
        return to_string(value.objectId);
    } else {
        throw ArgumentException("Cannot describe constant expression of type: " + to_string(static_cast<int>(value.type)));
    }
}

string NssWriter::describeParameter(const ParameterExpression &paramExpr) {
    if (paramExpr.locality == ParameterLocality::Global) {
        return str(boost::format("glob_%08x") % paramExpr.offset);
    } else if (paramExpr.locality == ParameterLocality::Local || paramExpr.locality == ParameterLocality::ReturnValue) {
        if (!paramExpr.suffix.empty()) {
            return str(boost::format("var_%08x_%s") % paramExpr.offset % paramExpr.suffix);
        } else {
            return str(boost::format("var_%08x") % paramExpr.offset);
        }
    } else if (paramExpr.locality == ParameterLocality::Argument) {
        return str(boost::format("arg_%d") % (-paramExpr.stackOffset - 1));
    } else {
        throw ArgumentException("Unsupported parameter locality: " + to_string(static_cast<int>(paramExpr.locality)));
    }
}

string NssWriter::describeAction(const ActionExpression &actionExpr) {
    auto numRoutines = _routines.getNumRoutines();
    if (actionExpr.action >= numRoutines) {
        throw ArgumentException(str(boost::format("Action number out of bounds: %d/%d") % actionExpr.action % numRoutines));
    }
    return _routines.get(actionExpr.action).name();
}

} // namespace script

} // namespace reone
