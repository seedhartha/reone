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

#include "reone/resource/di/module.h"

#include "../context.h"
#include "../fonts.h"
#include "../lipanimations.h"
#include "../meshes.h"
#include "../models.h"
#include "../pipeline.h"
#include "../shaders.h"
#include "../textures.h"
#include "../uniforms.h"
#include "../walkmeshes.h"
#include "../window.h"

#include "services.h"

namespace reone {

namespace graphics {

class GraphicsModule : boost::noncopyable {
public:
    GraphicsModule(graphics::GraphicsOptions &options, resource::ResourceModule &resource) :
        _options(options),
        _resource(resource) {
    }

    virtual ~GraphicsModule() { deinit(); }

    void init();
    void deinit();

    graphics::Fonts &fonts() { return *_fonts; }
    graphics::GraphicsContext &context() { return *_context; }
    graphics::LipAnimations &lips() { return *_lips; }
    graphics::Meshes &meshes() { return *_meshes; }
    graphics::Models &models() { return *_models; }
    graphics::Pipeline &pipeline() { return *_pipeline; }
    graphics::Shaders &shaders() { return *_shaders; }
    graphics::Textures &textures() { return *_textures; }
    graphics::Uniforms &uniforms() { return *_uniforms; }
    graphics::Walkmeshes &walkmeshes() { return *_walkmeshes; }
    graphics::IWindow &window() { return *_window; }

    graphics::GraphicsServices &services() { return *_services; }

protected:
    graphics::GraphicsOptions &_options;
    resource::ResourceModule &_resource;

    std::unique_ptr<graphics::Fonts> _fonts;
    std::unique_ptr<graphics::GraphicsContext> _context;
    std::unique_ptr<graphics::LipAnimations> _lips;
    std::unique_ptr<graphics::Meshes> _meshes;
    std::unique_ptr<graphics::Models> _models;
    std::unique_ptr<graphics::Pipeline> _pipeline;
    std::unique_ptr<graphics::Shaders> _shaders;
    std::unique_ptr<graphics::Textures> _textures;
    std::unique_ptr<graphics::Uniforms> _uniforms;
    std::unique_ptr<graphics::Walkmeshes> _walkmeshes;
    std::unique_ptr<graphics::IWindow> _window;

    std::unique_ptr<graphics::GraphicsServices> _services;

    virtual std::unique_ptr<graphics::IWindow> newWindow();
};

} // namespace graphics

} // namespace reone
