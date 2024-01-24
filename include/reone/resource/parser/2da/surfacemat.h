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

#pragma once

#include "reone/resource/2da.h"

namespace reone {

namespace resource {

namespace generated {

struct surfacemat {
    bool grass;
    std::string label;
    bool lineofsight;
    std::optional<std::string> name;
    std::optional<std::string> sound;
    bool walk;
    bool walkcheck;
};

surfacemat parse_surfacemat(const TwoDA &twoDA, int row) {
    surfacemat strct;
    strct.grass = twoDA.getBool(row, "grass");
    strct.label = twoDA.getString(row, "label");
    strct.lineofsight = twoDA.getBool(row, "lineofsight");
    strct.name = twoDA.getStringOpt(row, "name");
    strct.sound = twoDA.getStringOpt(row, "sound");
    strct.walk = twoDA.getBool(row, "walk");
    strct.walkcheck = twoDA.getBool(row, "walkcheck");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

