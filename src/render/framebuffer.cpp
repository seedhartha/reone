/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "framebuffer.h"

#include <stdexcept>

#include "GL/glew.h"

using namespace std;

namespace reone {

namespace render {

Framebuffer::Framebuffer(int w, int h, int colorBufferCount) : _width(w), _height(h), _colorBufferCount(colorBufferCount) {
}

Framebuffer::~Framebuffer() {
    deinit();
}

void Framebuffer::init() {
    if (_inited) return;

    _colorBuffers.resize(_colorBufferCount);
    glGenTextures(_colorBufferCount, &_colorBuffers[0]);

    for (int i = 0; i < _colorBufferCount; ++i) {
        glBindTexture(GL_TEXTURE_2D, _colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glGenRenderbuffers(1, &_depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    for (int i = 0; i < _colorBufferCount; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _colorBuffers[i], 0);
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw runtime_error("Control: framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _inited = true;
}

void Framebuffer::deinit() {
    if (!_inited) return;

    glDeleteFramebuffers(1, &_framebuffer);
    glDeleteTextures(_colorBufferCount, &_colorBuffers[0]);
    glDeleteRenderbuffers(1, &_depthBuffer);

    _inited = false;
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindColorBuffer(int n) const {
    glBindTexture(GL_TEXTURE_2D, _colorBuffers[n]);
}

void Framebuffer::unbindColorBuffer() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

int Framebuffer::width() const {
    return _width;
}

int Framebuffer::height() const {
    return _height;
}

} // namespace render

} // namespace reone
