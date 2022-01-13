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

    setDepthTestMode(DepthTestMode::LessOrEqual);
    _depthTestModes.push(DepthTestMode::LessOrEqual);

    setCullFaceMode(CullFaceMode::None);
    _cullFaceModes.push(CullFaceMode::None);

    setBlendMode(BlendMode::None);
    _blendModes.push(BlendMode::None);

    glm::ivec4 viewport(0.0f);
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    _viewports.push(move(viewport));

    _inited = true;
}

void GraphicsContext::clearColor() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsContext::clearDepth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::clearColorDepth() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::withBlending(BlendMode mode, const function<void()> &block) {
    if (_blendModes.top() == mode) {
        block();
        return;
    }

    setBlendMode(mode);
    _blendModes.push(mode);

    block();

    _blendModes.pop();
    setBlendMode(_blendModes.top());
}

void GraphicsContext::withDepthTest(DepthTestMode mode, const function<void()> &block) {
    if (_depthTestModes.top() == mode) {
        block();
        return;
    }
    setDepthTestMode(mode);
    _depthTestModes.push(mode);

    block();

    _depthTestModes.pop();
    setDepthTestMode(_depthTestModes.top());
}

void GraphicsContext::withFaceCulling(CullFaceMode mode, const function<void()> &block) {
    if (_cullFaceModes.top() == mode) {
        block();
        return;
    }
    setCullFaceMode(mode);
    _cullFaceModes.push(mode);

    block();

    _cullFaceModes.pop();
    setCullFaceMode(_cullFaceModes.top());
}

void GraphicsContext::withViewport(glm::ivec4 viewport, const function<void()> &block) {
    if (_viewports.top() == viewport) {
        block();
        return;
    }
    setViewport(viewport);
    _viewports.push(viewport);

    block();

    _viewports.pop();
    setViewport(_viewports.top());
}

void GraphicsContext::withScissorTest(const glm::ivec4 &bounds, const function<void()> &block) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds[0], bounds[1], bounds[2], bounds[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    block();

    glDisable(GL_SCISSOR_TEST);
}

void GraphicsContext::setDepthTestMode(DepthTestMode mode) {
    if (mode == DepthTestMode::None) {
        glDisable(GL_DEPTH_TEST);
    } else {
        glEnable(GL_DEPTH_TEST);
        switch (mode) {
            break;
        case DepthTestMode::Equal:
            glDepthFunc(GL_EQUAL);
            break;
        case DepthTestMode::LessOrEqual:
            glDepthFunc(GL_LEQUAL);
            break;
        case DepthTestMode::Less:
        default:
            glDepthFunc(GL_LESS);
            break;
        }
    }
}

void GraphicsContext::setCullFaceMode(CullFaceMode mode) {
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
}

void GraphicsContext::setBlendMode(BlendMode mode) {
    if (mode == BlendMode::None) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        switch (mode) {
        case BlendMode::Additive:
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
            break;
        case BlendMode::Lighten:
            glBlendEquationSeparate(GL_MAX, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
            break;
        case BlendMode::Normal:
        default:
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
            break;
        }
    }
}

void GraphicsContext::setViewport(glm::ivec4 viewport) {
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

} // namespace graphics

} // namespace reone
