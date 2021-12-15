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

#include "../../graphics/context.h"
#include "../../graphics/format/tgawriter.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/textures.h"
#include "../../graphics/textureutil.h"
#include "../../graphics/window.h"

#include "../graph.h"
#include "../node/camera.h"
#include "../node/light.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kShadowNearPlane = 0.0f;
static constexpr float kShadowFarPlane = 10000.0f;
static constexpr float kOrthographicScale = 10.0f;
static constexpr int kScreenshotResolution = 256;

static bool g_wireframesEnabled = false;

void WorldRenderPipeline::init() {
    for (int i = 0; i < kNumCubeFaces; ++i) {
        _shadowLightSpaceMatrices[i] = glm::mat4(1.0f);
    }

    // Reusable depth renderbuffers

    _depthRenderbuffer = make_unique<Renderbuffer>();
    _depthRenderbuffer->clearPixels(_options.width, _options.height, PixelFormat::Depth);
    _depthRenderbuffer->init();

    _depthRenderbufferMultisample = make_unique<Renderbuffer>(_options.aaSamples);
    _depthRenderbufferMultisample->clearPixels(_options.width, _options.height, PixelFormat::Depth);
    _depthRenderbufferMultisample->init();

    // Multi-sampled geometry framebuffer

    _geometry1Color1 = make_unique<Texture>("geometry1_color1", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _geometry1Color1->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry1Color1->init();

    _geometry1Color2 = make_unique<Texture>("geometry1_color2", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _geometry1Color2->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry1Color2->init();

    _geometry1 = make_shared<Framebuffer>();
    _geometry1->init();
    _context.bindDrawFramebuffer(_geometry1);
    _geometry1->attachColor(*_geometry1Color1, 0);
    _geometry1->attachColor(*_geometry1Color2, 1);
    _geometry1->attachDepth(*_depthRenderbufferMultisample);
    _context.unbindDrawFramebuffer();

    // Normal geometry framebuffer

    _geometry2Color1 = make_unique<Texture>("geometry2_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _geometry2Color1->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry2Color1->init();

    _geometry2Color2 = make_unique<Texture>("geometry2_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _geometry2Color2->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry2Color2->init();

    _geometry2 = make_shared<Framebuffer>();
    _geometry2->init();
    _context.bindDrawFramebuffer(_geometry2);
    _geometry2->attachColor(*_geometry2Color1, 0);
    _geometry2->attachColor(*_geometry2Color2, 1);
    _geometry2->attachDepth(*_depthRenderbuffer);
    _context.unbindDrawFramebuffer();

    // Vertical blur framebuffer

    _verticalBlurColor = make_unique<Texture>("verticalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _verticalBlurColor->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _verticalBlurColor->init();

    _verticalBlur = make_shared<Framebuffer>();
    _verticalBlur->init();
    _context.bindDrawFramebuffer(_verticalBlur);
    _verticalBlur->attachColor(*_verticalBlurColor);
    _verticalBlur->attachDepth(*_depthRenderbuffer);
    _context.unbindDrawFramebuffer();

    // Horizontal blur framebuffer

    _horizontalBlurColor = make_unique<Texture>("horizontalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _horizontalBlurColor->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _horizontalBlurColor->init();

    _horizontalBlur = make_shared<Framebuffer>();
    _horizontalBlur->init();
    _context.bindDrawFramebuffer(_horizontalBlur);
    _horizontalBlur->attachColor(*_horizontalBlurColor);
    _horizontalBlur->attachDepth(*_depthRenderbuffer);
    _context.unbindDrawFramebuffer();

    // Shadows framebuffer

    _shadowsDepth = make_unique<Texture>("shadows_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _shadowsDepth->clearPixels(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth);
    _shadowsDepth->init();

    _cubeShadowsDepth = make_unique<Texture>("cubeshadows_depth", getTextureProperties(TextureUsage::DepthBufferCubeMap));
    _cubeShadowsDepth->clearPixels(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth);
    _cubeShadowsDepth->init();

    _shadows = make_shared<Framebuffer>();
    _shadows->init();

    // Screenshot framebuffer

    _screenshotColor = make_unique<Texture>("result_color", getTextureProperties(TextureUsage::ColorBuffer));
    _screenshotColor->clearPixels(kScreenshotResolution, kScreenshotResolution, PixelFormat::RGB);
    _screenshotColor->init();

    _screenshot = make_shared<Framebuffer>();
    _screenshot->init();
    _context.bindDrawFramebuffer(_screenshot);
    _screenshot->attachColor(*_screenshotColor);
    _screenshot->attachDepth(*_depthRenderbuffer);
    _context.unbindDrawFramebuffer();
}

void WorldRenderPipeline::render() {
    shared_ptr<CameraSceneNode> camera(_sceneGraph.activeCamera());
    if (!camera)
        return;

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

    const LightSceneNode *shadowLight = _sceneGraph.shadowLight();
    if (!shadowLight)
        return;

    glm::vec3 lightPosition(shadowLight->absoluteTransform()[3]);
    glm::vec3 cameraPosition(_sceneGraph.activeCamera()->absoluteTransform()[3]);

    if (shadowLight->isDirectional()) {
        glm::mat4 projection(glm::ortho(-kOrthographicScale, kOrthographicScale, -kOrthographicScale, kOrthographicScale, kShadowNearPlane, kShadowFarPlane));
        glm::mat4 lightView(glm::lookAt(lightPosition, cameraPosition, up));
        _shadowLightSpaceMatrices[0] = projection * lightView;
    } else {
        glm::mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, kShadowNearPlane, kShadowFarPlane));
        for (int i = 0; i < kNumCubeFaces; ++i) {
            glm::mat4 lightView(getPointLightView(lightPosition, static_cast<CubeMapFace>(i)));
            _shadowLightSpaceMatrices[i] = projection * lightView;
        }
    }
}

void WorldRenderPipeline::drawShadows() {
    // Set uniforms prototype

    const LightSceneNode *shadowLight = _sceneGraph.shadowLight();
    if (!shadowLight)
        return;

    glm::vec4 lightPosition(
        glm::vec3(shadowLight->absoluteTransform()[3]),
        shadowLight->isDirectional() ? 0.0f : 1.0f);

    auto &uniformsPrototype = _sceneGraph.uniformsPrototype();
    uniformsPrototype.general.reset();
    uniformsPrototype.general.featureMask = UniformsFeatureFlags::shadows;
    uniformsPrototype.general.shadowLightPosition = move(lightPosition);
    for (int i = 0; i < kNumCubeFaces; ++i) {
        uniformsPrototype.general.shadowLightSpaceMatrices[i] = _shadowLightSpaceMatrices[i];
    }

    // Set viewport

    glm::ivec4 oldViewport(_context.viewport());
    _context.setViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution));

    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind shadows framebuffer

    _context.bindDrawFramebuffer(_shadows);
    if (shadowLight->isDirectional()) {
        _shadows->attachDepth(*_shadowsDepth);
    } else {
        _shadows->attachDepth(*_cubeShadowsDepth);
    }
    _shadows->disableDrawBuffer();
    _shadows->disableReadBuffer();

    // Draw the scene

    _context.clear(ClearBuffers::depth);
    _sceneGraph.draw(true);

    // Restore context

    _context.setDepthTestEnabled(oldDepthTest);
    _context.setViewport(move(oldViewport));
}

void WorldRenderPipeline::drawGeometry() {
    // Set uniforms prototype

    shared_ptr<CameraSceneNode> camera(_sceneGraph.activeCamera());
    const LightSceneNode *shadowLight = _sceneGraph.shadowLight();

    auto &uniforms = _sceneGraph.uniformsPrototype();
    uniforms.general.reset();
    uniforms.general.featureMask = shadowLight ? UniformsFeatureFlags::shadowlight : 0;
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();
    uniforms.general.cameraPosition = camera->absoluteTransform()[3];

    if (shadowLight) {
        glm::vec4 lightPosition(
            glm::vec3(shadowLight->absoluteTransform()[3]),
            shadowLight->isDirectional() ? 0.0f : 1.0f);

        uniforms.general.shadowLightPosition = move(lightPosition);
        uniforms.general.shadowStrength = 1.0f - shadowLight->fadeFactor();

        for (int i = 0; i < kNumCubeFaces; ++i) {
            uniforms.general.shadowLightSpaceMatrices[i] = _shadowLightSpaceMatrices[i];
        }
    }

    // Enable wireframe mode

    PolygonMode oldPolygonMode = _context.polygonMode();
    if (g_wireframesEnabled) {
        _context.setPolygonMode(PolygonMode::Line);
    }

    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind multi-sampled geometry framebuffer

    _context.bindDrawFramebuffer(_geometry1);
    _geometry1->setDrawBuffers(2);

    if (shadowLight) {
        if (shadowLight->isDirectional()) {
            _context.bindTexture(TextureUnits::shadowMap, _shadowsDepth);
        } else {
            _context.bindTexture(TextureUnits::shadowMapCube, _cubeShadowsDepth);
        }
    }

    // Draw the scene

    _context.clear(ClearBuffers::colorDepth);
    _sceneGraph.draw();

    // Blit multi-sampled geometry framebuffer to normal

    _context.bindReadFramebuffer(_geometry1);
    _context.bindDrawFramebuffer(_geometry2);
    for (int i = 0; i < 2; ++i) {
        _geometry1->setReadBuffer(i);
        _geometry2->setDrawBuffer(i);
        _geometry1->blit(_options.width, _options.height);
    }

    // Restore context

    _context.unbindDrawFramebuffer();
    _context.setPolygonMode(oldPolygonMode);
    _context.setDepthTestEnabled(oldDepthTest);
}

void WorldRenderPipeline::applyHorizontalBlur() {
    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind horizontal blur framebuffer

    _context.bindDrawFramebuffer(_horizontalBlur);

    // Bind bright geometry texture

    _context.bindTexture(0, _geometry2Color2);

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.reset();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(1.0f, 0.0f);

    // Draw a quad

    _context.useShaderProgram(_shaders.blur());
    _shaders.refreshUniforms();
    _context.clear(ClearBuffers::colorDepth);
    _meshes.quadNDC().draw();

    // Restore context

    _context.unbindDrawFramebuffer();
    _context.setDepthTestEnabled(oldDepthTest);
}

void WorldRenderPipeline::applyVerticalBlur() {
    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind vertical blur framebuffer

    _context.bindDrawFramebuffer(_verticalBlur);

    // Bind diffuse map

    _context.bindTexture(0, _horizontalBlurColor);

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.reset();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(0.0f, 1.0f);

    // Draw a quad

    _context.useShaderProgram(_shaders.blur());
    _shaders.refreshUniforms();
    _context.clear(ClearBuffers::colorDepth);
    _meshes.quadNDC().draw();

    // Restore context

    _context.unbindDrawFramebuffer();
    _context.setDepthTestEnabled(oldDepthTest);
}

void WorldRenderPipeline::drawResult() {
    // Set viewport

    glm::ivec4 oldViewport;
    if (_takeScreenshot) {
        oldViewport = _context.viewport();
        _context.setViewport(glm::ivec4(0, 0, kScreenshotResolution, kScreenshotResolution));
        _context.bindDrawFramebuffer(_screenshot);
    }

    // Bind geometry texture

    _context.bindTexture(0, _geometry2Color1);

    // Bind blur texture

    _context.bindTexture(TextureUnits::bloom, _verticalBlurColor);

    // Set shader uniforms

    auto &uniforms = _shaders.uniforms();
    uniforms.general.reset();

    // Draw a quad

    _context.useShaderProgram(_shaders.presentWorld());
    _shaders.refreshUniforms();
    _meshes.quadNDC().draw();

    // Restore context

    if (_takeScreenshot) {
        saveScreenshot();
        _context.unbindDrawFramebuffer();
        _context.setViewport(move(oldViewport));
        _takeScreenshot = false; // finished taking a screenshot
    }
}

void WorldRenderPipeline::saveScreenshot() {
    _context.bindTexture(0, _screenshotColor);
    _screenshotColor->flushGPUToCPU();
}

} // namespace scene

} // namespace reone
