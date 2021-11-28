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
 *  Implementation of movie-related routines.
 */

#include "../declarations.h"

#include "../../../../script/exception/notimpl.h"
#include "../../../../script/types.h"

#include "../../../game.h"

#include "../argutil.h"
#include "../context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable playMovie(const vector<Variable> &args, const RoutineContext &ctx) {
    string movie(boost::to_lower_copy(getString(args, 0)));

    ctx.game.playVideo(movie);

    return Variable::ofNull();
}

Variable isMoviePlaying(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable queueMovie(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable playMovieQueue(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
