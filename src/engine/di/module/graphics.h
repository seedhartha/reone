/*
 * Copyright (c) 2020-2022 The reone project contributors
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
#include "../../graphics/fonts.h"
#include "../../graphics/lipanimations.h"
#include "../../graphics/meshes.h"
#include "../../graphics/models.h"
#include "../../graphics/options.h"
#include "../../graphics/pipeline.h"
#include "../../graphics/services.h"
#include "../../graphics/shaders.h"
#include "../../graphics/textures.h"
#include "../../graphics/uniforms.h"
#include "../../graphics/walkmeshes.h"
#include "../../graphics/window.h"

namespace reone {

namespace engine {

class ResourceModule;

class GraphicsModule : boost::noncopyable {
public:
    GraphicsModule(graphics::GraphicsOptions &options, ResourceModule &resource) :
        _options(options),
        _resource(resource) {
    }

    ~GraphicsModule() { deinit(); }

    void init();
    void deinit();

    graphics::Fonts &fonts() { return *_fonts; }
    graphics::GraphicsContext &graphicsContext() { return *_graphicsContext; }
    graphics::LipAnimations &lipAnimations() { return *_lipAnimations; }
    graphics::Meshes &meshes() { return *_meshes; }
    graphics::Models &models() { return *_models; }
    graphics::Pipeline &pipeline() { return *_pipeline; }
    graphics::Shaders &shaders() { return *_shaders; }
    graphics::Textures &textures() { return *_textures; }
    graphics::Uniforms &uniforms() { return *_uniforms; }
    graphics::Walkmeshes &walkmeshes() { return *_walkmeshes; }
    graphics::Window &window() { return *_window; }

    graphics::GraphicsServices &services() { return *_services; }

private:
    graphics::GraphicsOptions &_options;
    ResourceModule &_resource;

    std::unique_ptr<graphics::Fonts> _fonts;
    std::unique_ptr<graphics::GraphicsContext> _graphicsContext;
    std::unique_ptr<graphics::LipAnimations> _lipAnimations;
    std::unique_ptr<graphics::Meshes> _meshes;
    std::unique_ptr<graphics::Models> _models;
    std::unique_ptr<graphics::Pipeline> _pipeline;
    std::unique_ptr<graphics::Shaders> _shaders;
    std::unique_ptr<graphics::Textures> _textures;
    std::unique_ptr<graphics::Uniforms> _uniforms;
    std::unique_ptr<graphics::Walkmeshes> _walkmeshes;
    std::unique_ptr<graphics::Window> _window;

    std::unique_ptr<graphics::GraphicsServices> _services;
};

} // namespace engine

} // namespace reone
