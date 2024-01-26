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

struct SurfacematTwoDARow {
    bool grass;
    std::string label;
    bool lineofsight;
    std::optional<std::string> name;
    std::optional<std::string> sound;
    bool walk;
    bool walkcheck;
};

struct SurfacematTwoDA {
    std::vector<SurfacematTwoDARow> rows;
};

SurfacematTwoDARow parseSurfacematTwoDARow(const TwoDA &twoDA, int rownum) {
    SurfacematTwoDARow row;
    row.grass = twoDA.getBool(rownum, "grass");
    row.label = twoDA.getString(rownum, "label");
    row.lineofsight = twoDA.getBool(rownum, "lineofsight");
    row.name = twoDA.getStringOpt(rownum, "name");
    row.sound = twoDA.getStringOpt(rownum, "sound");
    row.walk = twoDA.getBool(rownum, "walk");
    row.walkcheck = twoDA.getBool(rownum, "walkcheck");
    return row;
}

SurfacematTwoDA parseSurfacematTwoDA(const TwoDA &twoDA) {
    SurfacematTwoDA strct;
    for (int i = 0; i < twoDA.getRowCount(); ++i) {
        strct.rows.push_back(parseSurfacematTwoDARow(twoDA, i));
    }
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

