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

#include "../audio/source.h"
#include "../common/types.h"

#include "videostream.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class Meshes;
class Shaders;
class Texture;
class Textures;
class UniformBuffers;

} // namespace graphics

namespace audio {

class AudioPlayer;

}

namespace movie {

class BikReader;

class Movie {
public:
    Movie(
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::UniformBuffers &uniformBuffers,
        audio::AudioPlayer &audioPlayer) :
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures),
        _uniformBuffers(uniformBuffers),
        _audioPlayer(audioPlayer) {
    }

    void init();
    void deinit();

    void update(float dt);
    void draw();

    void finish() { _finished = true; }

    bool isFinished() const { return _finished; }

    void setVideoStream(std::shared_ptr<VideoStream> stream) { _videoStream = std::move(stream); }
    void setAudioStream(std::shared_ptr<audio::AudioStream> stream) { _audioStream = std::move(stream); }

private:
    int _width {0};
    int _height {0};

    float _time {0.0f};
    bool _finished {false};

    std::shared_ptr<VideoStream> _videoStream;
    std::shared_ptr<audio::AudioStream> _audioStream;

    std::shared_ptr<graphics::Texture> _texture;
    std::shared_ptr<audio::AudioSource> _audioSource;

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
