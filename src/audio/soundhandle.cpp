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

#include "soundhandle.h"

using namespace std;

namespace reone {

namespace audio {

SoundHandle::SoundHandle(float duration, Vector3 position) : _duration(duration), _position(position) {
}

void SoundHandle::stop() {
    _state = State::Stopped;
}

void SoundHandle::resetPositionDirty() {
    _positionDirty = false;
}

bool SoundHandle::isNotInited() const {
    return _state == State::NotInited;
}

bool SoundHandle::isStopped() const {
    return _state == State::Stopped;
}

int SoundHandle::duration() const {
    return _duration;
}

Vector3 SoundHandle::position() const {
    return _position;
}

bool SoundHandle::isPositionDirty() const {
    return _positionDirty;
}

void SoundHandle::setState(State state) {
    _state = state;
}

void SoundHandle::setPosition(Vector3 position) {
    if (_position.load() != position) {
        _position = move(position);
        _positionDirty = true;
    }
}

} // namespace audio

} // namespace reone
