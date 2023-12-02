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
    _meshRegistry = std::make_unique<MeshRegistry>();
    _shaderRegistry = std::make_unique<ShaderRegistry>();
    _textureRegistry = std::make_unique<TextureRegistry>();
    _uniforms = std::make_unique<Uniforms>();
    _pipeline = std::make_unique<Pipeline>(
        _options,
        *_context,
        *_meshRegistry,
        *_shaderRegistry,
        *_textureRegistry,
        *_uniforms);

    _services = std::make_unique<GraphicsServices>(
        *_context,
        *_meshRegistry,
        *_pipeline,
        *_shaderRegistry,
        *_textureRegistry,
        *_uniforms,
        *_window);

    _context->init();
    _meshRegistry->init();
    _textureRegistry->init();
    _uniforms->init();
    _pipeline->init();
}

void GraphicsModule::deinit() {
    _services.reset();

    _pipeline.reset();
    _uniforms.reset();
    _meshRegistry.reset();
    _textureRegistry.reset();
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
