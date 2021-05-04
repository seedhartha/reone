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

#include "renderbuffer.h"

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "pixelutil.h"

namespace reone {

namespace graphics {

void Renderbuffer::init() {
    if (!_inited) {
        glGenRenderbuffers(1, &_id);
        _inited = true;
    }
}

Renderbuffer::~Renderbuffer() {
    deinit();
}

void Renderbuffer::deinit() {
    if (_inited) {
        glDeleteRenderbuffers(1, &_id);
        _inited = false;
    }
}

void Renderbuffer::bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, _id);
}

void Renderbuffer::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderbuffer::configure(int w, int h, PixelFormat format) {
    glRenderbufferStorage(GL_RENDERBUFFER, getInternalPixelFormatGL(format), w, h);
}

} // namespace graphics

} // namespace reone
