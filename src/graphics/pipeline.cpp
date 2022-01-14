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

#include "pipeline.h"

#include "../common/randomutil.h"

#include "camera/perspective.h"
#include "context.h"
#include "format/tgawriter.h"
#include "mesh.h"
#include "meshes.h"
#include "renderbuffer.h"
#include "scene.h"
#include "shaders.h"
#include "texture.h"
#include "textures.h"
#include "textureutil.h"
#include "window.h"

using namespace std;

#define R_BLUR_VERTICAL true

namespace reone {

namespace graphics {

static constexpr float kPointLightShadowsFOV = glm::radians(90.0f);
static constexpr float kPointLightShadowsNearPlane = 0.25f;
static constexpr float kPointLightShadowsFarPlane = 2500.0f;

static constexpr GLenum kColorAttachments[] {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7,
    GL_COLOR_ATTACHMENT8};

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

void Pipeline::init() {
    auto &uniforms = _shaders.uniforms();
    for (int i = 0; i < kNumSSAOSamples; ++i) {
        float scale = i / static_cast<float>(kNumSSAOSamples);
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        auto sample = glm::vec3(random(-1.0f, 1.0f), random(-1.0f, 1.0f), random(0.0f, 1.0f));
        sample = glm::normalize(sample);
        sample *= scale;
        uniforms.ssao.samples[i] = glm::vec4(move(sample), 0.0f);
    }
    _shaders.refreshSSAOUniforms();
}

void Pipeline::initAttachments(glm::ivec2 dim) {
    Attachments attachments;

    // Reusable depth buffer

    attachments.dbCommon = make_unique<Renderbuffer>();
    attachments.dbCommon->configure(dim.x, dim.y, PixelFormat::Depth32F);
    attachments.dbCommon->init();

    // Reusable framebuffers

    attachments.cbPing = make_unique<Texture>("ping_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbPing->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbPing->init();

    attachments.fbPing = make_unique<Framebuffer>();
    attachments.fbPing->attachColorDepth(attachments.cbPing, attachments.dbCommon);
    attachments.fbPing->init();

    attachments.cbPong = make_unique<Texture>("pong_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbPong->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbPong->init();

    attachments.fbPong = make_unique<Framebuffer>();
    attachments.fbPong->attachColorDepth(attachments.cbPong, attachments.dbCommon);
    attachments.fbPong->init();

    // Directional light shadows framebuffer

    attachments.dbDirectionalLightShadows = make_unique<Texture>("point_light_shadows_color", getTextureProperties(TextureUsage::DepthBuffer));
    attachments.dbDirectionalLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F, kNumShadowCascades);
    attachments.dbDirectionalLightShadows->init();

    attachments.fbDirectionalLightShadows = make_shared<Framebuffer>();
    attachments.fbDirectionalLightShadows->attachDepth(attachments.dbDirectionalLightShadows);
    attachments.fbDirectionalLightShadows->init();

    // Point light shadows framebuffer

    attachments.dbPointLightShadows = make_unique<Texture>("directional_light_shadows_color", getTextureProperties(TextureUsage::DepthBufferCubeMap));
    attachments.dbPointLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F);
    attachments.dbPointLightShadows->init();

    attachments.fbPointLightShadows = make_shared<Framebuffer>();
    attachments.fbPointLightShadows->attachDepth(attachments.dbPointLightShadows);
    attachments.fbPointLightShadows->init();

    // Depth framebuffer

    attachments.dbDepth = make_shared<Texture>("depth_depth", getTextureProperties(TextureUsage::DepthBuffer));
    attachments.dbDepth->clear(dim.x, dim.y, PixelFormat::Depth32F);
    attachments.dbDepth->init();

    attachments.fbDepth = make_shared<Framebuffer>();
    attachments.fbDepth->attachDepth(attachments.dbDepth);
    attachments.fbDepth->init();

    // SSAO framebuffer

    attachments.cbSSAO = make_shared<Texture>("ssao_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbSSAO->clear(dim.x, dim.y, PixelFormat::R8);
    attachments.cbSSAO->init();

    attachments.fbSSAO = make_shared<Framebuffer>();
    attachments.fbSSAO->attachColorDepth(attachments.cbSSAO, attachments.dbCommon);
    attachments.fbSSAO->init();

    // Opaque geometry framebuffer

    attachments.cbOpaqueGeometry1 = make_unique<Texture>("opaque_geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOpaqueGeometry1->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbOpaqueGeometry1->init();

    attachments.cbOpaqueGeometry2 = make_unique<Texture>("opaque_geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOpaqueGeometry2->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbOpaqueGeometry2->init();

    attachments.cbOpaqueGeometryEyeNormal = make_unique<Texture>("opaque_geometry_eyenormal", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOpaqueGeometryEyeNormal->clear(dim.x, dim.y, PixelFormat::RGB8);
    attachments.cbOpaqueGeometryEyeNormal->init();

    attachments.cbOpaqueGeometryRoughness = make_unique<Texture>("opaque_geometry_roughness", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOpaqueGeometryRoughness->clear(dim.x, dim.y, PixelFormat::R8);
    attachments.cbOpaqueGeometryRoughness->init();

    attachments.dbOpaqueGeometry = make_unique<Texture>("opaque_geometry_depth", getTextureProperties(TextureUsage::DepthBuffer));
    attachments.dbOpaqueGeometry->clear(dim.x, dim.y, PixelFormat::Depth32F);
    attachments.dbOpaqueGeometry->init();

    attachments.fbOpaqueGeometry = make_shared<Framebuffer>();
    attachments.fbOpaqueGeometry->attachColorsDepth(
        {attachments.cbOpaqueGeometry1,
         attachments.cbOpaqueGeometry2,
         attachments.cbOpaqueGeometryEyeNormal,
         attachments.cbOpaqueGeometryRoughness},
        attachments.dbOpaqueGeometry);
    attachments.fbOpaqueGeometry->init();

    // Translucent geometry framebuffer

    attachments.cbTranslucentGeometry1 = make_unique<Texture>("translucent_geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbTranslucentGeometry1->clear(dim.x, dim.y, PixelFormat::RGBA16F);
    attachments.cbTranslucentGeometry1->init();

    attachments.cbTranslucentGeometry2 = make_unique<Texture>("translucent_geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbTranslucentGeometry2->clear(dim.x, dim.y, PixelFormat::R16F);
    attachments.cbTranslucentGeometry2->init();

    attachments.fbTranslucentGeometry = make_shared<Framebuffer>();
    attachments.fbTranslucentGeometry->attachColorsDepth(
        {attachments.cbTranslucentGeometry1, attachments.cbTranslucentGeometry2},
        attachments.dbCommon);
    attachments.fbTranslucentGeometry->init();

    // SSR framebuffer

    attachments.cbSSR = make_unique<Texture>("ssr_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbSSR->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbSSR->init();

    attachments.fbSSR = make_shared<Framebuffer>();
    attachments.fbSSR->attachColorDepth(attachments.cbSSR, attachments.dbCommon);
    attachments.fbSSR->init();

    // Output framebuffer

    attachments.cbOutput = make_unique<Texture>("output_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOutput->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbOutput->init();

    attachments.fbOutput = make_unique<Framebuffer>();
    attachments.fbOutput->attachColorDepth(attachments.cbOutput, attachments.dbCommon);
    attachments.fbOutput->init();

    // Register attachments

    _attachments[dim] = move(attachments);
}

shared_ptr<Texture> Pipeline::draw(IScene &scene, const glm::ivec2 &dim) {
    if (!scene.camera()) {
        return nullptr;
    }

    if (_attachments.count(dim) == 0) {
        initAttachments(dim);
    }
    auto &attachments = _attachments.at(dim);

    computeLightSpaceMatrices(scene);

    _graphicsContext.withViewport(glm::ivec4(0, 0, dim.x, dim.y), [this, &scene, &dim, &attachments] {
        drawShadows(scene, attachments);

        // Screen-space ambient occlusion
        if (_options.ssao) {
            drawDepth(scene, attachments);
            drawSSAO(scene, dim, attachments);
            applyGaussianBlur(dim, *attachments.cbSSAO, *attachments.fbPing);
            applyGaussianBlur(dim, *attachments.cbPing, *attachments.fbSSAO, R_BLUR_VERTICAL);
        }

        drawOpaqueGeometry(scene, attachments);

        // Blur geometry hilights
        applyGaussianBlur(dim, *attachments.cbOpaqueGeometry2, *attachments.fbPing);
        applyGaussianBlur(dim, *attachments.cbPing, *attachments.fbPong, R_BLUR_VERTICAL);
        blitFramebuffer(dim, *attachments.fbPong, 0, *attachments.fbOpaqueGeometry, 1);

        // Screen-space reflections
        if (_options.ssr) {
            drawSSR(scene, dim, attachments);
            applyGaussianBlur(dim, *attachments.cbSSR, *attachments.fbPing);
            applyGaussianBlur(dim, *attachments.cbPing, *attachments.fbSSR, R_BLUR_VERTICAL);
        }

        drawCombineOpaque(attachments, *attachments.fbPing);
        blitFramebuffer(dim, *attachments.fbPing, 0, *attachments.fbOpaqueGeometry, 0);
        blitFramebuffer(dim, *attachments.fbOpaqueGeometry, 0, *attachments.fbTranslucentGeometry, 0, BlitFlags::depth);
        drawTranslucentGeometry(scene, attachments);
        drawCombineOIT(attachments, *attachments.fbOutput);
        drawLensFlares(scene, *attachments.fbOutput);

        if (_options.fxaa) {
            applyFXAA(dim, *attachments.cbOutput, *attachments.fbPing);
            blitFramebuffer(dim, *attachments.fbPing, 0, *attachments.fbOutput, 0);
        }
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return attachments.cbOutput;
}

void Pipeline::computeLightSpaceMatrices(IScene &scene) {
    if (!scene.hasShadowLight()) {
        return;
    }
    if (scene.isShadowLightDirectional()) {
        auto camera = static_pointer_cast<PerspectiveCamera>(scene.camera());
        auto lightDir = glm::normalize(camera->position() - scene.shadowLightPosition());
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
            glm::mat4 lightView(getPointLightView(scene.shadowLightPosition(), static_cast<CubeMapFace>(i)));
            _shadowLightSpace[i] = projection * lightView;
        }
    }
}

void Pipeline::drawShadows(IScene &scene, Attachments &attachments) {
    if (!scene.hasShadowLight()) {
        return;
    }

    // Set global uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.shadowLightPosition = glm::vec4(scene.shadowLightPosition(), scene.isShadowLightDirectional() ? 0.0f : 1.0f);
    for (int i = 0; i < kNumShadowLightSpace; ++i) {
        uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
    }

    // Draw shadows to a separate framebuffer
    auto framebuffer = scene.isShadowLightDirectional() ? attachments.fbDirectionalLightShadows : attachments.fbPointLightShadows;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->nameGL());
    glDrawBuffer(GL_NONE);
    _graphicsContext.withViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution), [this, &scene]() {
        _graphicsContext.clearDepth();
        scene.drawShadows();
    });
}

void Pipeline::drawDepth(IScene &scene, Attachments &attachments) {
    auto camera = scene.camera();

    // Set global uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();
    uniforms.general.viewInv = glm::inverse(camera->view());

    // Draw scene to depth framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbDepth->nameGL());
    glDrawBuffer(GL_NONE);
    _graphicsContext.clearDepth();
    scene.drawDepth();
}

void Pipeline::drawSSAO(IScene &scene, const glm::ivec2 &dim, Attachments &attachments) {
    // Reset uniforms
    auto camera = scene.camera();
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.projection = camera->projection();
    uniforms.general.projectionInv = glm::inverse(camera->projection());
    uniforms.general.screenResolution = glm::vec2(static_cast<float>(dim.x), static_cast<float>(dim.y));

    // Draw screen-space ambient occlusion
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbSSAO->nameGL());
    glDrawBuffer(kColorAttachments[0]);
    _shaders.use(_shaders.ssao(), true);
    _textures.bind(*attachments.dbDepth, TextureUnits::depthMap);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawOpaqueGeometry(IScene &scene, Attachments &attachments) {
    // Set global uniforms
    auto camera = scene.camera();
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();
    uniforms.general.viewInv = glm::inverse(camera->view());
    uniforms.general.cameraPosition = glm::vec4(camera->position(), 1.0f);
    uniforms.general.worldAmbientColor = glm::vec4(scene.ambientLightColor(), 1.0f);
    uniforms.general.fogNear = scene.fogNear();
    uniforms.general.fogFar = scene.fogFar();
    uniforms.general.fogColor = glm::vec4(scene.fogColor(), 1.0f);
    if (scene.hasShadowLight()) {
        glm::vec4 lightPosition(
            scene.shadowLightPosition(),
            scene.isShadowLightDirectional() ? 0.0f : 1.0f);
        uniforms.general.shadowLightPosition = move(lightPosition);
        uniforms.general.shadowStrength = scene.shadowStrength();
        uniforms.general.shadowRadius = scene.shadowRadius();
        for (int i = 0; i < kNumShadowLightSpace; ++i) {
            uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
        }
        for (int i = 0; i < 2; ++i) {
            uniforms.general.shadowCascadeFarPlanes[i] = _shadowCascadeFarPlanes[i];
        }
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbOpaqueGeometry->nameGL());
    int numBuffers = _options.ssr ? 5 : 2;
    glDrawBuffers(numBuffers, kColorAttachments);
    _graphicsContext.clearColorDepth();
    if (_options.ssao) {
        _textures.bind(*attachments.cbSSAO, TextureUnits::ssao);
    }
    if (scene.hasShadowLight()) {
        if (scene.isShadowLightDirectional()) {
            _textures.bind(*attachments.dbDirectionalLightShadows, TextureUnits::shadowMap);
        } else {
            _textures.bind(*attachments.dbPointLightShadows, TextureUnits::cubeShadowMap);
        }
    }
    scene.drawOpaque();
}

void Pipeline::drawTranslucentGeometry(IScene &scene, Attachments &attachments) {
    // Set global uniforms
    auto camera = scene.camera();
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();
    uniforms.general.viewInv = glm::inverse(camera->view());
    uniforms.general.cameraPosition = glm::vec4(camera->position(), 1.0f);
    uniforms.general.clipNear = camera->zNear();
    uniforms.general.clipFar = camera->zFar();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbTranslucentGeometry->nameGL());
    glDrawBuffers(2, kColorAttachments);
    _graphicsContext.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
    glDepthMask(GL_FALSE);
    _graphicsContext.withBlending(BlendMode::OIT_Translucent, [&scene] {
        scene.drawTranslucent();
    });
    glDepthMask(GL_TRUE);
}

void Pipeline::drawLensFlares(IScene &scene, Framebuffer &dst) {
    // Set global uniforms
    auto camera = scene.camera();
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    scene.drawLensFlares();
}

void Pipeline::drawSSR(IScene &scene, const glm::ivec2 &dim, Attachments &attachments) {
    // Set uniforms
    auto camera = scene.camera();
    auto screenProjection = glm::mat4(1.0f);
    screenProjection *= glm::scale(glm::vec3(dim.x, dim.y, 1.0f));
    screenProjection *= glm::translate(glm::vec3(0.5f, 0.5f, 0.0f));
    screenProjection *= glm::scale(glm::vec3(0.5f, 0.5f, 1.0f));
    screenProjection *= camera->projection();
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.projectionInv = glm::inverse(camera->projection());
    uniforms.general.screenProjection = move(screenProjection);
    uniforms.general.screenResolution = glm::vec2(dim.x, dim.y);
    uniforms.general.screenResolutionReciprocal = glm::vec4(1.0f / dim.x, 1.0f / dim.y, 0.0f, 0.0f);
    uniforms.general.clipNear = camera->zNear();
    uniforms.general.clipFar = camera->zFar();

    // Apply screen-space reflections to geometry color buffers
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbSSR->nameGL());
    glDrawBuffer(kColorAttachments[0]);
    _shaders.use(_shaders.ssr(), true);
    _textures.bind(*attachments.cbOpaqueGeometry1);
    _textures.bind(*attachments.dbOpaqueGeometry, TextureUnits::depthMap);
    _textures.bind(*attachments.cbOpaqueGeometryEyeNormal, TextureUnits::eyeNormal);
    _textures.bind(*attachments.cbOpaqueGeometryRoughness, TextureUnits::roughness);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawCombineOpaque(Attachments &attachments, Framebuffer &dst) {
    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _shaders.use(_shaders.combineOpaque(), true);
    _textures.bind(*attachments.cbOpaqueGeometry1);
    _textures.bind(*attachments.cbOpaqueGeometry2, TextureUnits::hilights);
    _textures.bind(*attachments.cbOpaqueGeometryRoughness, TextureUnits::roughness);
    _textures.bind(*attachments.cbSSR, TextureUnits::ssr);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawCombineOIT(Attachments &attachments, Framebuffer &dst) {
    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _shaders.use(_shaders.combineOIT(), true);
    _textures.bind(*attachments.cbOpaqueGeometry1);
    _textures.bind(*attachments.cbTranslucentGeometry1, TextureUnits::oitAccum);
    _textures.bind(*attachments.cbTranslucentGeometry2, TextureUnits::oitRevealage);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::applyGaussianBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool vertical) {
    // Set uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.screenResolutionReciprocal = glm::vec4(1.0f / dim.x, 1.0f / dim.y, 0.0f, 0.0f);
    uniforms.general.blurDirection = vertical ? glm::vec2(0.0f, 1.0f) : glm::vec2(1.0f, 0.0f);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _shaders.use(_shaders.gaussianBlur(), true);
    _textures.bind(srcTexture);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::applyMedianFilter(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst) {
    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.screenResolutionReciprocal = glm::vec4(1.0f / dim.x, 1.0f / dim.y, 0.0f, 0.0f);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _shaders.use(_shaders.medianFilter(), true);
    _textures.bind(srcTexture);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::applyFXAA(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst) {
    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.screenResolutionReciprocal = glm::vec4(
        -0.5f / dim.x,
        -0.5f / dim.y,
        0.5f / dim.x,
        0.5f / dim.y);
    uniforms.general.screenResolutionReciprocal2 = glm::vec4(
        -2.0f / dim.x,
        -2.0f / dim.y,
        2.0f / dim.x,
        2.0f / dim.y);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _shaders.use(_shaders.fxaa(), true);
    _textures.bind(srcTexture);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::blitFramebuffer(const glm::ivec2 &dim, Framebuffer &src, int srcColorIdx, Framebuffer &dst, int dstColorIdx, int flags) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.nameGL());
    glReadBuffer(kColorAttachments[srcColorIdx]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    glDrawBuffer(kColorAttachments[dstColorIdx]);

    int flagsGL = 0;
    if (flags & BlitFlags::color) {
        flagsGL |= GL_COLOR_BUFFER_BIT;
    }
    if (flags & BlitFlags::depth) {
        flagsGL |= GL_DEPTH_BUFFER_BIT;
    }
    glBlitFramebuffer(0, 0, dim.x, dim.y, 0, 0, dim.x, dim.y, flagsGL, GL_NEAREST);
}

} // namespace graphics

} // namespace reone
