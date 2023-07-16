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

#include "reone/graphics/framebuffer.h"

#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/texture.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

void Framebuffer::init() {
    if (_inited) {
        return;
    }
    checkMainThread();
    glGenFramebuffers(1, &_nameGL);
    glBindFramebuffer(GL_FRAMEBUFFER, _nameGL);
    configure();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _inited = true;
}

void Framebuffer::deinit() {
    if (!_inited) {
        return;
    }
    checkMainThread();
    glDeleteFramebuffers(1, &_nameGL);
    _inited = false;
}

void Framebuffer::configure() {
    for (size_t i = 0; i < _colors.size(); ++i) {
        auto &color = _colors[i];
        if (color->isTexture()) {
            attachTexture(static_cast<Texture &>(*color), Attachment::Color, i);
        } else if (color->isRenderbuffer()) {
            attachRenderbuffer(static_cast<Renderbuffer &>(*color), Attachment::Color, i);
        }
    }
    if (_depth) {
        if (_depth->isTexture()) {
            attachTexture(static_cast<Texture &>(*_depth), Attachment::Depth);
        } else if (_depth->isRenderbuffer()) {
            attachRenderbuffer(static_cast<Renderbuffer &>(*_depth), Attachment::Depth);
        }
    }
    if (_depthStencil) {
        if (_depthStencil->isTexture()) {
            attachTexture(static_cast<Texture &>(*_depthStencil), Attachment::DepthStencil);
        } else if (_depthStencil->isRenderbuffer()) {
            attachRenderbuffer(static_cast<Renderbuffer &>(*_depthStencil), Attachment::DepthStencil);
        }
    }
}

static GLenum getAttachmentGL(Framebuffer::Attachment attachment, int index = 0) {
    switch (attachment) {
    case Framebuffer::Attachment::Color:
        return GL_COLOR_ATTACHMENT0 + index;
    case Framebuffer::Attachment::Depth:
        return GL_DEPTH_ATTACHMENT;
    case Framebuffer::Attachment::DepthStencil:
        return GL_DEPTH_STENCIL_ATTACHMENT;
    }
    throw std::invalid_argument("Invalid framebuffer attachment: " + std::to_string(static_cast<int>(attachment)));
}

void Framebuffer::attachTexture(const Texture &texture, Attachment attachment, int index) const {
    auto attachmentGL = getAttachmentGL(attachment, index);
    if (texture.isCubemap() || texture.is2DArray()) {
        glFramebufferTexture(GL_FRAMEBUFFER, attachmentGL, texture.nameGL(), 0);
    } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentGL, GL_TEXTURE_2D, texture.nameGL(), 0);
    }
}

void Framebuffer::attachRenderbuffer(const Renderbuffer &renderbuffer, Attachment attachment, int index) const {
    auto attachmentGL = getAttachmentGL(attachment, index);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentGL, GL_RENDERBUFFER, renderbuffer.nameGL());
}

} // namespace graphics

} // namespace reone
