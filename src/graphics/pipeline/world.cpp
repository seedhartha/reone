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

#include "../context.h"
#include "../format/tgawriter.h"
#include "../mesh.h"
#include "../meshes.h"
#include "../renderbuffer.h"
#include "../scene.h"
#include "../shaders.h"
#include "../texture.h"
#include "../textures.h"
#include "../textureutil.h"
#include "../window.h"

using namespace std;

namespace reone {

namespace graphics {

static constexpr float kShadowNearPlane = 0.0f;
static constexpr float kShadowFarPlane = 10000.0f;
static constexpr float kOrthographicScale = 10.0f;
static constexpr int kScreenshotResolution = 256;

static bool g_wireframesEnabled = false;

void WorldPipeline::init() {
    for (int i = 0; i < kNumCubeFaces; ++i) {
        _shadowLightSpaceMatrices[i] = glm::mat4(1.0f);
    }

    // Reusable renderbuffers

    _depthRenderbuffer = make_unique<Renderbuffer>();
    _depthRenderbuffer->configure(_options.width, _options.height, PixelFormat::Depth);
    _depthRenderbuffer->init();

    _depthRenderbufferMultisample = make_unique<Renderbuffer>(_options.aaSamples);
    _depthRenderbufferMultisample->configure(_options.width, _options.height, PixelFormat::Depth);
    _depthRenderbufferMultisample->init();

    // Multi-sampled geometry framebuffer

    _geometry1Color1 = make_unique<Texture>("geometry1_color1", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _geometry1Color1->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry1Color1->init();

    _geometry1Color2 = make_unique<Texture>("geometry1_color2", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _geometry1Color2->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry1Color2->init();

    _geometry1 = make_shared<Framebuffer>(_geometry1Color1, _geometry1Color2, _depthRenderbufferMultisample);
    _geometry1->init();

    // Normal geometry framebuffer

    _geometry2Color1 = make_unique<Texture>("geometry2_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _geometry2Color1->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry2Color1->init();

    _geometry2Color2 = make_unique<Texture>("geometry2_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _geometry2Color2->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _geometry2Color2->init();

    _geometry2 = make_shared<Framebuffer>(_geometry2Color1, _geometry2Color2, _depthRenderbuffer);
    _geometry2->init();

    // Vertical blur framebuffer

    _verticalBlurColor = make_unique<Texture>("verticalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _verticalBlurColor->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _verticalBlurColor->init();

    _verticalBlur = make_shared<Framebuffer>(_verticalBlurColor, _depthRenderbuffer);
    _verticalBlur->init();

    // Horizontal blur framebuffer

    _horizontalBlurColor = make_unique<Texture>("horizontalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _horizontalBlurColor->clearPixels(_options.width, _options.height, PixelFormat::RGB);
    _horizontalBlurColor->init();

    _horizontalBlur = make_shared<Framebuffer>(_horizontalBlurColor, _depthRenderbuffer);
    _horizontalBlur->init();

    // Shadows framebuffer

    _shadowsDepth = make_unique<Texture>("shadows_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _shadowsDepth->clearPixels(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth);
    _shadowsDepth->init();

    _directionalLightShadows = make_shared<Framebuffer>(_shadowsDepth);
    _directionalLightShadows->init();

    _cubeShadowsDepth = make_unique<Texture>("cubeshadows_depth", getTextureProperties(TextureUsage::DepthBufferCubeMap));
    _cubeShadowsDepth->clearPixels(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth);
    _cubeShadowsDepth->init();

    _pointLightShadows = make_shared<Framebuffer>(_cubeShadowsDepth);
    _pointLightShadows->init();

    // Screenshot framebuffer

    _screenshotColor = make_unique<Texture>("result_color", getTextureProperties(TextureUsage::ColorBuffer));
    _screenshotColor->clearPixels(kScreenshotResolution, kScreenshotResolution, PixelFormat::RGB);
    _screenshotColor->init();

    _screenshot = make_shared<Framebuffer>(_screenshotColor, _depthRenderbuffer);
    _screenshot->init();
}

void WorldPipeline::draw() {
    if (!_scene->hasCamera()) {
        return;
    }
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

void WorldPipeline::computeLightSpaceMatrices() {
    static glm::vec3 up(0.0f, 0.0f, 1.0f);

    if (!_scene->hasShadowLight()) {
        return;
    }
    glm::vec3 lightPosition(_scene->shadowLightPosition());
    glm::vec3 cameraPosition(_scene->cameraPosition());
    if (_scene->isShadowLightDirectional()) {
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

void WorldPipeline::drawShadows() {
    if (!_scene->hasShadowLight()) {
        return;
    }

    // Set uniforms prototype
    glm::vec4 lightPosition(
        _scene->shadowLightPosition(),
        _scene->isShadowLightDirectional() ? 0.0f : 1.0f);
    auto &uniformsPrototype = _scene->uniformsPrototype();
    uniformsPrototype.general.reset();
    uniformsPrototype.general.featureMask = UniformsFeatureFlags::shadows;
    uniformsPrototype.general.shadowLightPosition = move(lightPosition);
    for (int i = 0; i < kNumCubeFaces; ++i) {
        uniformsPrototype.general.shadowLightSpaceMatrices[i] = _shadowLightSpaceMatrices[i];
    }

    // Set viewport
    glViewport(0, 0, _options.shadowResolution, _options.shadowResolution);

    // Enable depth testing
    bool oldDepthTest = _graphicsContext.isDepthTestEnabled();
    _graphicsContext.setDepthTestEnabled(true);

    // Bind shadows framebuffer
    auto shadows = _scene->isShadowLightDirectional() ? _directionalLightShadows : _pointLightShadows;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadows->nameGL());
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);

    // Draw the scene
    glClear(GL_DEPTH_BUFFER_BIT);
    _scene->draw(true);

    // Restore context
    _graphicsContext.setDepthTestEnabled(oldDepthTest);
    glViewport(0, 0, _options.width, _options.height);
}

void WorldPipeline::drawGeometry() {
    static constexpr GLenum colors[] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    // Set uniforms prototype

    auto &uniforms = _scene->uniformsPrototype();
    uniforms.general.reset();
    uniforms.general.featureMask = _scene->hasShadowLight() ? UniformsFeatureFlags::shadowlight : 0;
    uniforms.general.projection = _scene->cameraProjection();
    uniforms.general.view = _scene->cameraView();
    uniforms.general.cameraPosition = glm::vec4(_scene->cameraPosition(), 1.0f);

    if (_scene->hasShadowLight()) {
        glm::vec4 lightPosition(
            _scene->shadowLightPosition(),
            _scene->isShadowLightDirectional() ? 0.0f : 1.0f);

        uniforms.general.shadowLightPosition = move(lightPosition);
        uniforms.general.shadowStrength = 1.0f - _scene->shadowFadeFactor();

        for (int i = 0; i < kNumCubeFaces; ++i) {
            uniforms.general.shadowLightSpaceMatrices[i] = _shadowLightSpaceMatrices[i];
        }
    }

    // Enable wireframe mode

    PolygonMode oldPolygonMode = _graphicsContext.polygonMode();
    if (g_wireframesEnabled) {
        _graphicsContext.setPolygonMode(PolygonMode::Line);
    }

    // Enable depth testing

    bool oldDepthTest = _graphicsContext.isDepthTestEnabled();
    _graphicsContext.setDepthTestEnabled(true);

    // Bind multi-sampled geometry framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _geometry1->nameGL());
    glDrawBuffers(2, colors);

    if (_scene->hasShadowLight()) {
        if (_scene->isShadowLightDirectional()) {
            _graphicsContext.bindTexture(TextureUnits::shadowMap, _shadowsDepth);
        } else {
            _graphicsContext.bindTexture(TextureUnits::shadowMapCube, _cubeShadowsDepth);
        }
    }

    // Draw the scene

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _scene->draw();

    // Blit multi-sampled geometry framebuffer to normal

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _geometry1->nameGL());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _geometry2->nameGL());
    for (int i = 0; i < 2; ++i) {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, _options.width, _options.height, 0, 0, _options.width, _options.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // Restore context

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _graphicsContext.setPolygonMode(oldPolygonMode);
    _graphicsContext.setDepthTestEnabled(oldDepthTest);
}

void WorldPipeline::applyHorizontalBlur() {
    // Enable depth testing

    bool oldDepthTest = _graphicsContext.isDepthTestEnabled();
    _graphicsContext.setDepthTestEnabled(true);

    // Bind horizontal blur framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _horizontalBlur->nameGL());

    // Bind bright geometry texture

    _graphicsContext.bindTexture(0, _geometry2Color2);

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.reset();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(1.0f, 0.0f);

    // Draw a quad

    _graphicsContext.useShaderProgram(_shaders.blur());
    _shaders.refreshUniforms();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _meshes.quadNDC().draw();

    // Restore context

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _graphicsContext.setDepthTestEnabled(oldDepthTest);
}

void WorldPipeline::applyVerticalBlur() {
    // Enable depth testing

    bool oldDepthTest = _graphicsContext.isDepthTestEnabled();
    _graphicsContext.setDepthTestEnabled(true);

    // Bind vertical blur framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _verticalBlur->nameGL());

    // Bind diffuse map

    _graphicsContext.bindTexture(0, _horizontalBlurColor);

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.reset();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(0.0f, 1.0f);

    // Draw a quad

    _graphicsContext.useShaderProgram(_shaders.blur());
    _shaders.refreshUniforms();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _meshes.quadNDC().draw();

    // Restore context

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _graphicsContext.setDepthTestEnabled(oldDepthTest);
}

void WorldPipeline::drawResult() {
    // Set viewport

    glm::ivec4 oldViewport;
    if (_takeScreenshot) {
        glViewport(0, 0, kScreenshotResolution, kScreenshotResolution);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _screenshot->nameGL());
    }

    // Bind geometry texture

    _graphicsContext.bindTexture(0, _geometry2Color1);

    // Bind blur texture

    _graphicsContext.bindTexture(TextureUnits::bloom, _verticalBlurColor);

    // Set shader uniforms

    auto &uniforms = _shaders.uniforms();
    uniforms.general.reset();

    // Draw a quad

    _graphicsContext.useShaderProgram(_shaders.presentWorld());
    _shaders.refreshUniforms();
    _meshes.quadNDC().draw();

    // Restore context

    if (_takeScreenshot) {
        saveScreenshot();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, kScreenshotResolution, kScreenshotResolution);
        _takeScreenshot = false; // finished taking a screenshot
    }
}

void WorldPipeline::saveScreenshot() {
    glViewport(0, 0, _options.width, _options.height);
    _graphicsContext.bindTexture(0, _screenshotColor);
    _screenshotColor->flushGPUToCPU();
}

} // namespace graphics

} // namespace reone
