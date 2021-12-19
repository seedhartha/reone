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
    if (_inited) {
        return;
    }
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
    glDeleteFramebuffers(1, &_nameGL);
    _inited = false;
}

void Framebuffer::configure() {
    if (_color1) {
        if (_color1->isTexture()) {
            attachTexture(static_cast<Texture &>(*_color1), Attachment::Color1);
        } else if (_color1->isRenderbuffer()) {
            attachRenderbuffer(static_cast<Renderbuffer &>(*_color1), Attachment::Color1);
        }
    }
    if (_color2) {
        if (_color2->isTexture()) {
            attachTexture(static_cast<Texture &>(*_color2), Attachment::Color2);
        } else if (_color2->isRenderbuffer()) {
            attachRenderbuffer(static_cast<Renderbuffer &>(*_color2), Attachment::Color2);
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
        } else if (_depth->isRenderbuffer()) {
            attachRenderbuffer(static_cast<Renderbuffer &>(*_depthStencil), Attachment::DepthStencil);
        }
    }
}

static GLenum getAttachmentGL(Framebuffer::Attachment attachment) {
    switch (attachment) {
    case Framebuffer::Attachment::Color1:
        return GL_COLOR_ATTACHMENT0;
    case Framebuffer::Attachment::Color2:
        return GL_COLOR_ATTACHMENT1;
    case Framebuffer::Attachment::Depth:
        return GL_DEPTH_ATTACHMENT;
    case Framebuffer::Attachment::DepthStencil:
        return GL_DEPTH_STENCIL_ATTACHMENT;
    }
    throw invalid_argument("Unsupported framebuffer attachment: " + to_string(static_cast<int>(attachment)));
}

void Framebuffer::attachTexture(const Texture &texture, Attachment attachment) const {
    auto attachmentGL = getAttachmentGL(attachment);
    if (texture.isCubeMap()) {
        glFramebufferTexture(GL_FRAMEBUFFER, attachmentGL, texture.nameGL(), 0);
    } else {
        GLenum target = texture.isMultisample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentGL, target, texture.nameGL(), 0);
    }
}

void Framebuffer::attachRenderbuffer(const Renderbuffer &renderbuffer, Attachment attachment) const {
    auto attachmentGL = getAttachmentGL(attachment);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentGL, GL_RENDERBUFFER, renderbuffer.nameGL());
}

} // namespace graphics

} // namespace reone
