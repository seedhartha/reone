/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/context.h"
#include "reone/graphics/shaderprogram.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniformbuffer.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

void GraphicsContext::init() {
    if (_inited) {
        return;
    }
    checkMainThread();
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        throw std::runtime_error(str(boost::format("glewInit failed: %s") % glewGetErrorString(error)));
    }
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    setDepthTestMode(DepthTestMode::LessOrEqual);
    _depthTestModes.push(DepthTestMode::LessOrEqual);

    setCullFaceMode(CullFaceMode::None);
    _cullFaceModes.push(CullFaceMode::None);

    setBlendMode(BlendMode::None);
    _blendModes.push(BlendMode::None);

    setPolygonMode(PolygonMode::Fill);
    _polygonModes.push(PolygonMode::Fill);

    glm::ivec4 viewport(0.0f);
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    _viewports.push(std::move(viewport));

    _inited = true;
}

void GraphicsContext::clearColor(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsContext::clearDepth() {
    checkMainThread();
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::clearColorDepth(glm::vec4 color) {
    checkMainThread();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::bind(Texture &texture, int unit) {
    if (_activeTexUnit != unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        _activeTexUnit = unit;
    }
    texture.bind();
}

void GraphicsContext::useProgram(ShaderProgram &program) {
    if (_usedProgram == &program) {
        return;
    }
    program.use();
    _usedProgram = &program;
}

void GraphicsContext::resetProgram() {
    if (!_usedProgram) {
        return;
    }
    glUseProgram(0);
    _usedProgram = nullptr;
}

void GraphicsContext::bind(UniformBuffer &buffer, int index) {
    _uniformBuffers[index] = &buffer;
    buffer.bind(index);
}

void GraphicsContext::pushBlending(BlendMode mode) {
    setBlendMode(mode);
    _blendModes.push(mode);
}

void GraphicsContext::pushViewport(glm::ivec4 viewport) {
    setViewport(viewport);
    _viewports.push(std::move(viewport));
}

void GraphicsContext::popBlending() {
    _blendModes.pop();
    setBlendMode(_blendModes.top());
}

void GraphicsContext::popViewport() {
    _viewports.pop();
    setViewport(_viewports.top());
}

void GraphicsContext::withBlending(BlendMode mode, const std::function<void()> &block) {
    if (_blendModes.top() == mode) {
        block();
        return;
    }
    pushBlending(mode);
    block();
    popBlending();
}

void GraphicsContext::withDepthTest(DepthTestMode mode, const std::function<void()> &block) {
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

void GraphicsContext::withFaceCulling(CullFaceMode mode, const std::function<void()> &block) {
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

void GraphicsContext::withPolygonMode(PolygonMode mode, const std::function<void()> &block) {
    if (_polygonModes.top() == mode) {
        block();
        return;
    }
    setPolygonMode(mode);
    _polygonModes.push(mode);

    block();

    _polygonModes.pop();
    setPolygonMode(_polygonModes.top());
}

void GraphicsContext::withViewport(glm::ivec4 viewport, const std::function<void()> &block) {
    if (_viewports.top() == viewport) {
        block();
        return;
    }
    pushViewport(std::move(viewport));
    block();
    popViewport();
}

void GraphicsContext::withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block) {
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
        case DepthTestMode::Always:
            glDepthFunc(GL_ALWAYS);
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
        case BlendMode::OIT_Transparent:
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::Normal:
        default:
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
            break;
        }
    }
}

void GraphicsContext::setPolygonMode(PolygonMode mode) {
    if (mode == PolygonMode::Line) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void GraphicsContext::setViewport(glm::ivec4 viewport) {
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

} // namespace graphics

} // namespace reone
