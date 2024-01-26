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

namespace reone {

namespace resource {

class TwoDA;

namespace generated {

struct PlaceablesTwoDARow {
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

PlaceablesTwoDARow parsePlaceablesTwoDARow(const TwoDA &twoDA, int rownum);

struct PlaceablesTwoDA {
    std::vector<PlaceablesTwoDARow> rows;
};

PlaceablesTwoDA parsePlaceablesTwoDA(const TwoDA &twoDA);

} // namespace generated

} // namespace resource

} // namespace reone
