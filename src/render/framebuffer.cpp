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
        for (int i = 0; i < _colorBufferCount; ++i) {
            auto texture = make_unique<Texture>("color" + to_string(i), TextureType::ColorBuffer, _width, _height);
            texture->init();
            texture->clearPixels(PixelFormat::RGBA);

            _colorBuffers.push_back(move(texture));
        }
    }

    _depthBuffer = make_unique<Texture>("depth", TextureType::DepthBuffer, _width, _height);
    _depthBuffer->init();
    _depthBuffer->clearPixels(PixelFormat::Depth);

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    for (int i = 0; i < _colorBufferCount; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _colorBuffers[i]->textureId(), 0);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthBuffer->textureId(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw runtime_error("Framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _inited = true;
}

void Framebuffer::deinit() {
    if (!_inited) return;

    glDeleteFramebuffers(1, &_framebuffer);

    for (auto &buffer : _colorBuffers) {
        buffer->deinit();
    }
    _colorBuffers.clear();
    _depthBuffer->deinit();

    _inited = false;
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindColorBuffer(int n) const {
    _colorBuffers[n]->bind();
}

void Framebuffer::bindDepthBuffer() const {
    _depthBuffer->bind();
}

void Framebuffer::unbindColorBuffer(int n) const {
    _colorBuffers[n]->unbind();
}

void Framebuffer::unbindDepthBuffer() const {
    _depthBuffer->unbind();
}

int Framebuffer::width() const {
    return _width;
}

int Framebuffer::height() const {
    return _height;
}

} // namespace render

} // namespace reone
