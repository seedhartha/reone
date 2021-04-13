/*
 * Copyright (c) 2020-2021 The reone project contributors
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

/** @file
 *  Sound functions related to blueprint loading.
 */

#include "sound.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/resources.h"
#include "../../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Sound::loadUTS(const GffStruct &uts) {
    _tag = boost::to_lower_copy(uts.getString("Tag"));
    _name = Strings::instance().get(uts.getInt("LocName"));
    _blueprintResRef = boost::to_lower_copy(uts.getString("TemplateResRef"));
    _active = uts.getBool("Active");
    _continuous = uts.getBool("Continuous");
    _looping = uts.getBool("Looping");
    _positional = uts.getBool("Positional");
    _randomPosition = uts.getBool("RandomPosition");
    _random = uts.getInt("Random");
    _elevation = uts.getFloat("Elevation");
    _maxDistance = uts.getFloat("MaxDistance");
    _minDistance = uts.getFloat("MinDistance");
    _randomRangeX = uts.getFloat("RandomRangeX");
    _randomRangeY = uts.getFloat("RandomRangeY");
    _interval = uts.getInt("Interval");
    _intervalVrtn = uts.getInt("IntervalVrtn");
    _pitchVariation = uts.getFloat("PitchVariation");
    _volume = uts.getInt("Volume");
    _volumeVrtn = uts.getInt("VolumeVrtn");

    loadPriorityFromUTS(uts);

    for (auto &soundGffs : uts.getList("Sounds")) {
        _sounds.push_back(boost::to_lower_copy(soundGffs->getString("Sound")));
    }

    // Unused fields:
    //
    // - Hours (always 0)
    // - Times (always 3)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Sound::loadPriorityFromUTS(const GffStruct &uts) {
    shared_ptr<TwoDA> priorityGroups(Resources::instance().get2DA("prioritygroups"));
    int priorityIdx = uts.getInt("Priority");
    _priority = priorityGroups->getInt(priorityIdx, "priority");
}

} // namespace game

} // namespace reone
