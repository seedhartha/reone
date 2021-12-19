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
            attachColor(static_cast<Texture &>(*_color1), 0);
        } else if (_color1->isRenderbuffer()) {
            attachColor(static_cast<Renderbuffer &>(*_color1), 0);
        }
    }
    if (_color2) {
        if (_color2->isTexture()) {
            attachColor(static_cast<Texture &>(*_color2), 1);
        } else if (_color2->isRenderbuffer()) {
            attachColor(static_cast<Renderbuffer &>(*_color2), 1);
        }
    }
    if (_depth) {
        if (_depth->isTexture()) {
            attachDepth(static_cast<Texture &>(*_depth));
        } else if (_depth->isRenderbuffer()) {
            attachDepth(static_cast<Renderbuffer &>(*_depth));
        }
    }
}

void Framebuffer::attachColor(const Texture &texture, int index) const {
    if (texture.isCubeMap()) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture.nameGL(), 0);
    } else {
        GLenum target = texture.isMultisample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target, texture.nameGL(), 0);
    }
}

void Framebuffer::attachColor(const Renderbuffer &renderbuffer, int index) const {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, renderbuffer.nameGL());
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

} // namespace graphics

} // namespace reone
