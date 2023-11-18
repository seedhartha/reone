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

#include "reone/game/gffschema/pth.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace gffschema {

static PTH_Path_Points parsePTH_Path_Points(const Gff &gff) {
    PTH_Path_Points strct;
    strct.Conections = gff.getUint("Conections");
    strct.First_Conection = gff.getUint("First_Conection");
    strct.X = gff.getFloat("X");
    strct.Y = gff.getFloat("Y");
    return strct;
}

static PTH_Path_Conections parsePTH_Path_Conections(const Gff &gff) {
    PTH_Path_Conections strct;
    strct.Destination = gff.getUint("Destination");
    return strct;
}

PTH parsePTH(const Gff &gff) {
    PTH strct;
    for (auto &item : gff.getList("Path_Conections")) {
        strct.Path_Conections.push_back(parsePTH_Path_Conections(*item));
    }
    for (auto &item : gff.getList("Path_Points")) {
        strct.Path_Points.push_back(parsePTH_Path_Points(*item));
    }
    return strct;
}

} // namespace gffschema

} // namespace game

} // namespace reone
