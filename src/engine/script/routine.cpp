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

#include "routine.h"

#include "../common/log.h"
#include "../script/exception/invalidarg.h"
#include "../script/exception/invalidcaller.h"
#include "../script/exception/invalidtriggerrer.h"
#include "../script/exception/notimpl.h"

#include "variable.h"

using namespace std;

namespace reone {

namespace script {

Routine::Routine(
    string name,
    VariableType retType,
    vector<VariableType> argTypes,
    function<Variable(const vector<Variable> &, ExecutionContext &ctx)> fn
) :
    _name(move(name)),
    _returnType(retType),
    _argumentTypes(move(argTypes)),
    _func(move(fn)) {
}

Variable Routine::invoke(const vector<Variable> &args, ExecutionContext &ctx) const {
    if (_func) {
        try {
            return move(_func(args, ctx));
        }
        catch (const NotImplementedException &) {
            debug("Script: routine not implemented: " + _name, 2, DebugChannels::script);
        }
        catch (const InvalidArgumentException &ex) {
            debug(boost::format("Script: routine '%s' invocation failed: %s") % _name % ex.what(), 2, DebugChannels::script);
        }
        catch (const InvalidCallerException &ex) {
            debug(boost::format("Script: routine '%s' invocation failed: %s") % _name % ex.what(), 2, DebugChannels::script);
        }
        catch (const InvalidTriggerrerException &ex) {
            debug(boost::format("Script: routine '%s' invocation failed: %s") % _name % ex.what(), 2, DebugChannels::script);
        }
    }

    Variable result;
    result.type = _returnType;
    if (result.type == VariableType::Object) {
        result.objectId = kObjectInvalid;
    }

    return move(result);
}

int Routine::getArgumentCount() const {
    return static_cast<int>(_argumentTypes.size());
}

VariableType Routine::getArgumentType(int index) const {
    return _argumentTypes[index];
}

} // namespace script

} // namespace reone
