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

#pragma once

#include <string>
#include <memory>

#include "glm/vec3.hpp"

#include "stream.h"

namespace reone {

namespace audio {

class SoundHandle;

class SoundInstance {
public:
    SoundInstance(const std::shared_ptr<AudioStream> &stream, bool loop, float gain, bool positional, glm::vec3 position);
    SoundInstance(SoundInstance &&) = default;
    ~SoundInstance();

    SoundInstance &operator=(SoundInstance &&) = default;

    void init();
    void update();

    std::shared_ptr<SoundHandle> handle() const;

private:
    std::shared_ptr<AudioStream> _stream;
    bool _loop { false };
    float _gain { 0.0f };
    bool _positional { false };
    std::shared_ptr<SoundHandle> _handle;
    std::vector<uint32_t> _buffers;
    bool _buffered { false };
    uint32_t _source { 0 };
    int _nextFrame { 0 };
    int _nextBuffer { 0 };

    SoundInstance(SoundInstance &) = delete;
    SoundInstance &operator=(SoundInstance &) = delete;

    void deinit();
};

} // namespace audio

} // namespace reone
