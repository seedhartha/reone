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

#include "../../graphics/mesh/meshes.h"
#include "../../graphics/pbribl.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/statemanager.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/texture/textureutil.h"
#include "../../graphics/window.h"

#include "../node/cameranode.h"
#include "../node/lightnode.h"
#include "../scenegraph.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

const float kShadowNearPlane = 0.0f;
const float kShadowFarPlane = 10000.0f;
const float kOrthographicScale = 10.0f;

static bool g_wireframesEnabled = false;

WorldRenderPipeline::WorldRenderPipeline(SceneGraph *scene, const GraphicsOptions &opts) :
    _scene(scene), _opts(opts) {

    for (int i = 0; i < kNumCubeFaces; ++i) {
        _lightSpaceMatrices[i] = glm::mat4(1.0f);
    }
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
    Textures::instance().bindDefaults();
    _geometry.attachColor(*_geometryColor1, 0);
    _geometry.attachColor(*_geometryColor2, 1);
    _geometry.attachDepth(*_depthRenderbuffer);
    _geometry.checkCompleteness();
    _geometry.unbind();

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
    _verticalBlur.unbind();

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
    _horizontalBlur.unbind();

    // Shadows framebuffer

    _shadowsDepth = make_unique<Texture>("shadows_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _shadowsDepth->init();
    _shadowsDepth->bind();
    _shadowsDepth->clearPixels(1024 * _opts.shadowResolution, 1024 * _opts.shadowResolution, PixelFormat::Depth);

    _cubeShadowsDepth = make_unique<Texture>("cubeshadows_depth", getTextureProperties(TextureUsage::CubeMapDepthBuffer));
    _cubeShadowsDepth->init();
    _cubeShadowsDepth->bind();
    _cubeShadowsDepth->clearPixels(1024 * _opts.shadowResolution, 1024 * _opts.shadowResolution, PixelFormat::Depth);

    _shadows.init();
}

void WorldRenderPipeline::render() {
    shared_ptr<CameraSceneNode> camera(_scene->activeCamera());
    if (!camera) return;

    computeLightSpaceMatrices();

    drawShadows();
    drawGeometry();
    applyHorizontalBlur();
    applyVerticalBlur();
    drawResult();
}

static glm::mat4 getPointLightView(const glm::vec3 &lightPos, CubeMapFace face) {
    switch (face) {
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

void WorldRenderPipeline::computeLightSpaceMatrices() {
    static glm::vec3 up(0.0f, 0.0f, 1.0f);

    const LightSceneNode *shadowLight = _scene->shadowLight();
    if (!shadowLight) return;

    glm::vec3 lightPosition(shadowLight->absoluteTransform()[3]);
    glm::vec3 cameraPosition(_scene->activeCamera()->absoluteTransform()[3]);

    if (shadowLight->isDirectional()) {
        glm::mat4 projection(glm::ortho(-kOrthographicScale, kOrthographicScale, -kOrthographicScale, kOrthographicScale, kShadowNearPlane, kShadowFarPlane));
        glm::mat4 lightView(glm::lookAt(lightPosition, cameraPosition, up));
        _lightSpaceMatrices[0] = projection * lightView;
    } else {
        glm::mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, kShadowNearPlane, kShadowFarPlane));
        for (int i = 0; i < kNumCubeFaces; ++i) {
            glm::mat4 lightView(getPointLightView(lightPosition, static_cast<CubeMapFace>(i)));
            _lightSpaceMatrices[i] = projection * lightView;
        }
    }
}

void WorldRenderPipeline::drawShadows() {
    if (_opts.shadowResolution < 1) return;

    const LightSceneNode *shadowLight = _scene->shadowLight();
    if (!shadowLight) return;

    StateManager::instance().withViewport(glm::ivec4(0, 0, 1024 * _opts.shadowResolution, 1024 * _opts.shadowResolution), [&]() {
        _shadows.bind();
        if (shadowLight->isDirectional()) {
            _shadows.attachDepth(*_shadowsDepth);
        } else {
            _shadows.attachDepth(*_cubeShadowsDepth);
        }

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glm::vec4 lightPosition(
            glm::vec3(shadowLight->absoluteTransform()[3]),
            shadowLight->isDirectional() ? 0.0f : 1.0f);

        ShaderUniforms uniforms(Shaders::instance().defaultUniforms());
        uniforms.combined.featureMask |= UniformFeatureFlags::shadows;
        uniforms.combined.shadows.lightPosition = move(lightPosition);
        for (int i = 0; i < kNumCubeFaces; ++i) {
            uniforms.combined.shadows.lightSpaceMatrices[i] = _lightSpaceMatrices[i];
        }
        _scene->setUniformsPrototype(move(uniforms));

        glClear(GL_DEPTH_BUFFER_BIT);
        StateManager::instance().withDepthTest([this]() { _scene->draw(true); });
    });
}

void WorldRenderPipeline::drawGeometry() {
    _geometry.bind();

    static constexpr GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);

    const LightSceneNode *shadowLight = _scene->shadowLight();

    ShaderUniforms uniforms(Shaders::instance().defaultUniforms());
    uniforms.combined.general.projection = _scene->activeCamera()->projection();
    uniforms.combined.general.view = _scene->activeCamera()->view();
    uniforms.combined.general.cameraPosition = _scene->activeCamera()->absoluteTransform()[3];
    uniforms.combined.shadows.lightPresent = static_cast<bool>(shadowLight);

    if (shadowLight) {
        glm::vec4 lightPosition(
            glm::vec3(shadowLight->absoluteTransform()[3]),
            shadowLight->isDirectional() ? 0.0f : 1.0f);

        uniforms.combined.shadows.lightPosition = move(lightPosition);
        uniforms.combined.shadows.strength = _scene->shadowStrength();

        for (int i = 0; i < kNumCubeFaces; ++i) {
            uniforms.combined.shadows.lightSpaceMatrices[i] = _lightSpaceMatrices[i];
        }
    }

    _scene->setUniformsPrototype(move(uniforms));

    if (shadowLight) {
        if (shadowLight->isDirectional()) {
            StateManager::instance().setActiveTextureUnit(TextureUnits::shadowMap);
            _shadowsDepth->bind();
        } else {
            StateManager::instance().setActiveTextureUnit(TextureUnits::shadowMapCube);
            _cubeShadowsDepth->bind();
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_wireframesEnabled) {
        StateManager::instance().withWireframes([this]() {
            StateManager::instance().withDepthTest([this]() { _scene->draw(); });
        });
    } else {
        StateManager::instance().withDepthTest([this]() { _scene->draw(); });
    }
}

void WorldRenderPipeline::applyHorizontalBlur() {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _horizontalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    StateManager::instance().setActiveTextureUnit(TextureUnits::diffuseMap);
    _geometryColor2->bind();

    ShaderUniforms uniforms;
    uniforms.combined.featureMask |= UniformFeatureFlags::blur;
    uniforms.combined.blur.resolution = glm::vec2(w, h);
    uniforms.combined.blur.direction = glm::vec2(1.0f, 0.0f);

    Shaders::instance().activate(ShaderProgram::SimpleBlur, uniforms);

    StateManager::instance().withDepthTest([]() {
        Meshes::instance().getQuadNDC()->draw();
    });
}

void WorldRenderPipeline::applyVerticalBlur() {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _verticalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    StateManager::instance().setActiveTextureUnit(TextureUnits::diffuseMap);
    _horizontalBlurColor->bind();

    ShaderUniforms uniforms;
    uniforms.combined.featureMask |= UniformFeatureFlags::blur;
    uniforms.combined.blur.resolution = glm::vec2(w, h);
    uniforms.combined.blur.direction = glm::vec2(0.0f, 1.0f);

    Shaders::instance().activate(ShaderProgram::SimpleBlur, uniforms);

    StateManager::instance().withDepthTest([]() {
        Meshes::instance().getQuadNDC()->draw();
    });

    _verticalBlur.unbind();
}

void WorldRenderPipeline::drawResult() {
    StateManager::instance().setActiveTextureUnit(TextureUnits::diffuseMap);
    _geometryColor1->bind();

    StateManager::instance().setActiveTextureUnit(TextureUnits::bloom);
    _verticalBlurColor->bind();

    ShaderUniforms uniforms;
    Shaders::instance().activate(ShaderProgram::SimplePresentWorld, uniforms);
    Meshes::instance().getQuadNDC()->draw();
}

} // namespace scene

} // namespace reone
