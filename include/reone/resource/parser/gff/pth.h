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

class Gff;

namespace generated {

struct PTH_Path_Points {
    uint32_t Conections {0};
    uint32_t First_Conection {0};
    float X {0.0f};
    float Y {0.0f};
};

struct PTH_Path_Conections {
    uint32_t Destination {0};
};

struct PTH {
    std::vector<PTH_Path_Conections> Path_Conections;
    std::vector<PTH_Path_Points> Path_Points;
};

PTH parsePTH(const Gff &gff);

} // namespace generated

} // namespace resource

} // namespace reone
