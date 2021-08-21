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
 *  Implementation of routines related to sound objects.
 */

#include "declarations.h"

#include "../../../../common/log.h"
#include "../../../../script/types.h"

#include "../../../object/sound.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable soundObjectSetFixedVariance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable soundObjectGetFixedVariance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable soundObjectPlay(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto sound = getSound(game, args, 0, ctx);
    if (sound) {
        sound->play();
    } else {
        debug("Script: soundObjectPlay: sound is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable soundObjectStop(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto sound = getSound(game, args, 0, ctx);
    if (sound) {
        sound->stop();
    } else {
        debug("Script: soundObjectStop: sound is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable soundObjectSetVolume(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable soundObjectSetPosition(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable soundObjectGetPitchVariance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable soundObjectSetPitchVariance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable soundObjectGetVolume(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable soundObjectFadeAndStop(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

} // namespace routine

} // namespace game

} // namespace reone
