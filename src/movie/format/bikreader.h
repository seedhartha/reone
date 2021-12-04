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

namespace reone {

namespace graphics {

class Context;
class Meshes;
class Shaders;

} // namespace graphics

namespace audio {

class AudioPlayer;

}

namespace movie {

class Movie;

/**
 * Encapsulates loading Bink Video files using FFmpeg.
 *
 * http://dranger.com/ffmpeg/ffmpeg.html
 */
class BikReader {
public:
    BikReader(
        boost::filesystem::path path,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        audio::AudioPlayer &audioPlayer) :
        _path(std::move(path)),
        _context(context),
        _meshes(meshes),
        _shaders(shaders),
        _audioPlayer(audioPlayer) {
    }

    void load();

    std::shared_ptr<Movie> video() const { return _video; }

private:
    boost::filesystem::path _path;

    std::shared_ptr<Movie> _video;

    // Services

    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;

    audio::AudioPlayer &_audioPlayer;

    // END Services
};

} // namespace movie

} // namespace reone
