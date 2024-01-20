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

#include "../context.h"
#include "../mixer.h"
#include "../options.h"

#include "services.h"

namespace reone {

namespace audio {

class AudioModule : boost::noncopyable {
public:
    AudioModule(AudioOptions &options) :
        _options(options) {
    }

    ~AudioModule() { deinit(); }

    void init();
    void deinit();

    Context &context() { return *_context; }
    AudioMixer &mixer() { return *_mixer; }

    AudioServices &services() { return *_services; }

private:
    AudioOptions &_options;

    std::unique_ptr<Context> _context;
    std::unique_ptr<AudioMixer> _mixer;

    std::unique_ptr<AudioServices> _services;
};

} // namespace audio

} // namespace reone
