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

#include "../common/memorycache.h"

#include "font.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class Meshes;
class Shaders;
class Textures;
class UniformBuffers;
class Window;

class Fonts : public MemoryCache<std::string, Font> {
public:
    Fonts(
        GraphicsContext &graphicsContext,
        Meshes &meshes,
        Shaders &shaders,
        Textures &textures,
        UniformBuffers &uniformBuffers,
        Window &window) :
        MemoryCache(std::bind(&Fonts::doGet, this, std::placeholders::_1)),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures),
        _uniformBuffers(uniformBuffers),
        _window(window) {
    }

private:
    // Services

    GraphicsContext &_graphicsContext;
    Meshes &_meshes;
    Shaders &_shaders;
    Textures &_textures;
    UniformBuffers &_uniformBuffers;
    Window &_window;

    // END Services

    std::shared_ptr<Font> doGet(std::string resRef);
};

} // namespace graphics

} // namespace reone
