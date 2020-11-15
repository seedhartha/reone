/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "routines.h"

#include "SDL2/SDL_timer.h"

#include "../../common/log.h"
#include "../../common/random.h"

#include "../action/commandaction.h"
#include "../action/movetoobject.h"
#include "../action/movetopoint.h"
#include "../action/startconversation.h"
#include "../game.h"
#include "../object/area.h"
#include "../object/creature.h"
#include "../object/door.h"

#include "util.h"

using namespace std;

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::random(const vector<Variable> &args, ExecutionContext &ctx) {
    return reone::random(0, args[0].intValue - 1);
}

Variable Routines::intToFloat(const vector<Variable> & args, ExecutionContext & ctx) {
    return static_cast<float>(args[0].intValue);
}

Variable Routines::getLoadFromSaveGame(const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable(_game->isLoadFromSaveGame() ? 1 : 0);
}

Variable Routines::d2(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return result;
}

Variable Routines::d3(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return result;
}

Variable Routines::d4(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return result;
}

Variable Routines::d6(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return result;
}

Variable Routines::d8(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return result;
}

Variable Routines::d10(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return result;
}

Variable Routines::d12(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return result;
}

Variable Routines::d20(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return result;
}

Variable Routines::d100(const vector<Variable> &args, ExecutionContext &ctx) {
    int numDice = args.size() > 0 ? args[0].intValue : 1;
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return result;
}

Variable Routines::shipBuild(const vector<Variable> &args, ExecutionContext &ctx) {
    return 1;
}

Variable Routines::executeScript(const vector<Variable> &args, ExecutionContext &ctx) {
    string script(args[0].strValue);
    int targetId = args[1].objectId;
    int scriptVar = args.size() >= 3 ? args[2].intValue : -1;

    shared_ptr<Object> target(getObjectById(targetId, ctx));
    if (target) {
        _game->setRunScriptVar(scriptVar);
        runScript(script, targetId, kObjectInvalid, -1);
    }

    return Variable();
}

Variable Routines::getRunScriptVar(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getRunScriptVar();
}

} // namespace game

} // namespace reone
