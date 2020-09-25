/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "soundinstance.h"

#include "AL/al.h"

using namespace std;

namespace reone {

namespace audio {

SoundInstance::SoundInstance(const shared_ptr<AudioStream> &stream, bool loop, float gain) :
    _stream(stream), _loop(loop), _gain(gain) {

    _multiframe = _stream->frameCount() > 1;
}

void SoundInstance::init() {
    int bufferCount = _multiframe ? 2 : 1;
    _buffers.resize(bufferCount);
    alGenBuffers(bufferCount, &_buffers[0]);
    alGenSources(1, &_source);
    alSourcef(_source, AL_GAIN, _gain);

    if (_multiframe) {
        _stream->fill(_nextFrame++, _buffers[0]);
        _stream->fill(_nextFrame++, _buffers[1]);
        alSourceQueueBuffers(_source, 2, &_buffers[0]);
    } else {
        _stream->fill(0, _buffers[0]);
        alSourcei(_source, AL_BUFFER, _buffers[0]);
        alSourcei(_source, AL_LOOPING, _loop);
    }

    alSourcePlay(_source);
    _state = State::Playing;
}

SoundInstance::~SoundInstance() {
    deinit();
}

void SoundInstance::deinit() {
    if (_source) {
        alSourceStop(_source);
        alDeleteSources(1, &_source);
        _source = 0;
    }
    if (!_buffers.empty()) {
        alDeleteBuffers(static_cast<int>(_buffers.size()), &_buffers[0]);
        _buffers.clear();
    }
}

void SoundInstance::update() {
    if (_state == State::NotInited) {
        init();
    }
    if (!_multiframe) {
        ALint state = 0;
        alGetSourcei(_source, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            _state = State::Stopped;
        }
        return;
    }

    ALint processed = 0;
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);

    while (processed--) {
        alSourceUnqueueBuffers(_source, 1, &_buffers[_nextBuffer]);
        if (_nextFrame >= _stream->frameCount()) {
            if (_loop) {
                _nextFrame = 0;
            } else {
                _state = State::Stopped;
                break;
            }
        }
        _stream->fill(_nextFrame++, _buffers[_nextBuffer]);
        alSourceQueueBuffers(_source, 1, &_buffers[_nextBuffer]);
        _nextBuffer = (++_nextBuffer) % 2;
    }
}

void SoundInstance::stop() {
    alSourceStop(_source);
    _state = State::Stopped;
}

bool SoundInstance::stopped() const {
    return _state == State::Stopped;
}

} // namespace audio

} // namespace reone
