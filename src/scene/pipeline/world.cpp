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

#include "../../render/meshes.h"
#include "../../render/pbribl.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"
#include "../../render/textures.h"
#include "../../render/textureutil.h"
#include "../../render/window.h"

#include "../node/cameranode.h"
#include "../scenegraph.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr int kShadowResolution = 2048;
static constexpr float kShadowFarPlane = 10000.0f;

static bool g_wireframesEnabled = false;
static bool g_debugCubeMap = false;

WorldRenderPipeline::WorldRenderPipeline(SceneGraph *scene, const GraphicsOptions &opts) :
    _scene(scene), _opts(opts) {
}

void WorldRenderPipeline::init() {
    // Reusable depth renderbuffer

    _depthRenderbuffer = make_unique<Renderbuffer>();
    _depthRenderbuffer->init();
    _depthRenderbuffer->bind();
    _depthRenderbuffer->configure(_opts.width, _opts.height, PixelFormat::Depth);
    _depthRenderbuffer->unbind();


    // Geometry framebuffer

    _geometryColor1 = make_unique<Texture>("geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _geometryColor1->init();
    _geometryColor1->bind();
    _geometryColor1->clearPixels(_opts.width, _opts.height, PixelFormat::RGB);

    _geometryColor2 = make_unique<Texture>("geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _geometryColor2->init();
    _geometryColor2->bind();
    _geometryColor2->clearPixels(_opts.width, _opts.height, PixelFormat::RGB);

    _geometry.init();
    _geometry.bind();
    _geometry.attachColor(*_geometryColor1, 0);
    _geometry.attachColor(*_geometryColor2, 1);
    _geometry.attachDepth(*_depthRenderbuffer);
    _geometry.checkCompleteness();


    // Vertical blur framebuffer

    _verticalBlurColor = make_unique<Texture>("verticalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _verticalBlurColor->init();
    _verticalBlurColor->bind();
    _verticalBlurColor->clearPixels(_opts.width, _opts.height, PixelFormat::RGB);

    _verticalBlur.init();
    _verticalBlur.bind();
    _verticalBlur.attachColor(*_verticalBlurColor);
    _verticalBlur.attachDepth(*_depthRenderbuffer);
    _verticalBlur.checkCompleteness();


    // Horizontal blur framebuffer

    _horizontalBlurColor = make_unique<Texture>("horizontalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _horizontalBlurColor->init();
    _horizontalBlurColor->bind();
    _horizontalBlurColor->clearPixels(_opts.width, _opts.height, PixelFormat::RGB);

    _horizontalBlur.init();
    _horizontalBlur.bind();
    _horizontalBlur.attachColor(*_horizontalBlurColor);
    _horizontalBlur.attachDepth(*_depthRenderbuffer);
    _horizontalBlur.checkCompleteness();


    // Shadows framebuffer

    _shadowsDepth = make_unique<Texture>("shadows_depth", getTextureProperties(TextureUsage::CubeMapDepthBuffer));
    _shadowsDepth->init();
    _shadowsDepth->bind();
    _shadowsDepth->clearPixels(kShadowResolution, kShadowResolution, PixelFormat::Depth);

    _shadows.init();
    _shadows.bind();
    _shadows.attachDepth(*_shadowsDepth);
    _shadows.checkCompleteness();
}

void WorldRenderPipeline::render() {
    drawShadows();
    drawGeometry();
    applyHorizontalBlur();
    applyVerticalBlur();
    drawResult();
}

void WorldRenderPipeline::drawShadows() {
    if (!_scene->isShadowLightPresent()) return;

    withViewport(glm::ivec4(0, 0, kShadowResolution, kShadowResolution), [this]() {
        _shadows.bind();

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glm::mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, kShadowFarPlane));
        glm::vec3 lightPosition(_scene->shadowLightPosition());

        ShaderUniforms uniforms;
        uniforms.general.shadowLightPresent = true;
        uniforms.general.shadowLightPosition = glm::vec4(lightPosition, 1.0f);

        for (int i = 0; i < kNumCubeFaces; ++i) {
            auto side = static_cast<CubeMapFace>(i);
            uniforms.general.shadowMatrices[i] = projection * getShadowView(lightPosition, side);
        }

        _scene->setUniformsPrototype(move(uniforms));

        glClear(GL_DEPTH_BUFFER_BIT);
        withDepthTest([this]() { _scene->render(true); });
    });
}

glm::mat4 WorldRenderPipeline::getShadowView(const glm::vec3 &lightPos, CubeMapFace side) const {
    switch (side) {
        case CubeMapFace::PositiveX:
            return glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapFace::NegativeX:
            return glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapFace::PositiveY:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        case CubeMapFace::NegativeY:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
        case CubeMapFace::PositiveZ:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapFace::NegativeZ:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
        default:
            throw invalid_argument("side is invalid");
    }
}

void WorldRenderPipeline::drawGeometry() {
    _geometry.bind();

    static constexpr GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);

    ShaderUniforms uniforms;
    uniforms.general.projection = _scene->activeCamera()->projection();
    uniforms.general.view = _scene->activeCamera()->view();
    uniforms.general.cameraPosition = _scene->activeCamera()->absoluteTransform()[3];
    uniforms.general.shadowLightPresent = _scene->isShadowLightPresent();
    uniforms.general.shadowLightPosition = glm::vec4(_scene->shadowLightPosition(), 1.0f);
    _scene->setUniformsPrototype(move(uniforms));

    if (_scene->isShadowLightPresent()) {
        setActiveTextureUnit(TextureUnits::shadowMap);
        _shadowsDepth->bind();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_wireframesEnabled) {
        withWireframes([this]() {
            withDepthTest([this]() { _scene->render(); });
        });
    } else {
        withDepthTest([this]() { _scene->render(); });
    }
}

void WorldRenderPipeline::applyHorizontalBlur() {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _horizontalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    ShaderUniforms uniforms;
    uniforms.general.projection = RenderWindow::instance().getOrthoProjection();
    uniforms.general.model = move(transform);
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(1.0f, 0.0f);
    Shaders::instance().activate(ShaderProgram::SimpleBlur, uniforms);

    setActiveTextureUnit(TextureUnits::diffuse);
    _geometryColor2->bind();

    withDepthTest([]() {
        Meshes::instance().getQuad().render();
    });
}

void WorldRenderPipeline::applyVerticalBlur() {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _verticalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    ShaderUniforms uniforms;
    uniforms.general.projection = RenderWindow::instance().getOrthoProjection();
    uniforms.general.model = move(transform);
    uniforms.general.blurResolution = glm::vec2(_opts.width, _opts.height);
    uniforms.general.blurDirection = glm::vec2(0.0f, 1.0f);
    Shaders::instance().activate(ShaderProgram::SimpleBlur, uniforms);

    setActiveTextureUnit(TextureUnits::diffuse);
    _horizontalBlurColor->bind();

    withDepthTest([]() {
        Meshes::instance().getQuad().render();
    });

    _verticalBlur.unbind();
}

void WorldRenderPipeline::drawResult() {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    if (g_debugCubeMap) {
        ShaderUniforms uniforms;
        uniforms.general.projection = RenderWindow::instance().getOrthoProjection();
        uniforms.general.model = move(transform);
        Shaders::instance().activate(ShaderProgram::SimpleDebugCubeMap, uniforms);

        setActiveTextureUnit(TextureUnits::diffuse);
        auto envmap = Textures::instance().get("cm_baremetal", TextureUsage::EnvironmentMap);
        PBRIBL::Derived derived;
        if (PBRIBL::instance().getDerived(envmap.get(), derived)) {
            derived.brdfLookup->bind();
        } else {
            envmap->bind();
        }
        //_shadowsDepth->bind();

        Meshes::instance().getQuad().render();

    } else {
        ShaderUniforms uniforms;
        uniforms.general.projection = RenderWindow::instance().getOrthoProjection();
        uniforms.general.model = move(transform);
        Shaders::instance().activate(ShaderProgram::SimplePresentWorld, uniforms);

        setActiveTextureUnit(TextureUnits::diffuse);
        _geometryColor1->bind();

        setActiveTextureUnit(TextureUnits::bloom);
        _verticalBlurColor->bind();

        Meshes::instance().getQuad().render();
    }
}

} // namespace scene

} // namespace reone
