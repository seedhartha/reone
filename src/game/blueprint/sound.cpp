/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "sound.h"

#include <boost/algorithm/string.hpp>

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void SoundBlueprint::load(const GffStruct &uts) {
    _tag = uts.getString("Tag");
    boost::to_lower(_tag);

    _active = uts.getInt("Active") != 0;
    _priority = uts.getInt("Priority");
    _maxDistance = uts.getFloat("MaxDistance");
    _minDistance = uts.getFloat("MinDistance");
    _continuous = uts.getInt("Continuous") != 0;
    _looping = uts.getInt("Looping") != 0;
    _interval = uts.getInt("Interval");
    _volume = uts.getInt("Volume");

    for (auto &sound : uts.getList("Sounds")) {
        _sounds.push_back(sound.getString("Sound"));
    }
}

const string &SoundBlueprint::tag() const {
    return _tag;
}

bool SoundBlueprint::active() const {
    return _active;
}

int SoundBlueprint::priority() const {
    return _priority;
}

float SoundBlueprint::maxDistance() const {
    return _maxDistance;
}

float SoundBlueprint::minDistance() const {
    return _minDistance;
}

bool SoundBlueprint::continuous() const {
    return _continuous;
}

bool SoundBlueprint::looping() const {
    return _looping;
}

int SoundBlueprint::interval() const {
    return _interval;
}

int SoundBlueprint::volume() const {
    return _volume;
}

const vector<string> &SoundBlueprint::sounds() const {
    return _sounds;
}

} // namespace game

} // namespace reone
