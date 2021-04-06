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

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Sound::loadUTS(const GffStruct &uts) {
    _active = uts.getBool("Active");
    _continuous = uts.getBool("Continuous");
    _elevation = uts.getFloat("Elevation");
    _interval = uts.getInt("Interval");
    _looping = uts.getBool("Looping");
    _maxDistance = uts.getFloat("MaxDistance");
    _minDistance = uts.getFloat("MinDistance");
    _positional = uts.getBool("Positional");
    _tag = boost::to_lower_copy(uts.getString("Tag"));
    _volume = uts.getInt("Volume");

    loadPriorityFromUTS(uts);

    for (auto &soundGffs : uts.getList("Sounds")) {
        _sounds.push_back(boost::to_lower_copy(soundGffs->getString("Sound")));
    }
}

void Sound::loadPriorityFromUTS(const GffStruct &uts) {
    shared_ptr<TwoDA> priorityGroups(Resources::instance().get2DA("prioritygroups"));
    int priorityIdx = uts.getInt("Priority");
    _priority = priorityGroups->getInt(priorityIdx, "priority");
}

} // namespace game

} // namespace reone
