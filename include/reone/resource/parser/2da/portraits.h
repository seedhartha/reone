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

struct portraits {
    std::optional<int> appearance_l;
    std::optional<int> appearance_s;
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

portraits parse_portraits(const TwoDA &twoDA, int row) {
    portraits strct;
    strct.appearance_l = twoDA.getIntOpt(row, "appearance_l");
    strct.appearance_s = twoDA.getIntOpt(row, "appearance_s");
    strct.appearancenumber = twoDA.getIntOpt(row, "appearancenumber");
    strct.baseresref = twoDA.getStringOpt(row, "baseresref");
    strct.baseresrefe = twoDA.getStringOpt(row, "baseresrefe");
    strct.baseresrefve = twoDA.getStringOpt(row, "baseresrefve");
    strct.baseresrefvve = twoDA.getStringOpt(row, "baseresrefvve");
    strct.baseresrefvvve = twoDA.getStringOpt(row, "baseresrefvvve");
    strct.forpc = twoDA.getBool(row, "forpc");
    strct.inanimatetype = twoDA.getStringOpt(row, "inanimatetype");
    strct.lowgore = twoDA.getStringOpt(row, "lowgore");
    strct.plot = twoDA.getBoolOpt(row, "plot");
    strct.race = twoDA.getIntOpt(row, "race");
    strct.sex = twoDA.getInt(row, "sex");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

