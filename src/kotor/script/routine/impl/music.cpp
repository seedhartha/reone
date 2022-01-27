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
 *  Implementation of music-related routines.
 */

#include "../declarations.h"

#include "../../../../common/logutil.h"
#include "../../../../script/exception/notimpl.h"
#include "../../../../script/types.h"

#include "../context.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

Variable musicBackgroundPlay(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable musicBackgroundStop(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable musicBackgroundChangeDay(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable musicBackgroundChangeNight(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable musicBattlePlay(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable musicBattleStop(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable musicBackgroundGetBattleTrack(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setMusicVolume(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace kotor

} // namespace reone
