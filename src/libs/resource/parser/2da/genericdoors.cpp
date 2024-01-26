
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

#include "reone/resource/parser/2da/genericdoors.h"

#include "reone/resource/2da.h"

namespace reone {

namespace resource {

namespace generated {

GenericdoorsTwoDARow parseGenericdoorsTwoDARow(const TwoDA &twoDA, int rownum) {
    GenericdoorsTwoDARow row;
    row.blocksight = twoDA.getBool(rownum, "blocksight");
    row.label = twoDA.getString(rownum, "label");
    row.modelname = twoDA.getString(rownum, "modelname");
    row.name = twoDA.getStringOpt(rownum, "name");
    row.nobin = twoDA.getBool(rownum, "nobin");
    row.preciseuse = twoDA.getBool(rownum, "preciseuse");
    row.soundapptype = twoDA.getIntOpt(rownum, "soundapptype");
    row.staticanim = twoDA.getStringOpt(rownum, "staticanim");
    row.strref = twoDA.getIntOpt(rownum, "strref");
    row.visiblemodel = twoDA.getBool(rownum, "visiblemodel");
    return row;
}

GenericdoorsTwoDA parseGenericdoorsTwoDA(const TwoDA &twoDA) {
    GenericdoorsTwoDA strct;
    for (int i = 0; i < twoDA.getRowCount(); ++i) {
        strct.rows.push_back(parseGenericdoorsTwoDARow(twoDA, i));
    }
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
