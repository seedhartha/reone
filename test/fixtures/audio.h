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

#include <gmock/gmock.h>

#include "reone/audio/di/services.h"
#include "reone/audio/format/mp3reader.h"

namespace reone {

namespace audio {

class MockMp3Reader : public Mp3Reader {
public:
    MOCK_METHOD(void, load, (IInputStream & stream), (override));
};

class MockMp3ReaderFactory : public IMp3ReaderFactory, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Mp3Reader>, create, (), (override));
};

class MockAudioContext : public IAudioContext, boost::noncopyable {
public:
    void setListenerPosition(glm::vec3 position) override {}
};

class MockAudioFiles : public IAudioFiles, boost::noncopyable {
public:
    void invalidate() override {}

    std::shared_ptr<AudioStream> get(const std::string &key) override {
        return nullptr;
    }
};

class MockAudioPlayer : public IAudioPlayer, boost::noncopyable {
public:
    std::shared_ptr<AudioSource> play(const std::string &resRef, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f)) override {
        return nullptr;
    }

    std::shared_ptr<AudioSource> play(std::shared_ptr<AudioStream> stream, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f)) override {
        return nullptr;
    }
};

class TestAudioModule : boost::noncopyable {
public:
    void init() {
        _context = std::make_unique<MockAudioContext>();
        _files = std::make_unique<MockAudioFiles>();
        _player = std::make_unique<MockAudioPlayer>();

        _services = std::make_unique<AudioServices>(
            *_context,
            *_files,
            *_player);
    }

    AudioServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockAudioContext> _context;
    std::unique_ptr<MockAudioFiles> _files;
    std::unique_ptr<MockAudioPlayer> _player;

    std::unique_ptr<AudioServices> _services;
};

} // namespace audio

} // namespace reone
