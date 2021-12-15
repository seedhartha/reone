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

#include "renderbuffer.h"
#include "texture.h"

using namespace std;

namespace reone {

namespace graphics {

void Framebuffer::init() {
    if (!_nameGL) {
        glGenFramebuffers(1, &_nameGL);
    }
}

void Framebuffer::deinit() {
    if (_nameGL) {
        glDeleteFramebuffers(1, &_nameGL);
        _nameGL = 0;
    }
}

static GLenum getTargetGL(FramebufferTarget target) {
    switch (target) {
    case FramebufferTarget::Read:
        return GL_READ_FRAMEBUFFER;
    case FramebufferTarget::Draw:
        return GL_DRAW_FRAMEBUFFER;
    }
}

void Framebuffer::bind(FramebufferTarget target) {
    glBindFramebuffer(getTargetGL(target), _nameGL);
}

void Framebuffer::unbind(FramebufferTarget target) {
    glBindFramebuffer(getTargetGL(target), 0);
}

void Framebuffer::attachColor(const Texture &texture, int index, int mip) const {
    if (texture.isCubeMap()) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture.nameGL(), mip);
    } else {
        GLenum target = texture.isMultisample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target, texture.nameGL(), mip);
    }
}

void Framebuffer::attachColor(const Renderbuffer &renderbuffer, int index) const {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, renderbuffer.nameGL());
}

void Framebuffer::attachCubeMapFaceAsColor(const Texture &texture, CubeMapFace face, int index, int mip) const {
    if (texture.isCubeMap()) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(face), texture.nameGL(), mip);
    }
}

void Framebuffer::attachDepth(const Texture &texture) const {
    if (texture.isCubeMap()) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.nameGL(), 0);
    } else {
        GLenum target = texture.isMultisample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, texture.nameGL(), 0);
    }
}

void Framebuffer::attachDepth(const Renderbuffer &renderbuffer) const {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.nameGL());
}

void Framebuffer::blit(int width, int height) {
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Framebuffer::disableReadBuffer() {
    glReadBuffer(GL_NONE);
}

void Framebuffer::disableDrawBuffer() {
    glDrawBuffer(GL_NONE);
}

void Framebuffer::setReadBuffer(int colorIdx) {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + colorIdx);
}

void Framebuffer::setDrawBuffer(int colorIdx) {
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + colorIdx);
}

void Framebuffer::setDrawBuffers(int numColors) {
    static constexpr GLenum colors[] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(numColors, colors);
}

} // namespace graphics

} // namespace reone
