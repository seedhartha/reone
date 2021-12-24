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

#include "../camera/perspective.h"
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

static constexpr float kPointLightShadowsFOV = glm::radians(90.0f);
static constexpr float kPointLightShadowsNearPlane = 0.1f;
static constexpr float kPointLightShadowsFarPlane = 10000.0f;

static constexpr int kScreenshotResolution = 256;

static const vector<float> g_shadowCascadeDivisors {
    0.005f,
    0.015f,
    0.035f,
    0.075f,
    0.155f,
    0.315f,
    0.635f};

void WorldPipeline::init() {
    // Reusable renderbuffers

    _depthRenderbuffer = make_unique<Renderbuffer>();
    _depthRenderbuffer->configure(_options.width, _options.height, PixelFormat::Depth);
    _depthRenderbuffer->init();

    _depthRenderbufferMultisample = make_unique<Renderbuffer>(_options.aaSamples);
    _depthRenderbufferMultisample->configure(_options.width, _options.height, PixelFormat::Depth);
    _depthRenderbufferMultisample->init();

    // Multi-sampled geometry framebuffer

    _geometry1Color1 = make_unique<Texture>("geometry1_color1", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _geometry1Color1->clear(_options.width, _options.height, PixelFormat::RGB);
    _geometry1Color1->init();

    _geometry1Color2 = make_unique<Texture>("geometry1_color2", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _geometry1Color2->clear(_options.width, _options.height, PixelFormat::RGB);
    _geometry1Color2->init();

    _geometry1 = make_shared<Framebuffer>();
    _geometry1->attachColorsDepth(_geometry1Color1, _geometry1Color2, _depthRenderbufferMultisample);
    _geometry1->init();

    // Normal geometry framebuffer

    _geometry2Color1 = make_unique<Texture>("geometry2_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _geometry2Color1->clear(_options.width, _options.height, PixelFormat::RGB);
    _geometry2Color1->init();

    _geometry2Color2 = make_unique<Texture>("geometry2_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _geometry2Color2->clear(_options.width, _options.height, PixelFormat::RGB);
    _geometry2Color2->init();

    _geometry2 = make_shared<Framebuffer>();
    _geometry2->attachColorsDepth(_geometry2Color1, _geometry2Color2, _depthRenderbuffer);
    _geometry2->init();

    // Vertical blur framebuffer

    _verticalBlurColor = make_unique<Texture>("verticalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _verticalBlurColor->clear(_options.width, _options.height, PixelFormat::RGB);
    _verticalBlurColor->init();

    _verticalBlur = make_shared<Framebuffer>();
    _verticalBlur->attachColorDepth(_verticalBlurColor, _depthRenderbuffer);
    _verticalBlur->init();

    // Horizontal blur framebuffer

    _horizontalBlurColor = make_unique<Texture>("horizontalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _horizontalBlurColor->clear(_options.width, _options.height, PixelFormat::RGB);
    _horizontalBlurColor->init();

    _horizontalBlur = make_shared<Framebuffer>();
    _horizontalBlur->attachColorDepth(_horizontalBlurColor, _depthRenderbuffer);
    _horizontalBlur->init();

    // Shadows framebuffer

    _shadowsDepth = make_unique<Texture>("shadows_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _shadowsDepth->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth, kNumShadowCascades);
    _shadowsDepth->init();

    _directionalLightShadows = make_shared<Framebuffer>();
    _directionalLightShadows->attachDepth(_shadowsDepth);
    _directionalLightShadows->init();

    _cubeShadowsDepth = make_unique<Texture>("cubeshadows_depth", getTextureProperties(TextureUsage::DepthBufferCubeMap));
    _cubeShadowsDepth->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth);
    _cubeShadowsDepth->init();

    _pointLightShadows = make_shared<Framebuffer>();
    _pointLightShadows->attachDepth(_cubeShadowsDepth);
    _pointLightShadows->init();

    // Screenshot framebuffer

    _screenshotColor = make_unique<Texture>("result_color", getTextureProperties(TextureUsage::ColorBuffer));
    _screenshotColor->clear(kScreenshotResolution, kScreenshotResolution, PixelFormat::RGB);
    _screenshotColor->init();

    _screenshot = make_shared<Framebuffer>();
    _screenshot->attachColorDepth(_screenshotColor, _depthRenderbuffer);
    _screenshot->init();
}

void WorldPipeline::draw() {
    if (!_scene->camera()) {
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

static vector<glm::vec4> computeFrustumCornersWorldSpace(const glm::mat4 &projection, const glm::mat4 &view) {
    auto inv = glm::inverse(projection * view);

    vector<glm::vec4> corners;
    for (auto x = 0; x < 2; ++x) {
        for (auto y = 0; y < 2; ++y) {
            for (auto z = 0; z < 2; ++z) {
                auto pt = inv * glm::vec4(
                                    2.0f * x - 1.0f,
                                    2.0f * y - 1.0f,
                                    2.0f * z - 1.0f,
                                    1.0f);
                corners.push_back(pt / pt.w);
            }
        }
    }

    return move(corners);
}

static glm::mat4 computeDirectionalLightSpaceMatrix(
    float fov,
    float aspect,
    float near, float far,
    const glm::vec3 &lightDir,
    const glm::mat4 &cameraView) {

    auto projection = glm::perspective(fov, aspect, near, far);

    glm::vec3 center(0.0f);
    auto corners = computeFrustumCornersWorldSpace(projection, cameraView);
    for (auto &v : corners) {
        center += glm::vec3(v);
    }
    center /= corners.size();

    auto lightView = glm::lookAt(center - lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = numeric_limits<float>::max();
    float maxX = numeric_limits<float>::min();
    float minY = numeric_limits<float>::max();
    float maxY = numeric_limits<float>::min();
    float minZ = numeric_limits<float>::max();
    float maxZ = numeric_limits<float>::min();
    for (auto &v : corners) {
        auto trf = lightView * v;
        minX = min(minX, trf.x);
        maxX = max(maxX, trf.x);
        minY = min(minY, trf.y);
        maxY = max(maxY, trf.y);
        minZ = min(minZ, trf.z);
        maxZ = max(maxZ, trf.z);
    }
    float zMult = 10.0f;
    if (minZ < 0.0f) {
        minZ *= zMult;
    } else {
        minZ /= zMult;
    }
    if (maxZ < 0.0f) {
        maxZ /= zMult;
    } else {
        maxZ *= zMult;
    }

    auto lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

void WorldPipeline::computeLightSpaceMatrices() {
    if (!_scene->hasShadowLight()) {
        return;
    }
    if (_scene->isShadowLightDirectional()) {
        auto camera = static_pointer_cast<PerspectiveCamera>(_scene->camera());
        auto lightDir = glm::normalize(camera->position() - _scene->shadowLightPosition());
        float fovy = camera->fovy();
        float aspect = camera->aspect();
        float cameraNear = camera->zNear();
        float cameraFar = camera->zFar();
        for (int i = 0; i < kNumShadowCascades; ++i) {
            float near = cameraNear;
            if (i > 0) {
                near = cameraFar * g_shadowCascadeDivisors[i - 1];
            }
            float far = cameraFar;
            if (i < kNumShadowCascades - 1) {
                far *= g_shadowCascadeDivisors[i];
            }
            _shadowLightSpace[i] = computeDirectionalLightSpaceMatrix(fovy, aspect, near, far, lightDir, camera->view());
            _shadowCascadeFarPlanes[i / 4][i % 4] = far;
        }
    } else {
        glm::mat4 projection(glm::perspective(kPointLightShadowsFOV, 1.0f, kPointLightShadowsNearPlane, kPointLightShadowsFarPlane));
        for (int i = 0; i < kNumCubeFaces; ++i) {
            glm::mat4 lightView(getPointLightView(_scene->shadowLightPosition(), static_cast<CubeMapFace>(i)));
            _shadowLightSpace[i] = projection * lightView;
        }
    }
}

void WorldPipeline::drawShadows() {
    if (!_scene->hasShadowLight()) {
        return;
    }

    // Set global uniforms
    glm::vec4 lightPosition(
        _scene->shadowLightPosition(),
        _scene->isShadowLightDirectional() ? 0.0f : 1.0f);
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.shadowLightPosition = move(lightPosition);
    for (int i = 0; i < kNumShadowLightSpace; ++i) {
        uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
    }

    _graphicsContext.withViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution), [this]() {
        auto shadows = _scene->isShadowLightDirectional() ? _directionalLightShadows : _pointLightShadows;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadows->nameGL());
        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_NONE);

        glClear(GL_DEPTH_BUFFER_BIT);
        _scene->drawShadows();
    });
}

void WorldPipeline::drawGeometry() {
    static constexpr GLenum colors[] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    auto camera = _scene->camera();

    // Set global uniforms

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();
    uniforms.general.cameraPosition = glm::vec4(camera->position(), 1.0f);
    uniforms.general.worldAmbientColor = glm::vec4(_scene->ambientLightColor(), 1.0f);
    uniforms.general.fogNear = _scene->fogNear();
    uniforms.general.fogFar = _scene->fogFar();
    uniforms.general.fogColor = glm::vec4(_scene->fogColor(), 1.0f);

    if (_scene->hasShadowLight()) {
        glm::vec4 lightPosition(
            _scene->shadowLightPosition(),
            _scene->isShadowLightDirectional() ? 0.0f : 1.0f);
        uniforms.general.shadowLightPosition = move(lightPosition);
        uniforms.general.shadowStrength = 1.0f - _scene->shadowFadeFactor();
        for (int i = 0; i < kNumShadowLightSpace; ++i) {
            uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
        }
        for (int i = 0; i < 2; ++i) {
            uniforms.general.shadowCascadeFarPlanes[i] = _shadowCascadeFarPlanes[i];
        }
    }

    // Draw scene to multi-sample framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _geometry1->nameGL());
    glDrawBuffers(2, colors);

    if (_scene->hasShadowLight()) {
        if (_scene->isShadowLightDirectional()) {
            _textures.bind(*_shadowsDepth, TextureUnits::shadowMap);
        } else {
            _textures.bind(*_cubeShadowsDepth, TextureUnits::cubeShadowMap);
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _scene->draw();

    // Blit multi-sample framebuffer to a second framebuffer

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _geometry1->nameGL());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _geometry2->nameGL());
    for (int i = 0; i < 2; ++i) {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, _options.width, _options.height, 0, 0, _options.width, _options.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WorldPipeline::applyHorizontalBlur() {
    // Bind horizontal blur framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _horizontalBlur->nameGL());

    // Bind bright geometry texture

    _textures.bind(*_geometry2Color2);

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(1.0f, 0.0f);

    // Draw a quad

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shaders.use(_shaders.blur(), true);
    _meshes.quadNDC().draw();

    // Restore context

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WorldPipeline::applyVerticalBlur() {
    // Bind vertical blur framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _verticalBlur->nameGL());

    // Bind diffuse map

    _textures.bind(*_horizontalBlurColor);

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(0.0f, 1.0f);

    // Draw a quad

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shaders.use(_shaders.blur(), true);
    _meshes.quadNDC().draw();

    // Restore context

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WorldPipeline::drawResult() {
    // Set viewport
    if (_takeScreenshot) {
        glViewport(0, 0, kScreenshotResolution, kScreenshotResolution);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _screenshot->nameGL());
    }

    // Bind textures
    _textures.bind(*_geometry2Color1);
    _textures.bind(*_verticalBlurColor, TextureUnits::bloom);

    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();

    // Draw a quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shaders.use(_shaders.presentWorld(), true);
    _meshes.quadNDC().draw();

    // Save screenshot and restore context
    if (_takeScreenshot) {
        glViewport(0, 0, _options.width, _options.height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _textures.bind(*_screenshotColor);
        _screenshotColor->flushGPUToCPU();
        _takeScreenshot = false; // finished taking a screenshot
    }
}

} // namespace graphics

} // namespace reone
