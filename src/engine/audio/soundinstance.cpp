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

#include "soundinstance.h"

#include "../common/guardutil.h"
#include "../common/logutil.h"

#include "soundhandle.h"
#include "stream.h"

using namespace std;

namespace reone {

namespace audio {

static constexpr int kMaxBufferCount = 8;

SoundInstance::SoundInstance(shared_ptr<AudioStream> stream, bool loop, float gain, bool positional, glm::vec3 position) :
    _stream(stream),
    _loop(loop),
    _gain(gain),
    _positional(positional),
    _handle(make_shared<SoundHandle>(stream->duration(), move(position))) {

    ensureNotNull(stream, "stream");
}

void SoundInstance::init() {
    int frameCount = _stream->getFrameCount();
    int bufferCount = min(max(frameCount, 1), kMaxBufferCount);

    _buffers.resize(bufferCount);
    _buffered = bufferCount > 1;

    alGenBuffers(bufferCount, &_buffers[0]);
    alGenSources(1, &_source);
    alSourcef(_source, AL_GAIN, _gain);

    if (_positional) {
        glm::vec3 position(_handle->position());
        alSource3f(_source, AL_POSITION, position.x, position.y, position.z);
    } else {
        alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
    }
    if (_buffered) {
        for (int i = 0; i < bufferCount; ++i) {
            _stream->fill(_nextFrame, _buffers[i]);
            ++_nextFrame;
        }
        alSourceQueueBuffers(_source, bufferCount, &_buffers[0]);
    } else {
        _stream->fill(0, _buffers[0]);
        alSourcei(_source, AL_BUFFER, _buffers[0]);
        alSourcei(_source, AL_LOOPING, _loop);
    }

    alSourcePlay(_source);
    _handle->setState(SoundHandle::State::Playing);
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
    if (_positional && _handle->isPositionDirty()) {
        glm::vec3 position(_handle->position());
        alSource3f(_source, AL_POSITION, position.x, position.y, position.z);
        _handle->resetPositionDirty();
    }
    if (!_buffered) {
        ALint state = 0;
        alGetSourcei(_source, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            _handle->setState(SoundHandle::State::Stopped);
        }
        return;
    }
    ALint processed = 0;
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
    while (processed-- > 0) {
        alSourceUnqueueBuffers(_source, 1, &_buffers[_nextBuffer]);
        if (_loop && _nextFrame == _stream->getFrameCount()) {
            _nextFrame = 0;
        }
        if (_nextFrame < _stream->getFrameCount()) {
            _stream->fill(_nextFrame++, _buffers[_nextBuffer]);
            alSourceQueueBuffers(_source, 1, &_buffers[_nextBuffer]);
        }
        _nextBuffer = (_nextBuffer + 1) % static_cast<int>(_buffers.size());
    }
    ALint queued = 0;
    alGetSourcei(_source, AL_BUFFERS_QUEUED, &queued);
    if (queued == 0) {
        _handle->setState(SoundHandle::State::Stopped);
    }
}

} // namespace audio

} // namespace reone
