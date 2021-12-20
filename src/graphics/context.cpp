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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDepthFunc(GL_LEQUAL);

    if (_options.aaSamples > 1) {
        glEnable(GL_MULTISAMPLE);
    }
    setBlendMode(BlendMode::Default);
    _inited = true;
}

void GraphicsContext::deinit() {
    if (!_inited) {
        return;
    }
    if (_shaderProgram) {
        glUseProgram(0);
        _shaderProgram.reset();
    }
    for (size_t i = 0; i < _textures.size(); ++i) {
        unbindTexture(static_cast<int>(i));
    }
    _textures.clear();
    _inited = false;
}

void GraphicsContext::useShaderProgram(shared_ptr<ShaderProgram> program) {
    if (_shaderProgram == program) {
        return;
    }
    program->use();
    _shaderProgram = move(program);
}

void GraphicsContext::bindTexture(int unit, shared_ptr<Texture> texture) {
    size_t numUnits = _textures.size();
    if (numUnits <= unit) {
        _textures.resize(unit + 1);
    }
    if (_textures[unit] == texture) {
        return;
    }
    setActiveTextureUnit(unit);
    texture->bind();
    _textures[unit] = move(texture);
}

void GraphicsContext::unbindTexture(int unit) {
    if (!_textures[unit]) {
        return;
    }
    setActiveTextureUnit(unit);
    _textures[unit]->unbind();
    _textures[unit].reset();
}

void GraphicsContext::setBackFaceCullingEnabled(bool enabled) {
    if (_backFaceCulling == enabled) {
        return;
    }
    if (enabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
    _backFaceCulling = enabled;
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

void GraphicsContext::setActiveTextureUnit(int unit) {
    if (_textureUnit == unit) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    _textureUnit = unit;
}

void GraphicsContext::withScissorTest(const glm::ivec4 &bounds, const function<void()> &block) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds[0], bounds[1], bounds[2], bounds[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    block();

    glDisable(GL_SCISSOR_TEST);
}

} // namespace graphics

} // namespace reone
