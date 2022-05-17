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

#pragma once

#include "../expressiontree.h"

namespace reone {

class IOutputStream;
class TextWriter;

namespace script {

class IRoutines;

class NssWriter {
public:
    NssWriter(
        ExpressionTree &program,
        IRoutines &routines) :
        _program(program),
        _routines(routines) {
    }

    void save(IOutputStream &stream);

private:
    ExpressionTree &_program;
    IRoutines &_routines;

    void writeFunction(const ExpressionTree::Function &function, TextWriter &writer);
    void writeBlock(int level, const ExpressionTree::BlockExpression &block, TextWriter &writer);
    void writeExpression(int blockLevel, bool declare, const ExpressionTree::Expression &expression, TextWriter &writer);

    std::string indentAtLevel(int level);

    std::string describeFunction(const ExpressionTree::Function &function);
    std::string describeLabel(const ExpressionTree::LabelExpression &labelExpr);
    std::string describeConstant(const ExpressionTree::ConstantExpression &constExpr);
    std::string describeParameter(const ExpressionTree::ParameterExpression &paramExpr);
    std::string describeAction(const ExpressionTree::ActionExpression &actionExpr);
};

} // namespace script

} // namespace reone
