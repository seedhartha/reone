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

    if (_colorBufferCount > 0) {
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
    }

    glGenTextures(1, &_depthBuffer);
    glBindTexture(GL_TEXTURE_2D, _depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    for (int i = 0; i < _colorBufferCount; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _colorBuffers[i], 0);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw runtime_error("Control: framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _inited = true;
}

void Framebuffer::deinit() {
    if (!_inited) return;

    glDeleteFramebuffers(1, &_framebuffer);

    if (_colorBufferCount > 0) {
        glDeleteTextures(_colorBufferCount, &_colorBuffers[0]);
    }
    glDeleteTextures(1, &_depthBuffer);

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

void Framebuffer::bindDepthBuffer() const {
    glBindTexture(GL_TEXTURE_2D, _depthBuffer);
}

void Framebuffer::unbindColorBuffer() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::unbindDepthBuffer() const {
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
