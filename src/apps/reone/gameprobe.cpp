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

#include "gameprobe.h"

#include "reone/resource/exception/format.h"
#include "reone/system/fileutil.h"

using namespace reone::resource;

namespace reone {

GameID GameProbe::probe() {
    // If there is a KotOR executable then game is KotOR
    auto exePathK1 = findFileIgnoreCase(_gamePath, "swkotor.exe");
    if (exePathK1) {
        return GameID::KotOR;
    }

    // If there is a TSL executable then game is TSL
    auto exePathK2 = findFileIgnoreCase(_gamePath, "swkotor2.exe");
    if (exePathK2) {
        return GameID::TSL;
    }

    throw std::runtime_error("Unable to determine game ID: " + _gamePath.string());
}

} // namespace reone
