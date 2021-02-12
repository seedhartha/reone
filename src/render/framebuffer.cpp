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
#include "SDL2/SDL_opengl.h"

using namespace std;

namespace reone {

namespace render {

void Framebuffer::init() {
    if (!_inited) {
        glGenFramebuffers(1, &_framebuffer);
        _inited = true;
    }
}

Framebuffer::~Framebuffer() {
    deinit();
}

void Framebuffer::deinit() {
    if (_inited) {
        glDeleteFramebuffers(1, &_framebuffer);
        _inited = false;
    }
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachColor(const Texture &texture, int index, int mip) const {
    if (texture.isCubeMap()) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture.textureId(), mip);
    } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.textureId(), mip);
    }
}

void Framebuffer::attachCubeMapFaceAsColor(const Texture &texture, CubeMapFace face, int index, int mip) const {
    if (texture.isCubeMap()) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(face), texture.textureId(), mip);
    }
}

void Framebuffer::attachDepth(const Texture &texture) const {
    if (texture.isCubeMap()) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.textureId(), 0);
    } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.textureId(), 0);
    }
}

void Framebuffer::checkCompleteness() {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw logic_error("Framebuffer is not complete");
    }
}

} // namespace render

} // namespace reone
