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

#include "reone/system/memorycache.h"

#include "font.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class Meshes;
class Shaders;
class Textures;
class Uniforms;
class Window;

class IFonts {
public:
    virtual ~IFonts() = default;
};

class Fonts : public IFonts, public MemoryCache<std::string, Font> {
public:
    Fonts(
        GraphicsContext &graphicsContext,
        Meshes &meshes,
        Shaders &shaders,
        Textures &textures,
        Uniforms &uniforms,
        Window &window) :
        MemoryCache(std::bind(&Fonts::doGet, this, std::placeholders::_1)),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures),
        _uniforms(uniforms),
        _window(window) {
    }

private:
    // Services

    GraphicsContext &_graphicsContext;
    Meshes &_meshes;
    Shaders &_shaders;
    Textures &_textures;
    Uniforms &_uniforms;
    Window &_window;

    // END Services

    std::shared_ptr<Font> doGet(std::string resRef);
};

} // namespace graphics

} // namespace reone
