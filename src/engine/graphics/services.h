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

#include <boost/noncopyable.hpp>

namespace reone {

namespace graphics {

class Meshes;
class Textures;
class Materials;
class Models;
class Walkmeshes;
class Lips;
class Fonts;
class Shaders;
class PBRIBL;
class Window;

class GraphicsServices : boost::noncopyable {
public:
    GraphicsServices(
        Meshes &meshes,
        Textures &textures,
        Materials &materials,
        Models &models,
        Walkmeshes &walkmeshes,
        Lips &lips,
        Fonts &fonts,
        Shaders &shaders,
        PBRIBL &pbrIbl,
        Window &window
    ) :
        _meshes(meshes),
        _textures(textures),
        _materials(materials),
        _models(models),
        _walkmeshes(walkmeshes),
        _lips(lips),
        _fonts(fonts),
        _shaders(shaders),
        _pbrIbl(pbrIbl),
        _window(window) {
    }

    Meshes &meshes() { return _meshes; }
    Textures &textures() { return _textures; }
    Materials &materials() { return _materials; }
    Models &models() { return _models; }
    Walkmeshes &walkmeshes() { return _walkmeshes; }
    Lips &lips() { return _lips; }
    Fonts &fonts() { return _fonts; }
    Shaders &shaders() { return _shaders; }
    PBRIBL &pbrIbl() { return _pbrIbl; }
    Window &window() { return _window; }

private:
    Meshes &_meshes;
    Textures &_textures;
    Materials &_materials;
    Models &_models;
    Walkmeshes &_walkmeshes;
    Lips &_lips;
    Fonts &_fonts;
    Shaders &_shaders;
    PBRIBL &_pbrIbl;
    Window &_window;
};

} // namespace graphics

} // namespace reone
