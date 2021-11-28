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

class Context;
class Features;
class Materials;
class Meshes;
class PBRIBL;
class Shaders;
class Textures;

} // namespace graphics

namespace audio {

class AudioPlayer;

}

namespace scene {

class SceneGraphs {
public:
    SceneGraphs(
        graphics::GraphicsOptions options,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::PBRIBL &pbrIbl,
        graphics::Shaders &shaders,
        graphics::Textures &textures) :
        _options(std::move(options)),
        _audioPlayer(audioPlayer),
        _context(context),
        _features(features),
        _materials(materials),
        _meshes(meshes),
        _pbrIbl(pbrIbl),
        _shaders(shaders),
        _textures(textures) {
    }

    void add(std::string name);

    SceneGraph &get(const std::string &name);

    const std::unordered_map<std::string, std::unique_ptr<SceneGraph>> &scenes() const { return _scenes; }

private:
    graphics::GraphicsOptions _options;

    audio::AudioPlayer &_audioPlayer;
    graphics::Context &_context;
    graphics::Features &_features;
    graphics::Materials &_materials;
    graphics::Meshes &_meshes;
    graphics::PBRIBL &_pbrIbl;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;

    std::unordered_map<std::string, std::unique_ptr<SceneGraph>> _scenes;
};

} // namespace scene

} // namespace reone
