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

struct PortraitsTwoDARow {
    std::optional<int> appearanceL;
    std::optional<int> appearanceS;
    std::optional<int> appearancenumber;
    std::optional<std::string> baseresref;
    std::optional<std::string> baseresrefe;
    std::optional<std::string> baseresrefve;
    std::optional<std::string> baseresrefvve;
    std::optional<std::string> baseresrefvvve;
    bool forpc;
    std::optional<std::string> inanimatetype;
    std::optional<std::string> lowgore;
    std::optional<bool> plot;
    std::optional<int> race;
    int sex;
};

struct PortraitsTwoDA {
    std::vector<PortraitsTwoDARow> rows;
};

PortraitsTwoDARow parsePortraitsTwoDARow(const TwoDA &twoDA, int rownum) {
    PortraitsTwoDARow row;
    row.appearanceL = twoDA.getIntOpt(rownum, "appearanceL");
    row.appearanceS = twoDA.getIntOpt(rownum, "appearanceS");
    row.appearancenumber = twoDA.getIntOpt(rownum, "appearancenumber");
    row.baseresref = twoDA.getStringOpt(rownum, "baseresref");
    row.baseresrefe = twoDA.getStringOpt(rownum, "baseresrefe");
    row.baseresrefve = twoDA.getStringOpt(rownum, "baseresrefve");
    row.baseresrefvve = twoDA.getStringOpt(rownum, "baseresrefvve");
    row.baseresrefvvve = twoDA.getStringOpt(rownum, "baseresrefvvve");
    row.forpc = twoDA.getBool(rownum, "forpc");
    row.inanimatetype = twoDA.getStringOpt(rownum, "inanimatetype");
    row.lowgore = twoDA.getStringOpt(rownum, "lowgore");
    row.plot = twoDA.getBoolOpt(rownum, "plot");
    row.race = twoDA.getIntOpt(rownum, "race");
    row.sex = twoDA.getInt(rownum, "sex");
    return row;
}

PortraitsTwoDA parsePortraitsTwoDA(const TwoDA &twoDA) {
    PortraitsTwoDA strct;
    for (int i = 0; i < twoDA.getRowCount(); ++i) {
        strct.rows.push_back(parsePortraitsTwoDARow(twoDA, i));
    }
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

