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

#include "../../audio/player.h"

using namespace std;

using namespace reone::audio;

namespace reone {

namespace scene {

void SoundSceneNode::playSound(const string &resRef, float gain, bool positional, bool loop) {
    _sound = _audioPlayer.play(resRef, AudioType::Sound, loop, gain, positional, _absTransform[3]);
}

bool SoundSceneNode::isSoundPlaying() const {
    return _sound && !_sound->isStopped();
}

void SoundSceneNode::setAudible(bool audible) {
    if (_audible == audible) {
        return;
    }
    if (!audible && _sound) {
        _sound->stop();
        _sound.reset();
    }
    _audible = audible;
}

} // namespace scene

} // namespace reone
