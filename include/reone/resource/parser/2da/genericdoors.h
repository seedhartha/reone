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

struct genericdoors {
    bool blocksight;
    std::string label;
    std::string modelname;
    std::optional<std::string> name;
    bool nobin;
    bool preciseuse;
    std::optional<int> soundapptype;
    std::optional<std::string> staticanim;
    std::optional<int> strref;
    bool visiblemodel;
};

genericdoors parse_genericdoors(const TwoDA &twoDA, int row) {
    genericdoors strct;
    strct.blocksight = twoDA.getBool(row, "blocksight");
    strct.label = twoDA.getString(row, "label");
    strct.modelname = twoDA.getString(row, "modelname");
    strct.name = twoDA.getStringOpt(row, "name");
    strct.nobin = twoDA.getBool(row, "nobin");
    strct.preciseuse = twoDA.getBool(row, "preciseuse");
    strct.soundapptype = twoDA.getIntOpt(row, "soundapptype");
    strct.staticanim = twoDA.getStringOpt(row, "staticanim");
    strct.strref = twoDA.getIntOpt(row, "strref");
    strct.visiblemodel = twoDA.getBool(row, "visiblemodel");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

