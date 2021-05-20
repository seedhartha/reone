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

#include <stdexcept>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "stateutil.h"
#include "window.h"

using namespace std;

namespace reone {

namespace graphics {

Cursor::Cursor(shared_ptr<Texture> up, shared_ptr<Texture> down, Window *window, Shaders *shaders, Meshes *meshes) :
    _up(up), _down(down), _window(window), _shaders(shaders), _meshes(meshes) {

    if (!up) {
        throw invalid_argument("up must not be null");
    }
    if (!down) {
        throw invalid_argument("down must not be null");
    }
    if (!window) {
        throw invalid_argument("window must not be null");
    }
    if (!shaders) {
        throw invalid_argument("shaders must not be null");
    }
    if (!meshes) {
        throw invalid_argument("meshes must not be null");
    }
}

void Cursor::draw() {
    shared_ptr<Texture> texture(_pressed ? _down : _up);
    setActiveTextureUnit(TextureUnits::diffuseMap);
    texture->bind();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(static_cast<float>(_position.x), static_cast<float>(_position.y), 0.0f));
    transform = glm::scale(transform, glm::vec3(texture->width(), texture->height(), 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _window->getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    _shaders->activate(ShaderProgram::SimpleGUI, uniforms);
    _meshes->getQuad()->draw();
}

void Cursor::setPosition(const glm::ivec2 &position) {
    _position = position;
}

void Cursor::setPressed(bool pressed) {
    _pressed = pressed;
}

} // namespace graphics

} // namespace reone
