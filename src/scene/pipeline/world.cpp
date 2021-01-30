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

#include "world.h"

#include <stdexcept>

#include "glm/ext.hpp"

#include "GL/glew.h"

#include "../../render/mesh/quad.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"
#include "../../render/textures.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr int kShadowResolution = 2048;
static constexpr float kShadowFarPlane = 10000.0f;

static bool g_wireframesEnabled = false;
static bool g_debugShadows = false;

WorldRenderPipeline::WorldRenderPipeline(SceneGraph *scene, const GraphicsOptions &opts) :
    _scene(scene),
    _opts(opts),
    _geometry(opts.width, opts.height, 2),
    _verticalBlur(opts.width, opts.height),
    _horizontalBlur(opts.width, opts.height),
    _shadows(kShadowResolution, kShadowResolution, 0, true) {
}

void WorldRenderPipeline::init() {
    _geometry.init();
    _verticalBlur.init();
    _horizontalBlur.init();
    _shadows.init();
}

void WorldRenderPipeline::render() const {
    drawShadows();
    drawGeometry();

    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    GlobalUniforms globals;
    globals.projection = glm::ortho(0.0f, w, h, 0.0f);

    Shaders::instance().setGlobalUniforms(globals);

    applyHorizontalBlur();
    applyVerticalBlur();
    drawResult();
}

void WorldRenderPipeline::drawShadows() const {
    if (!_scene->isShadowLightPresent()) return;

    withViewport(glm::ivec4(0, 0, kShadowResolution, kShadowResolution), [this]() {
        _shadows.bind();

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glm::mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, kShadowFarPlane));
        glm::vec3 lightPosition(_scene->shadowLightPosition());

        GlobalUniforms globals;
        globals.shadowLightPresent = true;
        globals.shadowLightPosition = lightPosition;

        for (int i = 0; i < kNumCubeFaces; ++i) {
            auto side = static_cast<CubeMapSide>(i);
            globals.shadowMatrices[i] = projection * getShadowView(lightPosition, side);
        }

        Shaders::instance().setGlobalUniforms(globals);

        glClear(GL_DEPTH_BUFFER_BIT);
        withDepthTest([this]() { _scene->renderNoGlobalUniforms(true); });

        _shadows.unbind();
    });
}

glm::mat4 WorldRenderPipeline::getShadowView(const glm::vec3 &lightPos, CubeMapSide side) const {
    switch (side) {
        case CubeMapSide::PositiveX:
            return glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapSide::NegativeX:
            return glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapSide::PositiveY:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        case CubeMapSide::NegativeY:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
        case CubeMapSide::PositiveZ:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapSide::NegativeZ:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
        default:
            throw invalid_argument("side is invalid");
    }
}

void WorldRenderPipeline::drawGeometry() const {
    _geometry.bind();

    static constexpr GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_scene->isShadowLightPresent()) {
        setActiveTextureUnit(TextureUnits::shadowmap);
        _shadows.bindDepthBuffer();
    }

    if (g_wireframesEnabled) {
        withWireframes([this]() {
            withDepthTest([this]() { _scene->render(); });
        });
    } else {
        withDepthTest([this]() { _scene->render(); });
    }

    if (_scene->isShadowLightPresent()) {
        setActiveTextureUnit(TextureUnits::shadowmap);
        _shadows.unbindDepthBuffer();
    }

    _geometry.unbind();
}

void WorldRenderPipeline::applyHorizontalBlur() const {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _horizontalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    LocalUniforms locals;
    locals.general.blurEnabled = true;
    locals.general.model = move(transform);
    locals.general.blurResolution = glm::vec2(w, h);
    locals.general.blurDirection = glm::vec2(1.0f, 0.0f);

    Shaders::instance().activate(ShaderProgram::GUIBlur, locals);

    setActiveTextureUnit(0);
    _geometry.bindColorBuffer(1);

    withDepthTest([]() {
        Quad::getDefault().renderTriangles();
    });

    _geometry.unbindColorBuffer(1);
    _horizontalBlur.unbind();
}

void WorldRenderPipeline::applyVerticalBlur() const {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _verticalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    LocalUniforms locals;
    locals.general.blurEnabled = true;
    locals.general.model = move(transform);
    locals.general.blurResolution = glm::vec2(_opts.width, _opts.height);
    locals.general.blurDirection = glm::vec2(0.0f, 1.0f);

    Shaders::instance().activate(ShaderProgram::GUIBlur, locals);

    setActiveTextureUnit(0);
    _horizontalBlur.bindColorBuffer(0);

    withDepthTest([]() {
        Quad::getDefault().renderTriangles();
    });

    _horizontalBlur.unbindColorBuffer(0);
    _verticalBlur.unbind();
}

void WorldRenderPipeline::drawResult() const {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    if (g_debugShadows) {
        LocalUniforms locals;
        locals.general.model = move(transform);

        Shaders::instance().activate(ShaderProgram::GUIDebugShadows, locals);

        setActiveTextureUnit(TextureUnits::shadowmap);
        _shadows.bindDepthBuffer();

        Quad::getDefault().renderTriangles();

        _shadows.unbindDepthBuffer();

    } else {
        LocalUniforms locals;
        locals.general.bloomEnabled = true;
        locals.general.model = move(transform);

        Shaders::instance().activate(ShaderProgram::GUIBloom, locals);

        setActiveTextureUnit(0);
        _geometry.bindColorBuffer(0);

        setActiveTextureUnit(TextureUnits::bloom);
        _verticalBlur.bindColorBuffer(0);

        Quad::getDefault().renderTriangles();

        setActiveTextureUnit(TextureUnits::bloom);
        _verticalBlur.unbindColorBuffer(0);

        setActiveTextureUnit(0);
        _geometry.unbindColorBuffer(0);
    }
}

} // namespace scene

} // namespace reone
