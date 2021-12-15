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

#include "context.h"

using namespace std;

namespace reone {

namespace graphics {

void Context::init() {
    if (_inited) {
        return;
    }
    glGetIntegerv(GL_VIEWPORT, &_viewport[0]);
    setBlendMode(BlendMode::Default);
    if (_options.aaSamples > 1) {
        glEnable(GL_MULTISAMPLE);
    }
    _inited = true;
}

void Context::deinit() {
    if (!_inited) {
        return;
    }
    unbindReadFramebuffer();
    unbindDrawFramebuffer();
    unbindRenderbuffer();

    for (size_t i = 0; i < _boundTextures.size(); ++i) {
        unbindTexture(static_cast<int>(i));
    }
    _boundTextures.clear();

    for (size_t i = 0; i < _boundUniformBuffers.size(); ++i) {
        unbindUniformBuffer(static_cast<int>(i));
    }
    _boundUniformBuffers.clear();

    _inited = false;
}

void Context::clear(int mask) {
    int glMask = 0;
    if (mask & ClearBuffers::color) {
        glMask |= GL_COLOR_BUFFER_BIT;
    }
    if (mask & ClearBuffers::depth) {
        glMask |= GL_DEPTH_BUFFER_BIT;
    }
    if (mask & ClearBuffers::stencil) {
        glMask |= GL_STENCIL_BUFFER_BIT;
    }
    glClear(glMask);
}

void Context::useShaderProgram(shared_ptr<ShaderProgram> program) {
    if (_shaderProgram == program) {
        return;
    }
    program->use();
    _shaderProgram = move(program);
}

void Context::setViewport(glm::ivec4 viewport) {
    if (_viewport == viewport)
        return;

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    _viewport = move(viewport);
}

void Context::setDepthTestEnabled(bool enabled) {
    if (_depthTest == enabled)
        return;

    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    _depthTest = enabled;
}

void Context::setBackFaceCullingEnabled(bool enabled) {
    if (_backFaceCulling == enabled)
        return;

    if (enabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
    _backFaceCulling = enabled;
}

static uint32_t getPolygonModeGL(PolygonMode mode) {
    switch (mode) {
    case PolygonMode::Line:
        return GL_LINE;
    case PolygonMode::Fill:
    default:
        return GL_FILL;
    }
}

void Context::setPolygonMode(PolygonMode mode) {
    if (_polygonMode == mode) {
        return;
    }
    glPolygonMode(GL_FRONT_AND_BACK, getPolygonModeGL(mode));
}

void Context::setBlendMode(BlendMode mode) {
    if (_blendMode == mode)
        return;

    switch (mode) {
    case BlendMode::None:
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
        break;
    case BlendMode::Add:
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendMode::Lighten:
        glBlendEquationSeparate(GL_MAX, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendMode::Default:
    default:
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
        break;
    }

    _blendMode = mode;
}

void Context::withScissorTest(const glm::ivec4 &bounds, const function<void()> &block) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds[0], bounds[1], bounds[2], bounds[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    block();

    glDisable(GL_SCISSOR_TEST);
}

void Context::bindReadFramebuffer(shared_ptr<Framebuffer> framebuffer) {
    if (_boundFramebufferRead == framebuffer) {
        return;
    }
    framebuffer->bind(FramebufferTarget::Read);
    _boundFramebufferRead = move(framebuffer);
}

void Context::bindDrawFramebuffer(shared_ptr<Framebuffer> framebuffer) {
    if (_boundFramebufferDraw == framebuffer) {
        return;
    }
    framebuffer->bind(FramebufferTarget::Draw);
    _boundFramebufferDraw = move(framebuffer);
}

void Context::bindRenderbuffer(shared_ptr<Renderbuffer> renderbuffer) {
    if (_boundRenderbuffer == renderbuffer) {
        return;
    }
    renderbuffer->bind();
    _boundRenderbuffer = move(renderbuffer);
}

void Context::bindTexture(int unit, shared_ptr<Texture> texture) {
    size_t numUnits = _boundTextures.size();
    if (numUnits <= unit) {
        _boundTextures.resize(unit + 1);
    }
    if (_boundTextures[unit] == texture) {
        return;
    }
    setActiveTextureUnit(unit);
    texture->bind();
    _boundTextures[unit] = move(texture);
}

void Context::bindUniformBuffer(int bindingPoint, shared_ptr<UniformBuffer> buffer) {
    size_t numBuffers = _boundUniformBuffers.size();
    if (numBuffers <= bindingPoint) {
        _boundUniformBuffers.resize(bindingPoint + 1);
    }
    if (_uniformBufferBindingPoint == bindingPoint && _boundUniformBuffers[bindingPoint] == buffer) {
        return;
    }
    buffer->bind(bindingPoint);
    _uniformBufferBindingPoint = bindingPoint;
    _boundUniformBuffers[bindingPoint] = move(buffer);
}

void Context::unbindReadFramebuffer() {
    if (!_boundFramebufferRead) {
        return;
    }
    _boundFramebufferRead->unbind(FramebufferTarget::Read);
    _boundFramebufferRead.reset();
}

void Context::unbindDrawFramebuffer() {
    if (!_boundFramebufferDraw) {
        return;
    }
    _boundFramebufferDraw->unbind(FramebufferTarget::Draw);
    _boundFramebufferDraw.reset();
}

void Context::unbindRenderbuffer() {
    if (!_boundRenderbuffer) {
        return;
    }
    _boundRenderbuffer->unbind();
    _boundRenderbuffer.reset();
}

void Context::unbindTexture(int unit) {
    if (!_boundTextures[unit]) {
        return;
    }
    setActiveTextureUnit(unit);
    _boundTextures[unit]->unbind();
    _boundTextures[unit].reset();
}

void Context::unbindUniformBuffer(int index) {
    if (!_boundUniformBuffers[index]) {
        return;
    }
    _boundUniformBuffers[index]->unbind(index);
    _boundUniformBuffers[index].reset();
}

void Context::setActiveTextureUnit(int unit) {
    if (_textureUnit == unit) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    _textureUnit = unit;
}

} // namespace graphics

} // namespace reone
