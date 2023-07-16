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

#include "reone/graphics/uniformbuffer.h"

#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

void UniformBuffer::init() {
    if (_inited) {
        return;
    }
    checkMainThread();
    glGenBuffers(1, &_nameGL);
    glBindBuffer(GL_UNIFORM_BUFFER, _nameGL);
    glBufferData(GL_UNIFORM_BUFFER, _size, _data, GL_DYNAMIC_DRAW);
    _inited = true;
}

void UniformBuffer::deinit() {
    if (!_inited) {
        return;
    }
    checkMainThread();
    glDeleteBuffers(1, &_nameGL);
    _inited = false;
}

void UniformBuffer::bind(int index) {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, _nameGL);
}

void UniformBuffer::unbind(int index) {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, 0);
}

void UniformBuffer::refresh() {
    glBufferSubData(GL_UNIFORM_BUFFER, 0, _size, _data);
}

} // namespace graphics

} // namespace reone
