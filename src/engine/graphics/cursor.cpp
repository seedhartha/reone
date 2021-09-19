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

#include "cursor.h"

#include "../common/guardutil.h"
#include "../di/services/graphics.h"

#include "context.h"
#include "mesh/mesh.h"
#include "mesh/meshes.h"
#include "shader/shaders.h"
#include "texture/texture.h"
#include "window.h"

using namespace std;

namespace reone {

namespace graphics {

Cursor::Cursor(shared_ptr<Texture> up, shared_ptr<Texture> down, GraphicsServices &graphics) :
    _up(up),
    _down(down),
    _graphics(graphics) {

    ensureNotNull(up, "up");
    ensureNotNull(down, "down");
}

void Cursor::draw() {
    shared_ptr<Texture> texture(_pressed ? _down : _up);
    _graphics.context().setActiveTextureUnit(TextureUnits::diffuseMap);
    texture->bind();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(static_cast<float>(_position.x), static_cast<float>(_position.y), 0.0f));
    transform = glm::scale(transform, glm::vec3(texture->width(), texture->height(), 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _graphics.window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    _graphics.shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _graphics.meshes().quad().draw();
}

} // namespace graphics

} // namespace reone
