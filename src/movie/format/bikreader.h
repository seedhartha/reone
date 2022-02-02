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

class GraphicsContext;
class Meshes;
class Shaders;
class Textures;
class UniformBuffers;

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
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::UniformBuffers &uniformBuffers,
        audio::AudioPlayer &audioPlayer) :
        _path(std::move(path)),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures),
        _uniformBuffers(uniformBuffers),
        _audioPlayer(audioPlayer) {
    }

    void load();

    std::shared_ptr<Movie> movie() const { return _movie; }

private:
    boost::filesystem::path _path;

    std::shared_ptr<Movie> _movie;

    // Services

    graphics::GraphicsContext &_graphicsContext;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;
    graphics::UniformBuffers &_uniformBuffers;

    audio::AudioPlayer &_audioPlayer;

    // END Services
};

} // namespace movie

} // namespace reone
