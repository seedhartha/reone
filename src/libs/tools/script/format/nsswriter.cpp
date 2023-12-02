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
#include "reone/system/exception/notimplemented.h"
#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/memoryoutput.h"
#include "reone/system/textwriter.h"

namespace reone {

namespace script {

void NssWriter::save(IOutputStream &stream) {
    auto writer = TextWriter(stream);

    if (!_program.globals().empty()) {
        auto ctx = WriteContext();
        for (auto &global : _program.globals()) {
            if (global.value.type != VariableType::Void) {
                writeExpression(0, true, *global.param, ctx, writer);
                writer.write(" = ");
                writer.write(describeConstant(global.value));
            } else {
                writeExpression(0, true, *global.param, ctx, writer);
            }
            writer.writeLine(";");
        }
        writer.write("\n");
    }

    for (auto it = _program.functions().rbegin(); it != _program.functions().rend(); ++it) {
        writeFunction(**it, writer);
    }
}

void NssWriter::writeFunction(const Function &function, TextWriter &writer) {
    auto returnType = describeVariableType(function.returnType);
    auto name = describeFunction(function);
    auto params = std::vector<std::string>();
    for (auto &argument : function.arguments) {
        auto type = describeVariableType(argument.type);
        if (argument.pointer) {
            params.push_back(str(boost::format("%s *arg_%08x") % type % argument.stackOffset));
        } else {
            params.push_back(str(boost::format("%s arg_%08x") % type % argument.stackOffset));
        }
    }
    writer.writeLine(str(boost::format("%s %s(%s)") % returnType % name % boost::join(params, ", ")));
    writeBlocks(function, writer);
    writer.write("\n\n");
}

struct BlockLevelCompare {
    bool operator()(const std::pair<BlockExpression *, int> &a, const std::pair<BlockExpression *, int> &b) const {
        if (a.second > b.second) {
            return true;
        }
        if (a.second < b.second) {
            return false;
        }
        if (a.first->offset > b.first->offset) {
            return true;
        }
        if (a.first->offset < b.first->offset) {
            return false;
        }
        return a.first < b.first;
    }
};

void NssWriter::writeBlocks(const Function &func, TextWriter &writer) {
    debug(boost::format("Writing blocks of function at %08x") % func.block->offset);
    std::set<std::pair<BlockExpression *, int>, BlockLevelCompare> blocksToWrite;

    std::queue<std::pair<Expression *, int>> exprToVisit;
    exprToVisit.push(std::make_pair(func.block, 0));
    while (!exprToVisit.empty()) {
        auto [expr, level] = exprToVisit.front();
        exprToVisit.pop();
        if (expr->type == ExpressionType::Block) {
            auto blockExpr = static_cast<BlockExpression *>(expr);
            debug(boost::format("Visiting block (%p, %d) at %08x") % blockExpr % level % blockExpr->offset);
            auto blockKey = std::make_pair(blockExpr, level);
            if (blocksToWrite.count(blockKey) == 0) {
                blocksToWrite.insert(blockKey);
                for (auto nestedExpr : blockExpr->expressions) {
                    exprToVisit.push(std::make_pair(nestedExpr, level + 1));
                }
            } else {
                debug("Ignoring already written block");
            }
        } else if (expr->type == ExpressionType::Return) {
            auto returnExpr = static_cast<ReturnExpression *>(expr);
            if (returnExpr->value) {
                exprToVisit.push(std::make_pair(returnExpr->value, level));
            }
        } else if (expr->type == ExpressionType::Conditional) {
            auto conditionalExpr = static_cast<ConditionalExpression *>(expr);
            exprToVisit.push(std::make_pair(conditionalExpr->test, level));
            if (conditionalExpr->ifTrue) {
                exprToVisit.push(std::make_pair(conditionalExpr->ifTrue, level));
            }
        } else if (expr->type == ExpressionType::Action) {
            auto actionExpr = static_cast<ActionExpression *>(expr);
            for (auto &arg : actionExpr->arguments) {
                exprToVisit.push(std::make_pair(arg, level));
            }
        } else if (expr->type == ExpressionType::Call) {
            auto callExpr = static_cast<CallExpression *>(expr);
            for (auto &arg : callExpr->arguments) {
                exprToVisit.push(std::make_pair(arg, level));
            }
        } else if (ExpressionTree::isUnaryExpression(expr->type)) {
            auto unaryExpr = static_cast<UnaryExpression *>(expr);
            exprToVisit.push(std::make_pair(unaryExpr->operand, level));
        } else if (ExpressionTree::isBinaryExpression(expr->type)) {
            auto binaryExpr = static_cast<BinaryExpression *>(expr);
            exprToVisit.push(std::make_pair(binaryExpr->left, level));
            exprToVisit.push(std::make_pair(binaryExpr->right, level));
        }
    }

    auto blockBytes = ByteBuffer();
    auto blockStream = MemoryOutputStream(blockBytes);
    auto blockWriter = TextWriter(blockStream);
    auto ctx = WriteContext();
    for (auto [block, level] : blocksToWrite) {
        debug(boost::format("Writing block (%p, %d)") % block % level);
        blockBytes.clear();
        writeBlock(level, *block, ctx, blockWriter);
        auto blockKey = std::make_pair(block, level);
        std::string blockString;
        blockString.reserve(blockBytes.size());
        blockString.insert(blockString.begin(), blockBytes.begin(), blockBytes.end());
        ctx.writtenBlocks[blockKey] = std::move(blockString);
    }
    auto &rootBlock = ctx.writtenBlocks.at(std::make_pair(func.block, 0));
    writer.write(rootBlock);
}

void NssWriter::writeBlock(int level, const BlockExpression &block, WriteContext &ctx, TextWriter &writer) {
    auto innerLevel = 1 + level;
    auto indent = indentAtLevel(level);
    auto innerIndent = indentAtLevel(innerLevel);

    writer.writeLine(indent + std::string("{"));
    for (auto &innerExpr : block.expressions) {
        if (innerExpr->type == ExpressionType::Label) {
            writer.write(indent);
        } else {
            writer.write(innerIndent);
        }
        writeExpression(innerLevel, true, *innerExpr, ctx, writer);
        if (innerExpr->type != ExpressionType::Label &&
            innerExpr->type != ExpressionType::Conditional) {
            writer.write(";");
        }
        if (innerExpr->type != ExpressionType::Conditional) {
            writer.write("\n");
        }
    }
    writer.write(indent + std::string("}"));
}

void NssWriter::writeExpression(int blockLevel, bool declare, const Expression &expression, WriteContext &ctx, TextWriter &writer) {
    auto indent = indentAtLevel(blockLevel);

    if (expression.type == ExpressionType::Label) {
        auto &labelExpr = static_cast<const LabelExpression &>(expression);
        auto name = describeLabel(labelExpr);
        writer.write(name + ":");

    } else if (expression.type == ExpressionType::Goto) {
        auto &gotoExpr = static_cast<const GotoExpression &>(expression);
        auto name = describeLabel(*gotoExpr.label);
        writer.write("goto " + name);

    } else if (expression.type == ExpressionType::Return) {
        auto &returnExpr = static_cast<const ReturnExpression &>(expression);
        writer.write("return");
        if (returnExpr.value) {
            writer.write(" ");
            writeExpression(blockLevel, false, *returnExpr.value, ctx, writer);
        }

    } else if (expression.type == ExpressionType::Constant) {
        auto &constExpr = static_cast<const ConstantExpression &>(expression);
        auto value = describeConstant(constExpr);
        writer.write(value);

    } else if (expression.type == ExpressionType::Parameter) {
        auto &paramExpr = static_cast<const ParameterExpression &>(expression);
        auto name = describeParameter(paramExpr);
        if (declare) {
            auto type = describeVariableType(paramExpr.variableType);
            writer.write(str(boost::format("%s %s") % type % name));
        } else {
            writer.write(name);
        }

    } else if (expression.type == ExpressionType::Call) {
        auto &callExpr = static_cast<const CallExpression &>(expression);
        auto name = describeFunction(*callExpr.function);
        writer.write(name + "(");
        for (size_t i = 0; i < callExpr.arguments.size(); ++i) {
            if (i > 0) {
                writer.write(", ");
            }
            auto argExpr = callExpr.arguments[i];
            if (callExpr.function->arguments[i].pointer) {
                writer.write("&");
            }
            writeExpression(blockLevel, false, *argExpr, ctx, writer);
        }
        writer.write(")");

    } else if (expression.type == ExpressionType::Action) {
        auto &actionExpr = static_cast<const ActionExpression &>(expression);
        auto name = describeAction(actionExpr);
        writer.write(name + "(");
        for (size_t i = 0; i < actionExpr.arguments.size(); ++i) {
            if (i > 0) {
                writer.write(", ");
            }
            auto argExpr = actionExpr.arguments[i];
            if (argExpr->type == ExpressionType::Block) {
                auto blockArg = static_cast<BlockExpression *>(argExpr);
                writer.writeLine("");
                auto blockKey = std::make_pair(blockArg, blockLevel);
                if (ctx.writtenBlocks.count(blockKey) > 0) {
                    writer.write(ctx.writtenBlocks.at(blockKey));
                } else {
                    warn(boost::format("Block (%p, %d) not written") % blockArg % blockLevel);
                }
            } else {
                writeExpression(blockLevel, false, *argExpr, ctx, writer);
            }
        }
        writer.write(")");

    } else if (expression.type == ExpressionType::Negate ||
               expression.type == ExpressionType::OnesComplement ||
               expression.type == ExpressionType::Not ||
               expression.type == ExpressionType::Increment ||
               expression.type == ExpressionType::Decrement) {
        auto &unaryExpr = static_cast<const UnaryExpression &>(expression);
        std::string prefix, suffix;
        if (expression.type == ExpressionType::Negate) {
            prefix = "-";
        } else if (expression.type == ExpressionType::OnesComplement) {
            prefix = "~";
        } else if (expression.type == ExpressionType::Not) {
            prefix = "!";
        } else if (expression.type == ExpressionType::Increment) {
            suffix = "++";
        } else if (expression.type == ExpressionType::Decrement) {
            suffix = "--";
        }
        if (!prefix.empty()) {
            writer.write(prefix);
        }
        if (ExpressionTree::isBinaryExpression(unaryExpr.operand->type)) {
            writer.write("(");
        }
        writeExpression(blockLevel, false, *unaryExpr.operand, ctx, writer);
        if (ExpressionTree::isBinaryExpression(unaryExpr.operand->type)) {
            writer.write(")");
        }
        if (!suffix.empty()) {
            writer.write(suffix);
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
        std::string operation;
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
            writer.write("(unsigned int)");
        }
        if (ExpressionTree::isBinaryExpression(binaryExpr.left->type)) {
            writer.write("(");
        }
        writeExpression(blockLevel, declareLeft, *binaryExpr.left, ctx, writer);
        if (ExpressionTree::isBinaryExpression(binaryExpr.left->type)) {
            writer.write(")");
        }
        writer.write(str(boost::format(" %s ") % operation));
        if (ExpressionTree::isBinaryExpression(binaryExpr.right->type)) {
            writer.write("(");
        }
        writeExpression(blockLevel, false, *binaryExpr.right, ctx, writer);
        if (ExpressionTree::isBinaryExpression(binaryExpr.right->type)) {
            writer.write(")");
        }

    } else if (expression.type == ExpressionType::Conditional) {
        auto &condExpr = static_cast<const ConditionalExpression &>(expression);
        writer.write("if(");
        writeExpression(blockLevel, false, *condExpr.test, ctx, writer);
        writer.writeLine(")");
        if (condExpr.ifTrue) {
            auto blockKey = std::make_pair(condExpr.ifTrue, blockLevel);
            if (ctx.writtenBlocks.count(blockKey) > 0) {
                writer.write(ctx.writtenBlocks.at(blockKey));
            } else {
                warn(boost::format("Block (%p, %d) not written") % condExpr.ifTrue % blockLevel);
            }
        }
        writer.writeLine("");

    } else if (expression.type == ExpressionType::Vector) {
        auto &vecExpr = static_cast<const VectorExpression &>(expression);
        auto xComp = describeParameter(*vecExpr.components[0]);
        auto yComp = describeParameter(*vecExpr.components[1]);
        auto zComp = describeParameter(*vecExpr.components[2]);
        writer.write(str(boost::format("vector(%s, %s, %s)") % xComp % yComp % zComp));

    } else if (expression.type == ExpressionType::VectorIndex) {
        auto &indexExpr = static_cast<const VectorIndexExpression &>(expression);
        auto name = describeParameter(*indexExpr.vector);
        writer.write(str(boost::format("%s[%d]") % name % indexExpr.index));

    } else {
        throw NotImplementedException("Cannot write expression of type: " + std::to_string(static_cast<int>(expression.type)));
    }
}

std::string NssWriter::indentAtLevel(int level) {
    return std::string(4 * level, ' ');
}

std::string NssWriter::describeFunction(const Function &function) {
    return !function.name.empty() ? function.name : str(boost::format("fun_%08x") % function.start);
}

std::string NssWriter::describeLabel(const LabelExpression &labelExpr) {
    return str(boost::format("loc_%08x") % labelExpr.offset);
}

std::string NssWriter::describeConstant(const ConstantExpression &constExpr) {
    return describeConstant(constExpr.value);
}

std::string NssWriter::describeConstant(const Variable &value) {
    if (value.type == VariableType::Int) {
        return std::to_string(value.intValue);
    } else if (value.type == VariableType::Float) {
        return str(boost::format("%f") % value.floatValue);
    } else if (value.type == VariableType::String) {
        return str(boost::format("\"%s\"") % value.strValue);
    } else if (value.type == VariableType::Object) {
        return std::to_string(value.objectId);
    } else {
        throw std::invalid_argument("Cannot describe constant expression of type: " + std::to_string(static_cast<int>(value.type)));
    }
}

std::string NssWriter::describeParameter(const ParameterExpression &paramExpr) {
    if (paramExpr.locality == ParameterLocality::Global) {
        return str(boost::format("glob_%08x") % paramExpr.offset);
    } else if (paramExpr.locality == ParameterLocality::Local) {
        if (!paramExpr.suffix.empty()) {
            return str(boost::format("var_%08x_%s") % paramExpr.offset % paramExpr.suffix);
        } else {
            return str(boost::format("var_%08x") % paramExpr.offset);
        }
    } else if (paramExpr.locality == ParameterLocality::Argument) {
        if (paramExpr.outerModified) {
            return str(boost::format("*arg_%08x") % paramExpr.outerStackOffset);
        } else {
            return str(boost::format("arg_%08x") % paramExpr.outerStackOffset);
        }
    } else if (paramExpr.locality == ParameterLocality::ReturnValue) {
        return str(boost::format("ret_%08x") % paramExpr.outerStackOffset);
    } else {
        throw std::invalid_argument("Unsupported parameter locality: " + std::to_string(static_cast<int>(paramExpr.locality)));
    }
}

std::string NssWriter::describeAction(const ActionExpression &actionExpr) {
    auto numRoutines = _routines.getNumRoutines();
    if (actionExpr.action >= numRoutines) {
        throw std::invalid_argument(str(boost::format("Action number out of bounds: %d/%d") % actionExpr.action % numRoutines));
    }
    return _routines.get(actionExpr.action).name();
}

} // namespace script

} // namespace reone
