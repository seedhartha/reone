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

#include "../graphics/options.h"

#include "graph.h"

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

namespace scene {

class SceneGraphs {
public:
    SceneGraphs(
        graphics::GraphicsOptions &options,
        audio::AudioPlayer &audioPlayer,
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::UniformBuffers &uniformBuffers) :
        _options(options),
        _audioPlayer(audioPlayer),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures),
        _uniformBuffers(uniformBuffers) {
    }

    void add(std::string name);

    SceneGraph &get(const std::string &name);

    const std::unordered_map<std::string, std::unique_ptr<SceneGraph>> &scenes() const { return _scenes; }

private:
    graphics::GraphicsOptions &_options;

    std::unordered_map<std::string, std::unique_ptr<SceneGraph>> _scenes;

    // Services

    graphics::GraphicsContext &_graphicsContext;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;
    graphics::UniformBuffers &_uniformBuffers;

    audio::AudioPlayer &_audioPlayer;

    // END Services
};

} // namespace scene

} // namespace reone
