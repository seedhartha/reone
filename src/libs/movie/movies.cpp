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

#include "reone/movie/movies.h"

#include "reone/movie/format/bikreader.h"
#include "reone/system/pathutil.h"

using namespace std;

namespace reone {

namespace movie {

shared_ptr<Movie> Movies::doGet(std::string name) {
    auto path = getPathIgnoreCase(_gamePath, "movies/" + name + ".bik");
    if (path.empty()) {
        return nullptr;
    }

    BikReader bik(path, _graphicsSvc, _audioSvc);
    bik.load();

    return bik.movie();
}

} // namespace movie

} // namespace reone
