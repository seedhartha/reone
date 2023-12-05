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

#include "reone/graphics/cursor.h"

#include "reone/graphics/context.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"

namespace reone {

namespace graphics {

void Cursor::draw() {
    std::shared_ptr<Texture> texture(_pressed ? _down : _up);
    _graphicsContext.bind(*texture);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(static_cast<float>(_position.x), static_cast<float>(_position.y), 0.0f));
    transform = glm::scale(transform, glm::vec3(texture->width(), texture->height(), 1.0f));

    _uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _window.getOrthoProjection();
    });
    _uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::texture2d));
    _graphicsContext.withBlending(BlendMode::Normal, [this]() {
        _meshRegistry.get(MeshName::quad).draw();
    });
}

} // namespace graphics

} // namespace reone
