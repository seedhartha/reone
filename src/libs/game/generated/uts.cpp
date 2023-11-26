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

#include "reone/game/generated/uts.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace generated {

static UTS_Sounds parseUTS_Sounds(const Gff &gff) {
    UTS_Sounds strct;
    strct.Sound = gff.getString("Sound");
    return strct;
}

UTS parseUTS(const Gff &gff) {
    UTS strct;
    strct.Active = gff.getUint("Active");
    strct.Comment = gff.getString("Comment");
    strct.Continuous = gff.getUint("Continuous");
    strct.Elevation = gff.getFloat("Elevation");
    strct.Hours = gff.getUint("Hours");
    strct.Interval = gff.getUint("Interval");
    strct.IntervalVrtn = gff.getUint("IntervalVrtn");
    strct.LocName = std::make_pair(gff.getInt("LocName"), gff.getString("LocName"));
    strct.Looping = gff.getUint("Looping");
    strct.MaxDistance = gff.getFloat("MaxDistance");
    strct.MinDistance = gff.getFloat("MinDistance");
    strct.PaletteID = gff.getUint("PaletteID");
    strct.PitchVariation = gff.getFloat("PitchVariation");
    strct.Positional = gff.getUint("Positional");
    strct.Priority = gff.getUint("Priority");
    strct.Random = gff.getUint("Random");
    strct.RandomPosition = gff.getUint("RandomPosition");
    strct.RandomRangeX = gff.getFloat("RandomRangeX");
    strct.RandomRangeY = gff.getFloat("RandomRangeY");
    for (auto &item : gff.getList("Sounds")) {
        strct.Sounds.push_back(parseUTS_Sounds(*item));
    }
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.Times = gff.getUint("Times");
    strct.Volume = gff.getUint("Volume");
    strct.VolumeVrtn = gff.getUint("VolumeVrtn");
    return strct;
}

} // namespace generated

} // namespace game

} // namespace reone
