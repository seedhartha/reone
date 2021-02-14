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

namespace reone {

namespace render {

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
    if (_inited) {
        glBindRenderbuffer(GL_RENDERBUFFER, _id);
    }
}

void Renderbuffer::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

static GLenum getInternalFormatGL(Renderbuffer::PixelFormat format) {
    switch (format) {
        case Renderbuffer::PixelFormat::Depth:
            return GL_DEPTH_COMPONENT;
        case Renderbuffer::PixelFormat::RGB:
            return GL_RGB8;
        case Renderbuffer::PixelFormat::RGBA:
        default:
            return GL_RGBA8;
    }
}

void Renderbuffer::configure(int w, int h, PixelFormat format) {
    glRenderbufferStorage(GL_RENDERBUFFER, getInternalFormatGL(format), w, h);
}

} // namespace render

} // namespace reone
