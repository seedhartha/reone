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

void GraphicsContext::init() {
    if (_inited) {
        return;
    }
    glewInit();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Viewport
    glGetIntegerv(GL_VIEWPORT, &_viewport[0]);

    // Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Face Culling
    setCullFaceMode(CullFaceMode::None);

    // Blending
    glEnable(GL_BLEND);
    setBlendMode(BlendMode::Default);

    _inited = true;
}

void GraphicsContext::clearColorDepth() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::clearDepth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::withBlending(BlendMode mode, const function<void()> &block) {
    auto oldBlendMode = _blendMode;
    if (oldBlendMode == mode) {
        block();
        return;
    }
    setBlendMode(mode);
    block();
    setBlendMode(oldBlendMode);
}

void GraphicsContext::withFaceCulling(CullFaceMode mode, const function<void()> &block) {
    auto oldCullFaceMode = _cullFaceMode;
    if (oldCullFaceMode == mode) {
        block();
        return;
    }
    setCullFaceMode(mode);
    block();
    setCullFaceMode(oldCullFaceMode);
}

void GraphicsContext::withViewport(glm::ivec4 viewport, const function<void()> &block) {
    if (_viewport == viewport) {
        block();
        return;
    }
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    block();
    glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
}

void GraphicsContext::withScissorTest(const glm::ivec4 &bounds, const function<void()> &block) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds[0], bounds[1], bounds[2], bounds[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    block();

    glDisable(GL_SCISSOR_TEST);
}

void GraphicsContext::withoutDepthTest(const function<void()> &block) {
    if (!_depthTest) {
        block();
        return;
    }
    glDisable(GL_DEPTH_TEST);
    block();
    glEnable(GL_DEPTH_TEST);
}

void GraphicsContext::setCullFaceMode(CullFaceMode mode) {
    if (_cullFaceMode == mode) {
        return;
    }
    if (mode == CullFaceMode::None) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        if (mode == CullFaceMode::Front) {
            glCullFace(GL_FRONT);
        } else {
            glCullFace(GL_BACK);
        }
    }
    _cullFaceMode = mode;
}

void GraphicsContext::setBlendMode(BlendMode mode) {
    if (_blendMode == mode) {
        return;
    }
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

} // namespace graphics

} // namespace reone
