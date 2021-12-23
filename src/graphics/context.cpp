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
    if (_options.aaSamples > 1) {
        glEnable(GL_MULTISAMPLE);
    }

    // Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Blending
    glEnable(GL_BLEND);
    setBlendMode(BlendMode::Default);

    _inited = true;
}

void GraphicsContext::withBlendMode(BlendMode mode, const function<void()> &block) {
    auto oldBlendMode = _blendMode;
    if (oldBlendMode == mode) {
        block();
        return;
    }
    setBlendMode(mode);
    block();
    setBlendMode(oldBlendMode);
}

void GraphicsContext::withBackFaceCulling(const function<void()> &block) {
    if (_backFaceCulling) {
        block();
        return;
    }
    enableBackFaceCulling();
    block();
    disableBackFaceCulling();
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

void GraphicsContext::enableBackFaceCulling() {
    if (_backFaceCulling) {
        return;
    }
    glEnable(GL_CULL_FACE);
    _backFaceCulling = true;
}

void GraphicsContext::disableBackFaceCulling() {
    if (!_backFaceCulling) {
        return;
    }
    glDisable(GL_CULL_FACE);
    _backFaceCulling = false;
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
