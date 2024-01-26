
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

#include "reone/resource/parser/2da/heads.h"

#include "reone/resource/2da.h"

namespace reone {

namespace resource {

namespace generated {

HeadsTwoDARow parseHeadsTwoDARow(const TwoDA &twoDA, int rownum) {
    HeadsTwoDARow row;
    row.alttexture = twoDA.getStringOpt(rownum, "alttexture");
    row.head = twoDA.getStringOpt(rownum, "head");
    row.headtexe = twoDA.getStringOpt(rownum, "headtexe");
    row.headtexg = twoDA.getStringOpt(rownum, "headtexg");
    row.headtexve = twoDA.getStringOpt(rownum, "headtexve");
    row.headtexvg = twoDA.getStringOpt(rownum, "headtexvg");
    row.headtexvve = twoDA.getStringOpt(rownum, "headtexvve");
    row.headtexvvve = twoDA.getStringOpt(rownum, "headtexvvve");
    return row;
}

HeadsTwoDA parseHeadsTwoDA(const TwoDA &twoDA) {
    HeadsTwoDA strct;
    for (int i = 0; i < twoDA.getRowCount(); ++i) {
        strct.rows.push_back(parseHeadsTwoDARow(twoDA, i));
    }
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
