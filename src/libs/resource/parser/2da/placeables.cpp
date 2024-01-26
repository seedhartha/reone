
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

#include "reone/resource/parser/2da/placeables.h"

#include "reone/resource/2da.h"

namespace reone {

namespace resource {

namespace generated {

PlaceablesTwoDARow parsePlaceablesTwoDARow(const TwoDA &twoDA, int rownum) {
    PlaceablesTwoDARow row;
    row.bodybag = twoDA.getBool(rownum, "bodybag");
    row.canseeheight = twoDA.getFloat(rownum, "canseeheight");
    row.hitcheck = twoDA.getBool(rownum, "hitcheck");
    row.hostile = twoDA.getBool(rownum, "hostile");
    row.ignorestatichitcheck = twoDA.getBool(rownum, "ignorestatichitcheck");
    row.label = twoDA.getStringOpt(rownum, "label");
    row.lightcolor = twoDA.getStringOpt(rownum, "lightcolor");
    row.lightoffsetx = twoDA.getStringOpt(rownum, "lightoffsetx");
    row.lightoffsety = twoDA.getStringOpt(rownum, "lightoffsety");
    row.lightoffsetz = twoDA.getStringOpt(rownum, "lightoffsetz");
    row.lowgore = twoDA.getStringOpt(rownum, "lowgore");
    row.modelname = twoDA.getStringOpt(rownum, "modelname");
    row.noncull = twoDA.getBool(rownum, "noncull");
    row.preciseuse = twoDA.getBool(rownum, "preciseuse");
    row.shadowsize = twoDA.getBool(rownum, "shadowsize");
    row.soundapptype = twoDA.getIntOpt(rownum, "soundapptype");
    row.strref = twoDA.getInt(rownum, "strref");
    row.usesearch = twoDA.getBool(rownum, "usesearch");
    return row;
}

PlaceablesTwoDA parsePlaceablesTwoDA(const TwoDA &twoDA) {
    PlaceablesTwoDA strct;
    for (int i = 0; i < twoDA.getRowCount(); ++i) {
        strct.rows.push_back(parsePlaceablesTwoDARow(twoDA, i));
    }
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
