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

#pragma once

#include "../exprtree.h"

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
    struct WriteContext {
        std::map<std::pair<BlockExpression *, int>, std::string> writtenBlocks;
    };

    ExpressionTree &_program;
    IRoutines &_routines;

    void writeFunction(const Function &function, TextWriter &writer);
    void writeBlocks(const Function &function, TextWriter &writer);
    void writeBlock(int level, const BlockExpression &block, WriteContext &ctx, TextWriter &writer);
    void writeExpression(int blockLevel, bool declare, const Expression &expression, WriteContext &ctx, TextWriter &writer);

    std::string indentAtLevel(int level);

    std::string describeFunction(const Function &function);
    std::string describeLabel(const LabelExpression &labelExpr);
    std::string describeConstant(const ConstantExpression &constExpr);
    std::string describeParameter(const ParameterExpression &paramExpr);
    std::string describeAction(const ActionExpression &actionExpr);

    std::string describeConstant(const Variable &value);
};

} // namespace script

} // namespace reone
