/*
 * Copyright (c) 2020 The reone project contributors
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

#include "glm/ext.hpp"

#include "GL/glew.h"

#include "../mesh/quad.h"
#include "../shaders.h"
#include "../util.h"

using namespace std;

namespace reone {

namespace render {

static const int kShadowResolution = 2048;

WorldRenderPipeline::WorldRenderPipeline(IRenderable *scene, const GraphicsOptions &opts) :
    _scene(scene),
    _opts(opts),
    _geometry(opts.width, opts.height, 2),
    _verticalBlur(opts.width, opts.height),
    _horizontalBlur(opts.width, opts.height) {

    for (int i = 0; i < kMaxShadowLightCount; ++i) {
        _shadows.push_back(make_unique<Framebuffer>(kShadowResolution, kShadowResolution, 0));
    }
}

void WorldRenderPipeline::init() {
    _geometry.init();
    _verticalBlur.init();
    _horizontalBlur.init();

    for (auto &shadows : _shadows) {
        shadows->init();
    }
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
    const vector<ShadowLight> &lights = _scene->shadowLights();

    int lightCount = static_cast<int>(lights.size());
    if (lightCount == 0) return;

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, kShadowResolution, kShadowResolution);

    GlobalUniforms globals;

    for (int i = 0; i < lightCount; ++i) {
        _shadows[i]->bind();

        glDrawBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);

        globals.cameraPosition = lights[i].position;
        globals.projection = lights[i].projection;
        globals.view = lights[i].view;

        Shaders::instance().setGlobalUniforms(globals);

        withDepthTest([this]() { _scene->renderNoGlobalUniforms(true); });

        _shadows[i]->unbind();
    }

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void WorldRenderPipeline::drawGeometry() const {
    _geometry.bind();

    static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int lightCount = static_cast<int>(_scene->shadowLights().size());

    for (int i = 0; i < lightCount; ++i) {
        glActiveTexture(GL_TEXTURE0 + TextureUniforms::shadowmap0 + i);
        _shadows[i]->bindDepthBuffer();
    }
    withDepthTest([this]() { _scene->render(); });

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

    glActiveTexture(GL_TEXTURE0);
    _geometry.bindColorBuffer(1);

    withDepthTest([]() {
        Quad::getDefault().renderTriangles();
    });

    _geometry.unbindColorBuffer();
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

    glActiveTexture(GL_TEXTURE0);
    _horizontalBlur.bindColorBuffer(0);

    withDepthTest([]() {
        Quad::getDefault().renderTriangles();
    });

    _horizontalBlur.unbindColorBuffer();
    _verticalBlur.unbind();
}

void WorldRenderPipeline::drawResult() const {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    LocalUniforms locals;
    locals.general.bloomEnabled = true;
    locals.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::GUIBloom, locals);

    glActiveTexture(GL_TEXTURE0);
    _geometry.bindColorBuffer(0);

    glActiveTexture(GL_TEXTURE0 + TextureUniforms::bloom);
    _verticalBlur.bindColorBuffer(0);

    Quad::getDefault().renderTriangles();

    glActiveTexture(GL_TEXTURE0 + TextureUniforms::bloom);
    _verticalBlur.unbindColorBuffer();

    glActiveTexture(GL_TEXTURE0);
    _geometry.unbindColorBuffer();
}

} // namespace render

} // namespace reone
