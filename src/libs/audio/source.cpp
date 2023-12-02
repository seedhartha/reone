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

#include "reone/audio/source.h"

#include "reone/audio/clip.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace audio {

static constexpr int kMaxBufferCount = 8;

static int getALFormat(AudioFormat format) {
    switch (format) {
    case AudioFormat::Mono8:
        return AL_FORMAT_MONO8;
    case AudioFormat::Mono16:
        return AL_FORMAT_MONO16;
    case AudioFormat::Stereo8:
        return AL_FORMAT_STEREO8;
    case AudioFormat::Stereo16:
        return AL_FORMAT_STEREO16;
    default:
        throw std::invalid_argument("Invalid audio format: " + std::to_string(static_cast<int>(format)));
    }
}

static void fillBuffer(const AudioClip::Frame &frame, uint32_t buffer) {
    alBufferData(
        buffer,
        getALFormat(frame.format),
        &frame.samples[0],
        static_cast<int>(frame.samples.size()),
        frame.sampleRate);
}

void AudioSource::init() {
    if (_inited) {
        return;
    }
    checkMainThread();

    int frameCount = _stream->getFrameCount();
    int bufferCount = std::min(std::max(frameCount, 1), kMaxBufferCount);

    _buffers.resize(bufferCount);
    _streaming = bufferCount > 1;

    alGenBuffers(bufferCount, &_buffers[0]);
    alGenSources(1, &_source);
    alSourcef(_source, AL_GAIN, _gain);

    if (_positional) {
        alSource3f(_source, AL_POSITION, _position.x, _position.y, _position.z);
    } else {
        alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
    }
    if (_streaming) {
        for (int i = 0; i < bufferCount; ++i) {
            auto &frame = _stream->getFrame(_nextFrame++);
            fillBuffer(frame, _buffers[i]);
        }
        alSourceQueueBuffers(_source, bufferCount, &_buffers[0]);
    } else {
        auto &frame = _stream->getFrame(0);
        fillBuffer(frame, _buffers[0]);
        alSourcei(_source, AL_BUFFER, _buffers[0]);
        alSourcei(_source, AL_LOOPING, _loop);
    }

    _inited = true;
}

void AudioSource::deinit() {
    if (!_inited) {
        return;
    }
    checkMainThread();
    if (_source) {
        alSourceStop(_source);
        alDeleteSources(1, &_source);
        _source = 0;
    }
    if (!_buffers.empty()) {
        alDeleteBuffers(static_cast<int>(_buffers.size()), &_buffers[0]);
        _buffers.clear();
    }
    _inited = false;
}

void AudioSource::update() {
    if (!_source) {
        return;
    }
    if (!_streaming) {
        ALint state = 0;
        alGetSourcei(_source, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            _playing = false;
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
            auto &frame = _stream->getFrame(_nextFrame++);
            fillBuffer(frame, _buffers[_nextBuffer]);
            alSourceQueueBuffers(_source, 1, &_buffers[_nextBuffer]);
        }
        _nextBuffer = (_nextBuffer + 1) % static_cast<int>(_buffers.size());
    }
    ALint queued = 0;
    alGetSourcei(_source, AL_BUFFERS_QUEUED, &queued);
    if (queued == 0) {
        _playing = false;
    }
}

void AudioSource::play() {
    if (!_source) {
        return;
    }
    alSourcePlay(_source);
    _playing = true;
}

void AudioSource::stop() {
    if (_source) {
        alSourceStop(_source);
    }
    _playing = false;
}

float AudioSource::duration() const {
    return _stream->duration();
}

void AudioSource::setPosition(glm::vec3 position) {
    if (_position == position) {
        return;
    }
    if (_source && _positional) {
        alSource3f(_source, AL_POSITION, position.x, position.y, position.z);
    }
    _position = std::move(position);
}

} // namespace audio

} // namespace reone
