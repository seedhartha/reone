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

#include "sound.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../../resource/format/2dareader.h"
#include "../../resource/resources.h"

#include "../object/sound.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

SoundBlueprint::SoundBlueprint(const string &resRef, const shared_ptr<GffStruct> &uts) :
    _resRef(resRef),
    _uts(uts) {

    if (!uts) {
        throw invalid_argument("uts must not be null");
    }
}

void SoundBlueprint::load(Sound &sound) {
    sound._tag = boost::to_lower_copy(_uts->getString("Tag"));
    sound._active = _uts->getBool("Active");

    shared_ptr<TwoDA> priorityGroups(Resources::instance().get2DA("prioritygroups"));
    sound._priority = priorityGroups->getInt(_uts->getInt("Priority"), "priority");

    sound._maxDistance = _uts->getFloat("MaxDistance");
    sound._minDistance = _uts->getFloat("MinDistance");
    sound._continuous = _uts->getBool("Continuous");
    sound._elevation = _uts->getFloat("Elevation");
    sound._looping = _uts->getBool("Looping");
    sound._positional = _uts->getBool("Positional");
    sound._interval = _uts->getInt("Interval");
    sound._volume = _uts->getInt("Volume");

    for (auto &soundGffs : _uts->getList("Sounds")) {
        sound._sounds.push_back(boost::to_lower_copy(soundGffs->getString("Sound")));
    }
}

} // namespace game

} // namespace reone
