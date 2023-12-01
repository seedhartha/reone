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

#include "reone/game/script/runner.h"

#include "reone/game/game.h"
#include "reone/resource/provider/scripts.h"
#include "reone/script/execution.h"
#include "reone/script/executioncontext.h"
#include "reone/script/routines.h"

using namespace reone::script;

namespace reone {

namespace game {

int ScriptRunner::run(const std::string &resRef, uint32_t callerId, uint32_t triggerrerId, int userDefinedEventNumber, int scriptVar) {
    if (callerId == kObjectSelf) {
        throw std::invalid_argument("Invalid callerId: " + std::to_string(callerId));
    }
    if (triggerrerId == kObjectSelf) {
        throw std::invalid_argument("Invalid triggerrerId: " + std::to_string(triggerrerId));
    }

    auto program = _scripts.get(resRef);
    if (!program)
        return -1;

    auto ctx = std::make_unique<ExecutionContext>();
    ctx->routines = &_routines;
    ctx->callerId = callerId;
    ctx->triggererId = triggerrerId;
    ctx->userDefinedEventNumber = userDefinedEventNumber;
    ctx->scriptVar = scriptVar;

    return ScriptExecution(program, std::move(ctx)).run();
}

} // namespace game

} // namespace reone
