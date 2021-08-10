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

#include "options.h"

#include "context.h"
#include "features.h"
#include "fonts.h"
#include "lip/lips.h"
#include "materials.h"
#include "mesh/meshes.h"
#include "model/models.h"
#include "pbribl.h"
#include "shader/shaders.h"
#include "texture/textures.h"
#include "walkmesh/walkmeshes.h"
#include "window.h"

namespace reone {

namespace resource {

class ResourceServices;

}

namespace graphics {

class GraphicsServices : boost::noncopyable {
public:
    GraphicsServices(GraphicsOptions options, resource::ResourceServices &resource);

    void init();

    Context &context() { return *_context; }
    Features &features() { return *_features; }
    Fonts &fonts() { return *_fonts; }
    Lips &lips() { return *_lips; }
    Materials &materials() { return *_materials; }
    Meshes &meshes() { return *_meshes; }
    Models &models() { return *_models; }
    PBRIBL &pbrIbl() { return *_pbrIbl; }
    Shaders &shaders() { return *_shaders; }
    Textures &textures() { return *_textures; }
    Walkmeshes &walkmeshes() { return *_walkmeshes; }
    Window &window() { return *_window; }

private:
    GraphicsOptions _options;
    resource::ResourceServices &_resource;

    std::unique_ptr<Context> _context;
    std::unique_ptr<Features> _features;
    std::unique_ptr<Fonts> _fonts;
    std::unique_ptr<Lips> _lips;
    std::unique_ptr<Materials> _materials;
    std::unique_ptr<Meshes> _meshes;
    std::unique_ptr<Models> _models;
    std::unique_ptr<PBRIBL> _pbrIbl;
    std::unique_ptr<Shaders> _shaders;
    std::unique_ptr<Textures> _textures;
    std::unique_ptr<Walkmeshes> _walkmeshes;
    std::unique_ptr<Window> _window;
};

} // namespace graphics

} // namespace reone
