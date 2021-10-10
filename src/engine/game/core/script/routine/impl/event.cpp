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
 *  Implementation of event-related routines.
 */

#include "../declarations.h"

#include "../../../../../common/logutil.h"
#include "../../../../../script/exception/notimpl.h"

#include "../../../event.h"
#include "../../../game.h"
#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"
#include "../../../script/runner.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable signalEvent(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);
    auto toRun = getEvent(args, 1);

    debug(boost::format("Event signalled: %s %s") % object->tag() % toRun->number());
    ctx.scriptRunner.run(object->getOnUserDefined(), object->id(), kObjectInvalid, toRun->number());

    return Variable::ofNull();
}

Variable eventUserDefined(const vector<Variable> &args, const RoutineContext &ctx) {
    int eventNumber = getInt(args, 0);
    auto event = make_shared<Event>(eventNumber);

    return Variable::ofEvent(event);
}

Variable eventSpellCastAt(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable eventConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable eventActivateItem(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
