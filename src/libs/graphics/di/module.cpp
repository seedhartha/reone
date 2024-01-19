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
    _context = std::make_unique<Context>(_options);
    _statistic = std::make_unique<Statistic>();
    _meshRegistry = std::make_unique<MeshRegistry>(*_statistic);
    _shaderRegistry = std::make_unique<ShaderRegistry>();
    _textureRegistry = std::make_unique<TextureRegistry>();
    _uniforms = std::make_unique<Uniforms>(*_context);
    _pbrTextures = std::make_unique<PBRTextures>(
        *_context,
        *_meshRegistry,
        *_shaderRegistry,
        *_uniforms);

    _services = std::make_unique<GraphicsServices>(
        *_context,
        *_meshRegistry,
        *_pbrTextures,
        *_shaderRegistry,
        *_statistic,
        *_textureRegistry,
        *_uniforms);

    _context->init();
    _meshRegistry->init();
    _textureRegistry->init();
    _uniforms->init();
}

void GraphicsModule::deinit() {
    _services.reset();

    _pbrTextures.reset();
    _uniforms.reset();
    _meshRegistry.reset();
    _textureRegistry.reset();
    _statistic.reset();
    _context.reset();
}

} // namespace graphics

} // namespace reone
