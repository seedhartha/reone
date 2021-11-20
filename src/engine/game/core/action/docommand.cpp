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

#include "docommand.h"

#include "../../../script/execution.h"
#include "../../../script/executioncontext.h"
#include "../../../script/program.h"

#include "../object/object.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

void CommandAction::execute(Object &actor, float dt) {
    auto executionCtx = make_unique<ExecutionContext>(*_context);
    executionCtx->callerId = actor.id();

    shared_ptr<ScriptProgram> program(_context->savedState->program);
    ScriptExecution(program, move(executionCtx)).run();
    complete();
}

} // namespace game

} // namespace reone
