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

#pragma once

namespace reone {

namespace audio {

class AudioBuffer;

class AudioSource {
public:
    AudioSource(std::shared_ptr<AudioBuffer> stream, bool loop, float gain, bool positional, glm::vec3 position) :
        _stream(std::move(stream)),
        _loop(loop),
        _gain(gain),
        _positional(positional),
        _position(std::move(position)) {
    }

    AudioSource(AudioSource &&) = default;
    ~AudioSource() { deinit(); }

    AudioSource &operator=(AudioSource &&) = default;

    void init();
    void update();

    void play();
    void stop();

    bool isPlaying() const { return _playing; }

    float duration() const;

    void setPosition(glm::vec3 position);

private:
    std::shared_ptr<AudioBuffer> _stream;
    bool _loop;
    float _gain;
    bool _positional;
    glm::vec3 _position;

    bool _inited {false};
    bool _playing {false};

    std::vector<uint32_t> _buffers;
    bool _streaming {false};
    uint32_t _source {0};
    int _nextFrame {0};
    int _nextBuffer {0};

    void deinit();
};

} // namespace audio

} // namespace reone
