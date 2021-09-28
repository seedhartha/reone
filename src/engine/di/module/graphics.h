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

#include "../../graphics/context.h"
#include "../../graphics/features.h"
#include "../../graphics/fonts.h"
#include "../../graphics/lip/lips.h"
#include "../../graphics/materials.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/model/models.h"
#include "../../graphics/options.h"
#include "../../graphics/pbribl.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/walkmesh/walkmeshes.h"
#include "../../graphics/window.h"

namespace reone {

namespace di {

class ResourceServices;

class GraphicsServices : boost::noncopyable {
public:
    GraphicsServices(graphics::GraphicsOptions options, ResourceServices &resource) :
        _options(std::move(options)),
        _resource(resource) {
    }

    void init();

    graphics::Context &context() { return *_context; }
    graphics::Features &features() { return *_features; }
    graphics::Fonts &fonts() { return *_fonts; }
    graphics::Lips &lips() { return *_lips; }
    graphics::Materials &materials() { return *_materials; }
    graphics::Meshes &meshes() { return *_meshes; }
    graphics::Models &models() { return *_models; }
    graphics::PBRIBL &pbrIbl() { return *_pbrIbl; }
    graphics::Shaders &shaders() { return *_shaders; }
    graphics::Textures &textures() { return *_textures; }
    graphics::Walkmeshes &walkmeshes() { return *_walkmeshes; }
    graphics::Window &window() { return *_window; }

private:
    graphics::GraphicsOptions _options;
    ResourceServices &_resource;

    std::unique_ptr<graphics::Context> _context;
    std::unique_ptr<graphics::Features> _features;
    std::unique_ptr<graphics::Fonts> _fonts;
    std::unique_ptr<graphics::Lips> _lips;
    std::unique_ptr<graphics::Materials> _materials;
    std::unique_ptr<graphics::Meshes> _meshes;
    std::unique_ptr<graphics::Models> _models;
    std::unique_ptr<graphics::PBRIBL> _pbrIbl;
    std::unique_ptr<graphics::Shaders> _shaders;
    std::unique_ptr<graphics::Textures> _textures;
    std::unique_ptr<graphics::Walkmeshes> _walkmeshes;
    std::unique_ptr<graphics::Window> _window;
};

} // namespace di

} // namespace reone
