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
#include "../meshes.h"
#include "../pipeline.h"
#include "../shaders.h"
#include "../uniforms.h"
#include "../window.h"

#include "services.h"

namespace reone {

namespace graphics {

class GraphicsModule : boost::noncopyable {
public:
    GraphicsModule(graphics::GraphicsOptions &options) :
        _options(options) {
    }

    virtual ~GraphicsModule() { deinit(); }

    void init();
    void deinit();

    graphics::GraphicsContext &context() { return *_context; }
    graphics::Meshes &meshes() { return *_meshes; }
    graphics::Pipeline &pipeline() { return *_pipeline; }
    graphics::Shaders &shaders() { return *_shaders; }
    graphics::Uniforms &uniforms() { return *_uniforms; }
    graphics::IWindow &window() { return *_window; }

    graphics::GraphicsServices &services() { return *_services; }

protected:
    graphics::GraphicsOptions &_options;

    std::unique_ptr<graphics::GraphicsContext> _context;
    std::unique_ptr<graphics::Meshes> _meshes;
    std::unique_ptr<graphics::Pipeline> _pipeline;
    std::unique_ptr<graphics::Shaders> _shaders;
    std::unique_ptr<graphics::Uniforms> _uniforms;
    std::unique_ptr<graphics::IWindow> _window;

    std::unique_ptr<graphics::GraphicsServices> _services;

    virtual std::unique_ptr<graphics::IWindow> newWindow();
};

} // namespace graphics

} // namespace reone
