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

#include "reone/audio/di/module.h"
#include "reone/graphics/di/module.h"

#include "../movies.h"

#include "services.h"

namespace reone {

namespace movie {

class MovieModule : boost::noncopyable {
public:
    MovieModule(std::filesystem::path gamePath,
                graphics::GraphicsModule &graphics,
                audio::AudioModule &audio) :
        _gamePath(gamePath),
        _graphics(graphics),
        _audio(audio) {
    }

    ~MovieModule() { deinit(); }

    void init();
    void deinit();

    Movies &movies() { return *_movies; }

    MovieServices &services() { return *_services; }

private:
    std::filesystem::path _gamePath;
    graphics::GraphicsModule &_graphics;
    audio::AudioModule &_audio;

    std::unique_ptr<Movies> _movies;

    std::unique_ptr<MovieServices> _services;
};

} // namespace movie

} // namespace reone
