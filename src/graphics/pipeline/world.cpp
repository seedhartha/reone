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

static constexpr GLenum kColorAttachments[] {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4};

static const vector<float> g_shadowCascadeDivisors {
    0.005f,
    0.015f,
    0.035f,
    0.075f,
    0.155f,
    0.315f,
    0.635f};

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

void WorldPipeline::init() {
    // Reusable depth buffers

    _dbCommon = make_unique<Renderbuffer>();
    _dbCommon->configure(_options.width, _options.height, PixelFormat::Depth32F);
    _dbCommon->init();

    // Reusable framebuffers

    _cbPing = make_unique<Texture>("ping_color", getTextureProperties(TextureUsage::ColorBuffer));
    _cbPing->clear(_options.width, _options.height, PixelFormat::RGBA8);
    _cbPing->init();

    _fbPing = make_unique<Framebuffer>();
    _fbPing->attachColorDepth(_cbPing, _dbCommon);
    _fbPing->init();

    _cbPong = make_unique<Texture>("pong_color", getTextureProperties(TextureUsage::ColorBuffer));
    _cbPong->clear(_options.width, _options.height, PixelFormat::RGBA8);
    _cbPong->init();

    _fbPong = make_unique<Framebuffer>();
    _fbPong->attachColorDepth(_cbPong, _dbCommon);
    _fbPong->init();

    // Directional light shadows framebuffer

    _dbDirectionalLightShadows = make_unique<Texture>("point_light_shadows_color", getTextureProperties(TextureUsage::DepthBuffer));
    _dbDirectionalLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F, kNumShadowCascades);
    _dbDirectionalLightShadows->init();

    _fbDirectionalLightShadows = make_shared<Framebuffer>();
    _fbDirectionalLightShadows->attachDepth(_dbDirectionalLightShadows);
    _fbDirectionalLightShadows->init();

    // Point light shadows framebuffer

    _dbPointLightShadows = make_unique<Texture>("directional_light_shadows_color", getTextureProperties(TextureUsage::DepthBufferCubeMap));
    _dbPointLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F);
    _dbPointLightShadows->init();

    _fbPointLightShadows = make_shared<Framebuffer>();
    _fbPointLightShadows->attachDepth(_dbPointLightShadows);
    _fbPointLightShadows->init();

    // Geometry framebuffer

    _cbGeometry1 = make_unique<Texture>("geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _cbGeometry1->clear(_options.width, _options.height, PixelFormat::RGB8);
    _cbGeometry1->init();

    _cbGeometry2 = make_unique<Texture>("geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _cbGeometry2->clear(_options.width, _options.height, PixelFormat::RGB8);
    _cbGeometry2->init();

    _cbGeometryEyeDepth = make_unique<Texture>("geometry_eyedepth", getTextureProperties(TextureUsage::ColorBuffer));
    _cbGeometryEyeDepth->clear(_options.width, _options.height, PixelFormat::R16F);
    _cbGeometryEyeDepth->init();

    _cbGeometryEyeNormal = make_unique<Texture>("geometry_eyenormal", getTextureProperties(TextureUsage::ColorBuffer));
    _cbGeometryEyeNormal->clear(_options.width, _options.height, PixelFormat::RGB8);
    _cbGeometryEyeNormal->init();

    _cbGeometryRoughness = make_unique<Texture>("geometry_roughness", getTextureProperties(TextureUsage::ColorBuffer));
    _cbGeometryRoughness->clear(_options.width, _options.height, PixelFormat::R8);
    _cbGeometryRoughness->init();

    _fbGeometry = make_shared<Framebuffer>();
    _fbGeometry->attachColorsDepth({_cbGeometry1, _cbGeometry2, _cbGeometryEyeDepth, _cbGeometryEyeNormal, _cbGeometryRoughness}, _dbCommon);
    _fbGeometry->init();

    // SSR framebuffer

    _cbSSR = make_unique<Texture>("ssr_color", getTextureProperties(TextureUsage::ColorBuffer));
    _cbSSR->clear(_options.width, _options.height, PixelFormat::RGB8);
    _cbSSR->init();

    _fbSSR = make_shared<Framebuffer>();
    _fbSSR->attachColorDepth(_cbSSR, _dbCommon);
    _fbSSR->init();

    // Screenshot framebuffer

    _cbScreenshot = make_unique<Texture>("screenshot_color", getTextureProperties(TextureUsage::ColorBuffer));
    _cbScreenshot->clear(kScreenshotResolution, kScreenshotResolution, PixelFormat::RGB8);
    _cbScreenshot->init();

    _dbScreenshot = make_unique<Renderbuffer>();
    _dbScreenshot->configure(kScreenshotResolution, kScreenshotResolution, PixelFormat::Depth32F);
    _dbScreenshot->init();

    _fbScreenshot = make_shared<Framebuffer>();
    _fbScreenshot->attachColorDepth(_cbScreenshot, _dbScreenshot);
    _fbScreenshot->init();
}

void WorldPipeline::draw() {
    if (!_scene->camera()) {
        return;
    }
    computeLightSpaceMatrices();
    drawShadows();
    drawGeometry();
    applySSR();
    applyHorizontalBlur();
    applyVerticalBlur();
    applyBloom();
    applyFXAA();
    presentWorld();
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
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.shadowLightPosition = glm::vec4(_scene->shadowLightPosition(), _scene->isShadowLightDirectional() ? 0.0f : 1.0f);
    for (int i = 0; i < kNumShadowLightSpace; ++i) {
        uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
    }

    // Draw shadows to a separate framebuffer
    auto framebuffer = _scene->isShadowLightDirectional() ? _fbDirectionalLightShadows : _fbPointLightShadows;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->nameGL());
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    _graphicsContext.withViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution), [this]() {
        _graphicsContext.clearDepth();
        _scene->drawShadows();
    });
}

void WorldPipeline::drawGeometry() {
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
        uniforms.general.shadowStrength = _scene->shadowStrength();
        uniforms.general.shadowRadius = _scene->shadowRadius();
        for (int i = 0; i < kNumShadowLightSpace; ++i) {
            uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
        }
        for (int i = 0; i < 2; ++i) {
            uniforms.general.shadowCascadeFarPlanes[i] = _shadowCascadeFarPlanes[i];
        }
    }

    // Draw scene to geometry framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbGeometry->nameGL());
    glDrawBuffers(_options.ssr ? 5 : 2, kColorAttachments);
    _graphicsContext.clearColorDepth();
    if (_scene->hasShadowLight()) {
        if (_scene->isShadowLightDirectional()) {
            _textures.bind(*_dbDirectionalLightShadows, TextureUnits::shadowMap);
        } else {
            _textures.bind(*_dbPointLightShadows, TextureUnits::cubeShadowMap);
        }
    }
    _scene->draw();
}

void WorldPipeline::applySSR() {
    if (!_options.ssr) {
        return;
    }

    // Set uniforms
    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);
    auto camera = _scene->camera();
    auto screenProjection = glm::mat4(1.0f);
    screenProjection *= glm::scale(glm::vec3(w, h, 1.0f));
    screenProjection *= glm::translate(glm::vec3(0.5f, 0.5f, 0.0f));
    screenProjection *= glm::scale(glm::vec3(0.5f, 0.5f, 1.0f));
    screenProjection *= camera->projection();
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.projection = camera->projection();
    uniforms.general.screenProjection = move(screenProjection);
    uniforms.general.screenResolution = glm::vec2(w, h);

    // Apply screen-space reflections to geometry color buffers
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbSSR->nameGL());
    _shaders.use(_shaders.ssr(), true);
    _textures.bind(*_cbGeometry1);
    _textures.bind(*_cbGeometryEyeDepth, TextureUnits::eyeDepth);
    _textures.bind(*_cbGeometryEyeNormal, TextureUnits::eyeNormal);
    _textures.bind(*_cbGeometryRoughness, TextureUnits::roughness);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void WorldPipeline::applyHorizontalBlur() {
    // Set uniforms
    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.screenResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(1.0f, 0.0f);

    // Apply horizontal blur to bright geometry color buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbPing->nameGL());
    _shaders.use(_shaders.blur(), true);
    _textures.bind(*_cbGeometry2);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void WorldPipeline::applyVerticalBlur() {
    // Set shader uniforms
    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.screenResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(0.0f, 1.0f);

    // Apply vertical blur to ping color buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbPong->nameGL());
    _shaders.use(_shaders.blur(), true);
    _textures.bind(*_cbPing);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void WorldPipeline::applyBloom() {
    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();

    // Combine geometry or SSR and pong (horizontal + vertical blur) color buffers
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbPing->nameGL());
    _shaders.use(_shaders.bloom(), true);
    _textures.bind(_options.ssr ? *_cbSSR : *_cbGeometry1);
    _textures.bind(*_cbPong, TextureUnits::bloom);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void WorldPipeline::applyFXAA() {
    if (!_options.fxaa) {
        return;
    }

    // Reset uniforms
    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.screenResolution = glm::vec2(w, h);

    // Apply FXAA to ping (bloom) color buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbPong->nameGL());
    _shaders.use(_shaders.fxaa(), true);
    _textures.bind(*_cbPing);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void WorldPipeline::presentWorld() {
    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();

    // Present ping (bloom) or pong (FXAA) color buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    _shaders.use(_shaders.presentWorld(), true);
    _textures.bind(_options.fxaa ? *_cbPong : *_cbPing);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();

    // Render to screenshot texture
    if (_takeScreenshot) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbScreenshot->nameGL());
        _graphicsContext.withViewport(glm::ivec4(0, 0, kScreenshotResolution, kScreenshotResolution), [this]() {
            _graphicsContext.clearColorDepth();
            _meshes.quadNDC().draw();
        });
        _textures.bind(*_cbScreenshot);
        _cbScreenshot->flushGPUToCPU();
        _takeScreenshot = false;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

} // namespace graphics

} // namespace reone
