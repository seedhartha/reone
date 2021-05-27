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

/** @file
 *  Implementation of routines related to the Event engine type.
 */

#include "routines.h"

#include "../../common/log.h"

#include "../enginetype/event.h"
#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::eventUserDefined(const VariablesList &args, ExecutionContext &ctx) {
    int eventNumber = getInt(args, 0);
    auto event = make_shared<Event>(eventNumber);
    return Variable::ofEvent(event);
}

Variable Routines::signalEvent(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto toRun = getEvent(args, 1);

    if (object && toRun) {
        debug(boost::format("Event signalled: %s %s") % object->tag() % toRun->number(), 2);
        _game.services().scriptRunner().run(object->getOnUserDefined(), object->id(), kObjectInvalid, toRun->number());
    } else if (!object) {
        debug("Script: signalEvent: object is invalid", 1, DebugChannels::script);
    } else if (!toRun) {
        debug("Script: signalEvent: toRun is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getUserDefinedEventNumber(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(ctx.userDefinedEventNumber);
}

} // namespace game

} // namespace reone
