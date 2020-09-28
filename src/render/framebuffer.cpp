/*
 * Copyright © 2020 Vsevolod Kremianskii
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

Framebuffer::Framebuffer(int w, int h) : _width(w), _height(h) {
}

Framebuffer::~Framebuffer() {
    deinit();
}

void Framebuffer::init() {
    if (_inited) return;

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw runtime_error("Control: framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _inited = true;
}

void Framebuffer::deinit() {
    if (!_inited) return;

    glDeleteFramebuffers(1, &_framebuffer);
    glDeleteTextures(1, &_texture);
    glDeleteRenderbuffers(1, &_renderbuffer);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, _texture);
}

void Framebuffer::unbindTexture() {
    glBindTexture(GL_TEXTURE_2D, _texture);
}

int Framebuffer::width() const {
    return _width;
}

int Framebuffer::height() const {
    return _height;
}

} // namespace render

} // namespace reone
