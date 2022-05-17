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

#include "nsswriter.h"

#include "../../common/textwriter.h"
#include "../../common/exception/argument.h"
#include "../../common/exception/notimplemented.h"
#include "../../common/exception/validation.h"

#include "../routine.h"
#include "../routines.h"

using namespace std;

namespace reone {

namespace script {

void NssWriter::save(IOutputStream &stream) {
    auto writer = TextWriter(stream);

    if (!_program.globals().empty()) {
        for (auto &global : _program.globals()) {
            writeExpression(0, true, *global, writer);
            writer.putLine(";");
        }
        writer.put("\n");
    }

    auto writtenOffsets = set<uint32_t>();
    for (auto &function : _program.functions()) {
        if (writtenOffsets.count(function->offset) > 0) {
            continue;
        }
        writeFunction(*function, writer);
        writtenOffsets.insert(function->offset);
    }
}

void NssWriter::writeFunction(const ExpressionTree::Function &function, TextWriter &writer) {
    auto returnType = describeVariableType(function.returnType);
    auto name = describeFunction(function);
    auto params = vector<string>();
    int paramIdx;
    paramIdx = 0;
    for (auto &paramType : function.inArgumentTypes) {
        auto type = describeVariableType(paramType);
        params.push_back(str(boost::format("%s in_%d") % type % (paramIdx++)));
    }
    paramIdx = 0;
    for (auto &paramType : function.outArgumentTypes) {
        auto type = describeVariableType(paramType);
        params.push_back(str(boost::format("%s &out_%d") % type % (paramIdx++)));
    }
    writer.putLine(str(boost::format("%s %s(%s)") % returnType % name % boost::join(params, ", ")));

    writeBlock(0, *function.block, writer);

    writer.put("\n\n");
}

void NssWriter::writeBlock(int level, const ExpressionTree::BlockExpression &block, TextWriter &writer) {
    auto innerLevel = 1 + level;
    auto indent = indentAtLevel(level);
    auto innerIndent = indentAtLevel(innerLevel);

    writer.putLine(indent + string("{"));
    for (auto &innerExpr : block.expressions) {
        if (innerExpr->type == ExpressionTree::ExpressionType::Parameter &&
            static_cast<const ExpressionTree::ParameterExpression *>(innerExpr)->locality == ExpressionTree::ParameterLocality::Global) {
            continue;
        }
        if (innerExpr->type == ExpressionTree::ExpressionType::Label) {
            writer.put(indent);
        } else {
            writer.put(innerIndent);
        }
        writeExpression(innerLevel, true, *innerExpr, writer);
        if (innerExpr->type != ExpressionTree::ExpressionType::Label &&
            innerExpr->type != ExpressionTree::ExpressionType::Conditional) {
            writer.put(";");
        }
        if (innerExpr->type != ExpressionTree::ExpressionType::Conditional) {
            writer.put("\n");
        }
    }
    writer.put(indent + string("}"));
}

void NssWriter::writeExpression(int blockLevel, bool declare, const ExpressionTree::Expression &expression, TextWriter &writer) {
    auto indent = indentAtLevel(blockLevel);

    if (expression.type == ExpressionTree::ExpressionType::Label) {
        auto &labelExpr = static_cast<const ExpressionTree::LabelExpression &>(expression);
        auto name = describeLabel(labelExpr);
        writer.put(name + ":");

    } else if (expression.type == ExpressionTree::ExpressionType::Goto) {
        auto &gotoExpr = static_cast<const ExpressionTree::GotoExpression &>(expression);
        auto name = describeLabel(*gotoExpr.label);
        writer.put("goto " + name);

    } else if (expression.type == ExpressionTree::ExpressionType::Return) {
        auto &returnExpr = static_cast<const ExpressionTree::ReturnExpression &>(expression);
        writer.put("return");
        if (returnExpr.value) {
            writer.put(" ");
            writeExpression(blockLevel, false, *returnExpr.value, writer);
        }

    } else if (expression.type == ExpressionTree::ExpressionType::Constant) {
        auto &constExpr = static_cast<const ExpressionTree::ConstantExpression &>(expression);
        auto value = describeConstant(constExpr);
        writer.put(value);

    } else if (expression.type == ExpressionTree::ExpressionType::Parameter) {
        auto &paramExpr = static_cast<const ExpressionTree::ParameterExpression &>(expression);
        auto name = describeParameter(paramExpr);
        if (declare) {
            auto type = describeVariableType(paramExpr.variableType);
            writer.put(str(boost::format("%s %s") % type % name));
        } else {
            writer.put(name);
        }

    } else if (expression.type == ExpressionTree::ExpressionType::Call) {
        auto &callExpr = static_cast<const ExpressionTree::CallExpression &>(expression);
        auto name = describeFunction(*callExpr.function);
        auto params = vector<string>();
        for (auto &param : callExpr.arguments) {
            auto name = describeParameter(*param);
            params.push_back(name);
        }
        writer.put(str(boost::format("%s(%s)") % name % boost::join(params, ", ")));

    } else if (expression.type == ExpressionTree::ExpressionType::Action) {
        auto &actionExpr = static_cast<const ExpressionTree::ActionExpression &>(expression);
        auto name = describeAction(actionExpr);
        writer.put(name + "(");
        for (size_t i = 0; i < actionExpr.arguments.size(); ++i) {
            if (i > 0) {
                writer.put(", ");
            }
            auto argExpr = actionExpr.arguments[i];
            if (argExpr->type == ExpressionTree::ExpressionType::Parameter) {
                auto paramExpr = static_cast<ExpressionTree::ParameterExpression *>(argExpr);
                auto name = describeParameter(*paramExpr);
                writer.put(name);
            } else if (argExpr->type == ExpressionTree::ExpressionType::Block) {
                auto blockExpr = static_cast<ExpressionTree::BlockExpression *>(argExpr);
                writer.putLine("[&]()");
                writeBlock(blockLevel, *blockExpr, writer);
            } else {
                throw ValidationException("Action argument is neither parameter nor block expression");
            }
        }
        writer.put(")");

    } else if (expression.type == ExpressionTree::ExpressionType::Negate ||
               expression.type == ExpressionTree::ExpressionType::OnesComplement ||
               expression.type == ExpressionTree::ExpressionType::Not ||
               expression.type == ExpressionTree::ExpressionType::Increment ||
               expression.type == ExpressionTree::ExpressionType::Decrement) {
        auto &unaryExpr = static_cast<const ExpressionTree::UnaryExpression &>(expression);
        auto name = describeParameter(*unaryExpr.operand);
        if (expression.type == ExpressionTree::ExpressionType::Negate) {
            writer.put("-" + name);
        } else if (expression.type == ExpressionTree::ExpressionType::OnesComplement) {
            writer.put("~" + name);
        } else if (expression.type == ExpressionTree::ExpressionType::Not) {
            writer.put("!" + name);
        } else if (expression.type == ExpressionTree::ExpressionType::Increment) {
            writer.put(name + "++");
        } else if (expression.type == ExpressionTree::ExpressionType::Decrement) {
            writer.put(name + "--");
        }

    } else if (expression.type == ExpressionTree::ExpressionType::Assign ||
               expression.type == ExpressionTree::ExpressionType::Add ||
               expression.type == ExpressionTree::ExpressionType::Subtract ||
               expression.type == ExpressionTree::ExpressionType::Multiply ||
               expression.type == ExpressionTree::ExpressionType::Divide ||
               expression.type == ExpressionTree::ExpressionType::Modulo ||
               expression.type == ExpressionTree::ExpressionType::LogicalAnd ||
               expression.type == ExpressionTree::ExpressionType::LogicalOr ||
               expression.type == ExpressionTree::ExpressionType::BitwiseOr ||
               expression.type == ExpressionTree::ExpressionType::BitwiseExlusiveOr ||
               expression.type == ExpressionTree::ExpressionType::BitwiseAnd ||
               expression.type == ExpressionTree::ExpressionType::LeftShift ||
               expression.type == ExpressionTree::ExpressionType::RightShift ||
               expression.type == ExpressionTree::ExpressionType::Equal ||
               expression.type == ExpressionTree::ExpressionType::NotEqual ||
               expression.type == ExpressionTree::ExpressionType::GreaterThanOrEqual ||
               expression.type == ExpressionTree::ExpressionType::GreaterThan ||
               expression.type == ExpressionTree::ExpressionType::LessThan ||
               expression.type == ExpressionTree::ExpressionType::LessThanOrEqual) {
        auto &binaryExpr = static_cast<const ExpressionTree::BinaryExpression &>(expression);
        string operation;
        bool declareLeft = false;
        if (binaryExpr.type == ExpressionTree::ExpressionType::Assign) {
            operation = "=";
            declareLeft = binaryExpr.declareLeft;
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::Add) {
            operation = "+";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::Subtract) {
            operation = "-";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::Multiply) {
            operation = "*";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::Divide) {
            operation = "/";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::Modulo) {
            operation = "%";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::LogicalAnd) {
            operation = "&&";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::LogicalOr) {
            operation = "||";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::BitwiseOr) {
            operation = "|";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::BitwiseExlusiveOr) {
            operation = "^";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::BitwiseAnd) {
            operation = "&";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::LeftShift) {
            operation = "<<";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::RightShift ||
                   binaryExpr.type == ExpressionTree::ExpressionType::RightShiftUnsigned) {
            operation = ">>";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::Equal) {
            operation = "==";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::NotEqual) {
            operation = "!=";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::GreaterThanOrEqual) {
            operation = ">=";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::GreaterThan) {
            operation = ">";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::LessThan) {
            operation = "<";
        } else if (binaryExpr.type == ExpressionTree::ExpressionType::LessThanOrEqual) {
            operation = "<=";
        }
        if (binaryExpr.type == ExpressionTree::ExpressionType::RightShiftUnsigned) {
            writer.put("(unsigned int)");
        }
        writeExpression(blockLevel, declareLeft, *binaryExpr.left, writer);
        writer.put(str(boost::format(" %s ") % operation));
        writeExpression(blockLevel, false, *binaryExpr.right, writer);

    } else if (expression.type == ExpressionTree::ExpressionType::Conditional) {
        auto &condExpr = static_cast<const ExpressionTree::ConditionalExpression &>(expression);
        writer.put("if(");
        writeExpression(blockLevel, false, *condExpr.test, writer);
        writer.putLine(")");
        writeBlock(blockLevel, *condExpr.ifTrue, writer);
        writer.putLine("");
        if (condExpr.ifFalse) {
            writer.putLine(indent + "else");
            writeBlock(blockLevel, *condExpr.ifFalse, writer);
            writer.putLine("");
        }

    } else {
        throw NotImplementedException("Cannot write expression of type: " + to_string(static_cast<int>(expression.type)));
    }
}

string NssWriter::indentAtLevel(int level) {
    return string(4 * level, ' ');
}

string NssWriter::describeFunction(const ExpressionTree::Function &function) {
    return !function.name.empty() ? function.name : str(boost::format("fun_%08x") % function.offset);
}

string NssWriter::describeLabel(const ExpressionTree::LabelExpression &labelExpr) {
    return str(boost::format("loc_%08x") % labelExpr.offset);
}

string NssWriter::describeConstant(const ExpressionTree::ConstantExpression &constExpr) {
    if (constExpr.value.type == VariableType::Int) {
        return to_string(constExpr.value.intValue);
    } else if (constExpr.value.type == VariableType::Float) {
        return str(boost::format("%ff") % constExpr.value.floatValue);
    } else if (constExpr.value.type == VariableType::String) {
        return str(boost::format("\"%s\"") % constExpr.value.strValue);
    } else if (constExpr.value.type == VariableType::Object) {
        return to_string(constExpr.value.objectId);
    } else {
        throw ArgumentException("Cannot describe constant expression of type: " + to_string(static_cast<int>(constExpr.value.type)));
    }
}

string NssWriter::describeParameter(const ExpressionTree::ParameterExpression &paramExpr) {
    if (paramExpr.locality == ExpressionTree::ParameterLocality::Local) {
        if (paramExpr.index > 0) {
            return str(boost::format("var_%08x_%d") % paramExpr.offset % paramExpr.index);
        } else {
            return str(boost::format("var_%08x") % paramExpr.offset);
        }
    } else if (paramExpr.locality == ExpressionTree::ParameterLocality::Input) {
        return str(boost::format("in_%d") % paramExpr.index);
    } else if (paramExpr.locality == ExpressionTree::ParameterLocality::Output) {
        return str(boost::format("out_%d") % paramExpr.index);
    } else if (paramExpr.locality == ExpressionTree::ParameterLocality::Global) {
        return str(boost::format("glob_%08x") % paramExpr.offset);
    } else {
        throw ArgumentException("Unsupported parameter locality: " + to_string(static_cast<int>(paramExpr.locality)));
    }
}

string NssWriter::describeAction(const ExpressionTree::ActionExpression &actionExpr) {
    auto numRoutines = _routines.getNumRoutines();
    if (actionExpr.action >= numRoutines) {
        throw ArgumentException(str(boost::format("Action number out of bounds: %d/%d") % actionExpr.action % numRoutines));
    }
    return _routines.get(actionExpr.action).name();
}

string NssWriter::describeVariableType(VariableType type) {
    switch (type) {
    case VariableType::Void:
        return "void";
    case VariableType::Int:
        return "int";
    case VariableType::Float:
        return "float";
    case VariableType::String:
        return "string";
    case VariableType::Vector:
        return "vector";
    case VariableType::Object:
        return "object";
    case VariableType::Effect:
        return "effect";
    case VariableType::Event:
        return "event";
    case VariableType::Location:
        return "location";
    case VariableType::Talent:
        return "talent";
    case VariableType::Action:
        return "action";
    default:
        throw ArgumentException("Cannot describe variable type: " + to_string(static_cast<int>(type)));
    }
}

} // namespace script

} // namespace reone
