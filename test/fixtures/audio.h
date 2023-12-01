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

#include "reone/audio/context.h"
#include "reone/audio/di/services.h"

namespace reone {

namespace audio {

class MockAudioContext : public IAudioContext, boost::noncopyable {
public:
    MOCK_METHOD(void, setListenerPosition, (glm::vec3 position), (override));
};

class TestAudioModule : boost::noncopyable {
public:
    void init() {
        _context = std::make_unique<MockAudioContext>();

        _services = std::make_unique<AudioServices>(*_context);
    }

    AudioServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockAudioContext> _context;

    std::unique_ptr<AudioServices> _services;
};

} // namespace audio

} // namespace reone
