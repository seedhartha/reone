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

#include "../declarations.h"

#include "../../../../../common/logutil.h"
#include "../../../../../script/exception/notimpl.h"
#include "../../../../../script/types.h"

#include "../../../object/sound.h"
#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable soundObjectSetFixedVariance(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable soundObjectGetFixedVariance(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable soundObjectPlay(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getSound(args, 0, ctx);
    sound->setActive(true);
    return Variable::ofNull();
}

Variable soundObjectStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto sound = getSound(args, 0, ctx);
    sound->setActive(false);
    return Variable::ofNull();
}

Variable soundObjectSetVolume(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable soundObjectSetPosition(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable soundObjectGetPitchVariance(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable soundObjectSetPitchVariance(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable soundObjectGetVolume(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable soundObjectFadeAndStop(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
