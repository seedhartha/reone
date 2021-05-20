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

#include "runner.h"

#include <stdexcept>

#include "../../script/execution.h"
#include "../../script/scripts.h"

#include "../game.h"

#include "routines.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

ScriptRunner::ScriptRunner(Game *game) : _game(game) {
    if (!game) {
        throw invalid_argument("game must not be null");
    }
}

int ScriptRunner::run(const string &resRef, uint32_t callerId, uint32_t triggerrerId, int userDefinedEventNumber, int scriptVar) {
    if (callerId == kObjectSelf) {
        throw invalid_argument("Invalid callerId");
    }
    if (triggerrerId == kObjectSelf) {
        throw invalid_argument("Invalid triggerrerId");
    }

    auto program = _game->scripts().get(resRef);
    if (!program) return -1;

    auto ctx = make_unique<ExecutionContext>();
    ctx->routines = &_game->routines();
    ctx->callerId = callerId;
    ctx->triggererId = triggerrerId;
    ctx->userDefinedEventNumber = userDefinedEventNumber;
    ctx->scriptVar = scriptVar;

    return ScriptExecution(program, move(ctx)).run();
}

} // namespace game

} // namespace reone
