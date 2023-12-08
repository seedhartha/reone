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

#include "reone/audio/context.h"

#include "reone/system/threadutil.h"

namespace reone {

namespace audio {

void Context::init() {
    checkMainThread();
    _device = alcOpenDevice(nullptr);
    if (!_device) {
        throw std::runtime_error("alcOpenDevice failed");
    }
    _context = alcCreateContext(_device, nullptr);
    if (!_context) {
        throw std::runtime_error("alcCreateContext failed");
    }
    alcMakeContextCurrent(_context);
}

void Context::deinit() {
    checkMainThread();
    if (_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(_context);
        _context = nullptr;
    }
    if (_device) {
        alcCloseDevice(_device);
        _device = nullptr;
    }
}

void Context::setListenerPosition(glm::vec3 position) {
    if (_listenerPosition == position) {
        return;
    }
    alListener3f(AL_POSITION, position.x, position.y, position.z);
    _listenerPosition = std::move(position);
}

} // namespace audio

} // namespace reone
