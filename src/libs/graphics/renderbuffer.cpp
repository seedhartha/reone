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

#include "reone/graphics/renderbuffer.h"

#include "reone/graphics/pixelutil.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

void Renderbuffer::init() {
    if (_inited) {
        return;
    }
    checkMainThread();
    glGenRenderbuffers(1, &_nameGL);
    bind();
    refresh();
    _inited = true;
}

void Renderbuffer::deinit() {
    if (!_inited) {
        return;
    }
    checkMainThread();
    glDeleteRenderbuffers(1, &_nameGL);
    _inited = false;
}

void Renderbuffer::bind() {
    glBindRenderbuffer(GL_RENDERBUFFER, _nameGL);
}

void Renderbuffer::unbind() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderbuffer::configure(int width, int height, PixelFormat format) {
    _width = width;
    _height = height;
    _pixelFormat = format;
}

void Renderbuffer::refresh() {
    glRenderbufferStorage(GL_RENDERBUFFER, getInternalPixelFormatGL(_pixelFormat), _width, _height);
}

} // namespace graphics

} // namespace reone
