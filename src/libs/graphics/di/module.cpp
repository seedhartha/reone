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

#include "reone/graphics/di/module.h"

namespace reone {

namespace graphics {

void GraphicsModule::init() {
    _window = newWindow();
    _context = std::make_unique<GraphicsContext>(_options);
    _meshes = std::make_unique<Meshes>();
    _textures = std::make_unique<Textures>(_options, _resource.resources());
    _models = std::make_unique<Models>(*_textures, _resource.resources());
    _walkmeshes = std::make_unique<Walkmeshes>(_resource.resources());
    _lips = std::make_unique<Lips>(_resource.resources());
    _uniforms = std::make_unique<Uniforms>();
    _shaders = std::make_unique<Shaders>(_options);
    _fonts = std::make_unique<Fonts>(*_context, *_meshes, *_shaders, *_textures, *_uniforms);
    _pipeline = std::make_unique<Pipeline>(_options, *_context, *_meshes, *_shaders, *_textures, *_uniforms);

    _services = std::make_unique<GraphicsServices>(
        *_fonts,
        *_context,
        *_lips,
        *_meshes,
        *_models,
        *_pipeline,
        *_shaders,
        *_textures,
        *_uniforms,
        *_walkmeshes,
        *_window);

    _context->init();
    _meshes->init();
    _textures->init();
    _uniforms->init();
    _shaders->init();
    _pipeline->init();
}

void GraphicsModule::deinit() {
    _services.reset();

    _pipeline.reset();
    _shaders.reset();
    _uniforms.reset();
    _textures.reset();
    _meshes.reset();
    _context.reset();
    _window.reset();
}

std::unique_ptr<IWindow> GraphicsModule::newWindow() {
    auto window = std::make_unique<Window>(_options);
    window->init();
    return window;
}

} // namespace graphics

} // namespace reone
