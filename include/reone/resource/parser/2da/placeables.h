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

struct placeables {
    bool bodybag;
    float canseeheight;
    bool hitcheck;
    bool hostile;
    bool ignorestatichitcheck;
    std::optional<std::string> label;
    std::optional<std::string> lightcolor;
    std::optional<std::string> lightoffsetx;
    std::optional<std::string> lightoffsety;
    std::optional<std::string> lightoffsetz;
    std::optional<std::string> lowgore;
    std::optional<std::string> modelname;
    bool noncull;
    bool preciseuse;
    bool shadowsize;
    std::optional<int> soundapptype;
    int strref;
    bool usesearch;
};

placeables parse_placeables(const TwoDA &twoDA, int row) {
    placeables strct;
    strct.bodybag = twoDA.getBool(row, "bodybag");
    strct.canseeheight = twoDA.getFloat(row, "canseeheight");
    strct.hitcheck = twoDA.getBool(row, "hitcheck");
    strct.hostile = twoDA.getBool(row, "hostile");
    strct.ignorestatichitcheck = twoDA.getBool(row, "ignorestatichitcheck");
    strct.label = twoDA.getStringOpt(row, "label");
    strct.lightcolor = twoDA.getStringOpt(row, "lightcolor");
    strct.lightoffsetx = twoDA.getStringOpt(row, "lightoffsetx");
    strct.lightoffsety = twoDA.getStringOpt(row, "lightoffsety");
    strct.lightoffsetz = twoDA.getStringOpt(row, "lightoffsetz");
    strct.lowgore = twoDA.getStringOpt(row, "lowgore");
    strct.modelname = twoDA.getStringOpt(row, "modelname");
    strct.noncull = twoDA.getBool(row, "noncull");
    strct.preciseuse = twoDA.getBool(row, "preciseuse");
    strct.shadowsize = twoDA.getBool(row, "shadowsize");
    strct.soundapptype = twoDA.getIntOpt(row, "soundapptype");
    strct.strref = twoDA.getInt(row, "strref");
    strct.usesearch = twoDA.getBool(row, "usesearch");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

