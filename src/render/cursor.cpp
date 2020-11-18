/*
 * Copyright (c) 2020 The reone project contributors
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

#include "mesh/quad.h"
#include "shaders.h"
#include "texture.h"

using namespace std;

namespace reone {

namespace render {

Cursor::Cursor(const shared_ptr<Texture> &up, const shared_ptr<Texture> &down) : _up(up), _down(down) {
    if (!up) {
        throw invalid_argument("up must not be null");
    }
    if (!down) {
        throw invalid_argument("down must not be null");
    }
}

void Cursor::render() const {
    shared_ptr<Texture> texture(_pressed ? _down : _up);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(static_cast<float>(_position.x), static_cast<float>(_position.y), 0.0f));
    transform = glm::scale(transform, glm::vec3(texture->width(), texture->height(), 1.0f));

    LocalUniforms locals;
    locals.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    texture->bind(0);

    Quad::getDefault().renderTriangles();
}

void Cursor::setPosition(const glm::ivec2 &position) {
    _position = position;
}

void Cursor::setPressed(bool pressed) {
    _pressed = pressed;
}

} // namespace render

} // namespace reone
