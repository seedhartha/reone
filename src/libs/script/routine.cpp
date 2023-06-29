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

#include "reone/script/routine.h"

#include "reone/script/routine/exception/argument.h"
#include "reone/script/routine/exception/notimplemented.h"
#include "reone/script/variable.h"
#include "reone/system/logutil.h"

namespace reone {

namespace script {

Variable Routine::invoke(const std::vector<Variable> &args, ExecutionContext &ctx) {
    try {
        return _func(args, ctx);
    } catch (const RoutineNotImplementedException &ex) {
        std::string msg = "Routine not implemented: " + _name;
        return onException(msg, ex);
    } catch (const RoutineArgumentException &ex) {
        std::string msg = str(boost::format("Routine '%s' invocation failed: %s") % _name % ex.what());
        return onException(msg, ex);
    }
}

Variable Routine::onException(const std::string &msg, const std::exception &ex) const {
    switch (_returnType) {
    case VariableType::Action:
        error(msg, LogChannel::Script);
        throw ex;
    default:
        warn(msg, LogChannel::Script);
        return _defaultReturnValue;
    }
}

int Routine::getArgumentCount() const {
    return static_cast<int>(_argumentTypes.size());
}

VariableType Routine::getArgumentType(int index) const {
    return _argumentTypes[index];
}

} // namespace script

} // namespace reone
