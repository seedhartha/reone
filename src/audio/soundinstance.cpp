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

#include "soundinstance.h"

#include <stdexcept>
#include <utility>

#include "AL/al.h"

#include "../common/log.h"

#include "files.h"
#include "soundhandle.h"

using namespace std;

namespace reone {

namespace audio {

static const int kMaxBufferCount = 8;

SoundInstance::SoundInstance(const string &resRef, Vector3 position, bool loop, float gain) :
    _resRef(resRef),
    _loop(loop),
    _gain(gain),
    _handle(new SoundHandle(position)) {

    if (resRef.empty()) {
        throw invalid_argument("resRef must not be empty");
    }
}

SoundInstance::SoundInstance(const shared_ptr<AudioStream> &stream, Vector3 position, bool loop, float gain) :
    _stream(stream),
    _loop(loop),
    _gain(gain),
    _handle(new SoundHandle(position)) {

    if (!stream) {
        throw invalid_argument("stream must not be null");
    }
}

void SoundInstance::init() {
    if (!_stream) {
        _stream = AudioFiles::instance().get(_resRef);
        if (!_stream) {
            warn("SoundInstance: file not found: " + _resRef);
            _handle->setState(SoundHandle::State::Stopped);
            return;
        }
    }
    _handle->setDuration(_stream->duration());

    int frameCount = _stream->frameCount();
    int bufferCount = min(max(frameCount, 1), kMaxBufferCount);

    _buffers.resize(bufferCount);
    _buffered = bufferCount > 1;

    alGenBuffers(bufferCount, &_buffers[0]);
    alGenSources(1, &_source);
    alSourcef(_source, AL_GAIN, _gain);

    Vector3 position(_handle->position());
    alSource3f(_source, AL_POSITION, position.x, position.y, position.z);

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
    if (_handle->isPositionDirty()) {
        Vector3 position(_handle->position());
        alSource3f(_source, AL_POSITION, position.x, position.y, position.z);
        _handle->setPositionDirty(false);
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
        if (_loop && _nextFrame == _stream->frameCount()) {
            _nextFrame = 0;
        }
        if (_nextFrame < _stream->frameCount()) {
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

shared_ptr<SoundHandle> SoundInstance::handle() const {
    return _handle;
}

} // namespace audio

} // namespace reone
