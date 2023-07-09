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

namespace graphics {

struct GraphicsServices;

}

namespace audio {

struct AudioServices;

}

namespace movie {

class Movie;

class BikReader {
public:
    BikReader(
        std::filesystem::path path,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc) :
        _path(std::move(path)),
        _graphicsSvc(graphicsSvc),
        _audioSvc(audioSvc) {
    }

    void load();

    std::shared_ptr<Movie> movie() const { return _movie; }

private:
    std::filesystem::path _path;
    graphics::GraphicsServices &_graphicsSvc;
    audio::AudioServices &_audioSvc;

    std::shared_ptr<Movie> _movie;
};

} // namespace movie

} // namespace reone
