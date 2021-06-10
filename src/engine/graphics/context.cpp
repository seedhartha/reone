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
    glGetIntegerv(GL_VIEWPORT, &_viewport[0]);
    setBlendMode(BlendMode::Default);
}

void Context::unbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Context::unbindRenderbuffer() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Context::setViewport(glm::ivec4 viewport) {
    if (_viewport == viewport) return;

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    _viewport = move(viewport);
}

void Context::setDepthTestEnabled(bool enabled) {
    if (_depthTest == enabled) return;

    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    _depthTest = enabled;
}

void Context::setBackFaceCullingEnabled(bool enabled) {
    if (_backFaceCulling == enabled) return;

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
    if (_polygonMode == mode) return;

    glPolygonMode(GL_FRONT_AND_BACK, getPolygonModeGL(mode));
}

void Context::setActiveTextureUnit(int n) {
    if (_textureUnit == n) return;

    glActiveTexture(GL_TEXTURE0 + n);
    _textureUnit = n;
}

void Context::setBlendMode(BlendMode mode) {
    if (_blendMode == mode) return;

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

} // namespace graphics

} // namespace reone
