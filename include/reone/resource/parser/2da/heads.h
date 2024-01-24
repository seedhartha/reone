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

struct heads {
    std::optional<std::string> alttexture;
    std::optional<std::string> head;
    std::optional<std::string> headtexe;
    std::optional<std::string> headtexg;
    std::optional<std::string> headtexve;
    std::optional<std::string> headtexvg;
    std::optional<std::string> headtexvve;
    std::optional<std::string> headtexvvve;
};

heads parse_heads(const TwoDA &twoDA, int row) {
    heads strct;
    strct.alttexture = twoDA.getStringOpt(row, "alttexture");
    strct.head = twoDA.getStringOpt(row, "head");
    strct.headtexe = twoDA.getStringOpt(row, "headtexe");
    strct.headtexg = twoDA.getStringOpt(row, "headtexg");
    strct.headtexve = twoDA.getStringOpt(row, "headtexve");
    strct.headtexvg = twoDA.getStringOpt(row, "headtexvg");
    strct.headtexvve = twoDA.getStringOpt(row, "headtexvve");
    strct.headtexvvve = twoDA.getStringOpt(row, "headtexvvve");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

