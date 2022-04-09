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

#include "../../audio/context.h"
#include "../../audio/files.h"
#include "../../audio/options.h"
#include "../../audio/player.h"
#include "../../audio/services.h"

namespace reone {

class ResourceModule;

class AudioModule : boost::noncopyable {
public:
    AudioModule(audio::AudioOptions &options, ResourceModule &resource) :
        _options(options),
        _resource(resource) {
    }

    ~AudioModule() { deinit(); }

    void init();
    void deinit();

    audio::AudioContext &audioContext() { return *_audioContext; }
    audio::AudioFiles &audioFiles() { return *_audioFiles; }
    audio::AudioPlayer &audioPlayer() { return *_audioPlayer; }

    audio::AudioServices &services() { return *_services; }

private:
    audio::AudioOptions &_options;
    ResourceModule &_resource;

    std::unique_ptr<audio::AudioContext> _audioContext;
    std::unique_ptr<audio::AudioFiles> _audioFiles;
    std::unique_ptr<audio::AudioPlayer> _audioPlayer;

    std::unique_ptr<audio::AudioServices> _services;
};

} // namespace reone
