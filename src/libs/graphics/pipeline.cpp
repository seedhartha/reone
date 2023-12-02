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

#include "reone/graphics/pipeline.h"

#include "reone/graphics/camera/perspective.h"
#include "reone/graphics/context.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshes.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/scene.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/textureregistry.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/scene/node/light.h"
#include "reone/system/randomutil.h"
#include "reone/system/threadutil.h"

#define R_GAUSSIAN_BLUR_HORIZONTAL false
#define R_GAUSSIAN_BLUR_VERTICAL true
#define R_GAUSSIAN_BLUR_STRONG true

#define R_MEDIAN_FILTER_STRONG true

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

static const std::vector<float> g_shadowCascadeDivisors {
    0.005f,
    0.015f,
    0.045f,
    0.135f};

static std::vector<glm::vec4> computeFrustumCornersWorldSpace(const glm::mat4 &projection, const glm::mat4 &view) {
    auto inv = glm::inverse(projection * view);

    std::vector<glm::vec4> corners;
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

    return corners;
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

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
    for (auto &v : corners) {
        auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
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
        throw std::invalid_argument("Invalid cube map face: " + std::to_string(static_cast<int>(face)));
    }
}

void Pipeline::init() {
    checkMainThread();

    // SSAO
    _uniforms.setSSAO([](auto &ssao) {
        for (int i = 0; i < kNumSSAOSamples; ++i) {
            float scale = i / static_cast<float>(kNumSSAOSamples);
            scale = glm::mix(0.1f, 1.0f, scale * scale);
            auto sample = glm::vec3(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f), randomFloat(0.0f, 1.0f));
            sample = glm::normalize(sample);
            sample *= scale;
            ssao.samples[i] = glm::vec4(std::move(sample), 0.0f);
        }
    });
}

void Pipeline::initAttachments(glm::ivec2 dim) {
    Attachments attachments;
    auto halfDim = dim / 2;

    // Reusable attachments

    attachments.dbCommon = std::make_unique<Renderbuffer>();
    attachments.dbCommon->configure(dim.x, dim.y, PixelFormat::Depth32F);
    attachments.dbCommon->init();

    attachments.dbCommonHalf = std::make_unique<Renderbuffer>();
    attachments.dbCommonHalf->configure(halfDim.x, halfDim.y, PixelFormat::Depth32F);
    attachments.dbCommonHalf->init();

    attachments.cbPing = std::make_unique<Texture>("ping_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbPing->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbPing->init();

    attachments.fbPing = std::make_unique<Framebuffer>();
    attachments.fbPing->attachColorDepth(attachments.cbPing, attachments.dbCommon);
    attachments.fbPing->init();

    attachments.cbPingHalf = std::make_unique<Texture>("ping_color_half", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbPingHalf->clear(halfDim.x, halfDim.y, PixelFormat::RGBA8);
    attachments.cbPingHalf->init();

    attachments.fbPingHalf = std::make_unique<Framebuffer>();
    attachments.fbPingHalf->attachColorDepth(attachments.cbPingHalf, attachments.dbCommonHalf);
    attachments.fbPingHalf->init();

    attachments.cbPong = std::make_unique<Texture>("pong_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbPong->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbPong->init();

    attachments.fbPong = std::make_unique<Framebuffer>();
    attachments.fbPong->attachColorDepth(attachments.cbPong, attachments.dbCommon);
    attachments.fbPong->init();

    attachments.cbPongHalf = std::make_unique<Texture>("pong_color_half", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbPongHalf->clear(halfDim.x, halfDim.y, PixelFormat::RGBA8);
    attachments.cbPongHalf->init();

    attachments.fbPongHalf = std::make_unique<Framebuffer>();
    attachments.fbPongHalf->attachColorDepth(attachments.cbPongHalf, attachments.dbCommonHalf);
    attachments.fbPongHalf->init();

    // Directional light shadows framebuffer

    attachments.dbDirectionalLightShadows = std::make_unique<Texture>("point_light_shadows_color", getTextureProperties(TextureUsage::DepthBuffer));
    attachments.dbDirectionalLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F, kNumShadowCascades);
    attachments.dbDirectionalLightShadows->init();

    attachments.fbDirectionalLightShadows = std::make_shared<Framebuffer>();
    attachments.fbDirectionalLightShadows->attachDepth(attachments.dbDirectionalLightShadows);
    attachments.fbDirectionalLightShadows->init();

    // Point light shadows framebuffer

    attachments.dbPointLightShadows = std::make_unique<Texture>("directional_light_shadows_color", getTextureProperties(TextureUsage::DepthBuffer));
    attachments.dbPointLightShadows->setCubemap(true);
    attachments.dbPointLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F);
    attachments.dbPointLightShadows->init();

    attachments.fbPointLightShadows = std::make_shared<Framebuffer>();
    attachments.fbPointLightShadows->attachDepth(attachments.dbPointLightShadows);
    attachments.fbPointLightShadows->init();

    // G-Buffer framebuffer

    attachments.cbGBufferDiffuse = std::make_unique<Texture>("gbuffer_color_diffuse", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbGBufferDiffuse->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbGBufferDiffuse->init();

    attachments.cbGBufferLightmap = std::make_unique<Texture>("gbuffer_color_lightmap", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbGBufferLightmap->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbGBufferLightmap->init();

    attachments.cbGBufferEnvMap = std::make_unique<Texture>("gbuffer_color_envmap", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbGBufferEnvMap->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbGBufferEnvMap->init();

    attachments.cbGBufferSelfIllum = std::make_unique<Texture>("gbuffer_color_selfillum", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbGBufferSelfIllum->clear(dim.x, dim.y, PixelFormat::RGB8);
    attachments.cbGBufferSelfIllum->init();

    attachments.cbGBufferFeatures = std::make_unique<Texture>("gbuffer_color_features", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbGBufferFeatures->clear(dim.x, dim.y, PixelFormat::RG8);
    attachments.cbGBufferFeatures->init();

    attachments.cbGBufferEyePos = std::make_unique<Texture>("gbuffer_color_eyepos", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbGBufferEyePos->clear(dim.x, dim.y, PixelFormat::RGB16F);
    attachments.cbGBufferEyePos->init();

    attachments.cbGBufferEyeNormal = std::make_unique<Texture>("gbuffer_color_eyenormal", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbGBufferEyeNormal->clear(dim.x, dim.y, PixelFormat::RGB8);
    attachments.cbGBufferEyeNormal->init();

    attachments.dbGBuffer = std::make_shared<Renderbuffer>();
    attachments.dbGBuffer->configure(dim.x, dim.y, PixelFormat::Depth32F);
    attachments.dbGBuffer->init();

    attachments.fbGBuffer = std::make_shared<Framebuffer>();
    attachments.fbGBuffer->attachColorsDepth(
        {attachments.cbGBufferDiffuse,
         attachments.cbGBufferLightmap,
         attachments.cbGBufferEnvMap,
         attachments.cbGBufferSelfIllum,
         attachments.cbGBufferFeatures,
         attachments.cbGBufferEyePos,
         attachments.cbGBufferEyeNormal},
        attachments.dbGBuffer);
    attachments.fbGBuffer->init();

    // Opaque geometry framebuffer

    attachments.cbOpaqueGeometry1 = std::make_unique<Texture>("opaque_geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOpaqueGeometry1->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbOpaqueGeometry1->init();

    attachments.cbOpaqueGeometry2 = std::make_unique<Texture>("opaque_geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOpaqueGeometry2->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbOpaqueGeometry2->init();

    attachments.fbOpaqueGeometry = std::make_shared<Framebuffer>();
    attachments.fbOpaqueGeometry->attachColorsDepth({attachments.cbOpaqueGeometry1, attachments.cbOpaqueGeometry2}, attachments.dbCommon);
    attachments.fbOpaqueGeometry->init();

    // Transparent geometry framebuffer

    attachments.cbTransparentGeometry1 = std::make_unique<Texture>("transparent_geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbTransparentGeometry1->clear(dim.x, dim.y, PixelFormat::RGBA16F);
    attachments.cbTransparentGeometry1->init();

    attachments.cbTransparentGeometry2 = std::make_unique<Texture>("transparent_geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbTransparentGeometry2->clear(dim.x, dim.y, PixelFormat::R16F);
    attachments.cbTransparentGeometry2->init();

    attachments.fbTransparentGeometry = std::make_shared<Framebuffer>();
    attachments.fbTransparentGeometry->attachColorsDepth(
        {attachments.cbTransparentGeometry1, attachments.cbTransparentGeometry2},
        attachments.dbCommon);
    attachments.fbTransparentGeometry->init();

    // SSAO framebuffer

    attachments.cbSSAO = std::make_shared<Texture>("ssao_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbSSAO->clear(halfDim.x, halfDim.y, PixelFormat::R8);
    attachments.cbSSAO->init();

    attachments.fbSSAO = std::make_shared<Framebuffer>();
    attachments.fbSSAO->attachColorDepth(attachments.cbSSAO, attachments.dbCommonHalf);
    attachments.fbSSAO->init();

    // SSR framebuffer

    attachments.cbSSR = std::make_unique<Texture>("ssr_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbSSR->clear(halfDim.x, halfDim.y, PixelFormat::RGBA8);
    attachments.cbSSR->init();

    attachments.fbSSR = std::make_shared<Framebuffer>();
    attachments.fbSSR->attachColorDepth(attachments.cbSSR, attachments.dbCommonHalf);
    attachments.fbSSR->init();

    // Output framebuffer

    attachments.cbOutput = std::make_unique<Texture>("output_color", getTextureProperties(TextureUsage::ColorBuffer));
    attachments.cbOutput->clear(dim.x, dim.y, PixelFormat::RGBA8);
    attachments.cbOutput->init();

    attachments.fbOutput = std::make_unique<Framebuffer>();
    attachments.fbOutput->attachColorDepth(attachments.cbOutput, attachments.dbCommon);
    attachments.fbOutput->init();

    // Register attachments

    _attachments[dim] = std::move(attachments);
}

std::shared_ptr<Texture> Pipeline::draw(IScene &scene, const glm::ivec2 &dim) {
    if (!scene.camera()) {
        return nullptr;
    }

    if (_attachments.count(dim) == 0) {
        initAttachments(dim);
    }
    auto &attachments = _attachments.at(dim);

    computeLightSpaceMatrices(scene);

    _graphicsContext.withViewport(glm::ivec4(0, 0, dim.x, dim.y), [this, &scene, &dim, &attachments] {
        // Opaque geometry

        drawShadows(scene, attachments);
        drawOpaqueGeometry(scene, attachments);

        // Screen-space effects

        auto halfDim = dim / 2;
        _graphicsContext.withViewport(glm::ivec4(0, 0, halfDim.x, halfDim.y), [this, &scene, &halfDim, &attachments]() {
            if (_options.ssao) {
                drawSSAO(scene, halfDim, attachments, 0.5f, 0.1f);
                drawBoxBlur(halfDim, *attachments.cbSSAO, *attachments.fbPingHalf);
                blitFramebuffer(halfDim, *attachments.fbPingHalf, 0, *attachments.fbSSAO, 0);
            }
            if (_options.ssr) {
                drawSSR(scene, halfDim, attachments, 0.25f, 4.0f, 64.0f);
                drawGaussianBlur(halfDim, *attachments.cbSSR, *attachments.fbPingHalf, R_GAUSSIAN_BLUR_HORIZONTAL, R_GAUSSIAN_BLUR_STRONG);
                drawGaussianBlur(halfDim, *attachments.cbPingHalf, *attachments.fbSSR, R_GAUSSIAN_BLUR_VERTICAL, R_GAUSSIAN_BLUR_STRONG);
            }
        });

        // END Screen-space effects

        drawCombineOpaque(scene, attachments, *attachments.fbOpaqueGeometry);
        drawGaussianBlur(dim, *attachments.cbOpaqueGeometry2, *attachments.fbPing, R_GAUSSIAN_BLUR_HORIZONTAL, R_GAUSSIAN_BLUR_STRONG);
        drawGaussianBlur(dim, *attachments.cbPing, *attachments.fbPong, R_GAUSSIAN_BLUR_VERTICAL, R_GAUSSIAN_BLUR_STRONG);
        blitFramebuffer(dim, *attachments.fbPong, 0, *attachments.fbOpaqueGeometry, 1);

        // END Opaque geometry

        // Transparent geometry

        blitFramebuffer(dim, *attachments.fbGBuffer, 0, *attachments.fbTransparentGeometry, 0, BlitFlags::depth);
        drawTransparentGeometry(scene, attachments);
        drawCombineGeometry(attachments, *attachments.fbOutput);
        drawLensFlares(scene, *attachments.fbOutput);

        // END Transparent geometry

        if (_options.fxaa && _options.sharpen) {
            drawFXAA(dim, *attachments.cbOutput, *attachments.fbPing);
            drawSharpen(dim, *attachments.cbPing, *attachments.fbPong, 0.25f);
            blitFramebuffer(dim, *attachments.fbPong, 0, *attachments.fbOutput, 0);
        } else if (_options.fxaa) {
            drawFXAA(dim, *attachments.cbOutput, *attachments.fbPing);
            blitFramebuffer(dim, *attachments.fbPing, 0, *attachments.fbOutput, 0);
        } else if (_options.sharpen) {
            drawSharpen(dim, *attachments.cbOutput, *attachments.fbPing, 0.25f);
            blitFramebuffer(dim, *attachments.fbPing, 0, *attachments.fbOutput, 0);
        }
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return attachments.cbOutput;
}

void Pipeline::drawShadows(IScene &scene, Attachments &attachments) {
    if (!scene.hasShadowLight()) {
        return;
    }

    _uniforms.setGeneral([this, &scene](auto &general) {
        general.resetGlobals();
        general.shadowLightPosition = glm::vec4(scene.shadowLightPosition(), scene.isShadowLightDirectional() ? 0.0f : 1.0f);
        for (int i = 0; i < kNumShadowLightSpace; ++i) {
            general.shadowLightSpace[i] = _shadowLightSpace[i];
        }
    });

    auto framebuffer = scene.isShadowLightDirectional() ? attachments.fbDirectionalLightShadows : attachments.fbPointLightShadows;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->nameGL());
    glDrawBuffer(GL_NONE);
    _graphicsContext.withViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution), [this, &scene]() {
        _graphicsContext.clearDepth();
        scene.drawShadows();
    });
}

void Pipeline::drawOpaqueGeometry(IScene &scene, Attachments &attachments) {
    auto camera = scene.camera();

    _uniforms.setGeneral([&camera](auto &general) {
        general.resetGlobals();
        general.projection = camera->projection();
        general.view = camera->view();
        general.viewInv = glm::inverse(camera->view());
        general.cameraPosition = glm::vec4(camera->position(), 1.0f);
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbGBuffer->nameGL());
    glDrawBuffers(7, kColorAttachments);
    _graphicsContext.clearColorDepth();
    scene.drawOpaque();
}

void Pipeline::drawTransparentGeometry(IScene &scene, Attachments &attachments) {
    auto camera = scene.camera();

    _uniforms.setGeneral([&camera](auto &general) {
        general.resetGlobals();
        general.projection = camera->projection();
        general.view = camera->view();
        general.viewInv = glm::inverse(camera->view());
        general.cameraPosition = glm::vec4(camera->position(), 1.0f);
        general.clipNear = camera->zNear();
        general.clipFar = camera->zFar();
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbTransparentGeometry->nameGL());
    glDrawBuffers(2, kColorAttachments);
    _graphicsContext.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
    glDepthMask(GL_FALSE);
    _graphicsContext.withBlending(BlendMode::OIT_Transparent, [&scene] {
        scene.drawTransparent();
    });
    glDepthMask(GL_TRUE);
}

void Pipeline::drawLensFlares(IScene &scene, Framebuffer &dst) {
    auto camera = scene.camera();

    _uniforms.setGeneral([&camera](auto &general) {
        general.resetGlobals();
        general.projection = camera->projection();
        general.view = camera->view();
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    scene.drawLensFlares();
}

void Pipeline::drawSSAO(IScene &scene, const glm::ivec2 &dim, Attachments &attachments, float sampleRadius, float bias) {
    auto camera = scene.camera();

    _uniforms.setGeneral([&dim, &sampleRadius, &bias, &camera](auto &general) {
        general.resetGlobals();
        general.resetLocals();
        general.projection = camera->projection();
        general.screenResolution = glm::vec2(static_cast<float>(dim.x), static_cast<float>(dim.y));
        general.ssaoSampleRadius = sampleRadius;
        general.ssaoBias = bias;
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbSSAO->nameGL());
    glDrawBuffer(kColorAttachments[0]);
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::ssao));
    _graphicsContext.bind(*attachments.cbGBufferEyePos, TextureUnits::eyePos);
    _graphicsContext.bind(*attachments.cbGBufferEyeNormal, TextureUnits::eyeNormal);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawSSR(IScene &scene, const glm::ivec2 &dim, Attachments &attachments, float bias, float pixelStride, float maxSteps) {
    auto camera = scene.camera();
    auto screenProjection = glm::mat4(1.0f);
    screenProjection *= glm::scale(glm::vec3(dim.x, dim.y, 1.0f));
    screenProjection *= glm::translate(glm::vec3(0.5f, 0.5f, 0.0f));
    screenProjection *= glm::scale(glm::vec3(0.5f, 0.5f, 1.0f));
    screenProjection *= camera->projection();

    _uniforms.setGeneral([&dim, &bias, &pixelStride, &maxSteps, &camera, screenProjection](auto &general) {
        general.resetGlobals();
        general.resetLocals();
        general.screenProjection = std::move(screenProjection);
        general.screenResolution = glm::vec2(dim.x, dim.y);
        general.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
        general.clipNear = camera->zNear();
        general.clipFar = camera->zFar();
        general.ssrBias = bias;
        general.ssrPixelStride = pixelStride;
        general.ssrMaxSteps = maxSteps;
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbSSR->nameGL());
    glDrawBuffer(kColorAttachments[0]);
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::ssr));
    _graphicsContext.bind(*attachments.cbGBufferDiffuse);
    _graphicsContext.bind(*attachments.cbGBufferLightmap, TextureUnits::lightmap);
    _graphicsContext.bind(*attachments.cbGBufferEnvMap, TextureUnits::envmapColor);
    _graphicsContext.bind(*attachments.cbGBufferEyePos, TextureUnits::eyePos);
    _graphicsContext.bind(*attachments.cbGBufferEyeNormal, TextureUnits::eyeNormal);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawCombineOpaque(IScene &scene, Attachments &attachments, Framebuffer &dst) {
    auto camera = scene.camera();
    _uniforms.setGeneral([this, &scene, &camera](auto &general) {
        general.resetGlobals();
        general.resetLocals();
        general.viewInv = glm::inverse(camera->view());
        general.cameraPosition = glm::vec4(camera->position(), 1.0f);
        general.worldAmbientColor = glm::vec4(scene.ambientLightColor(), 1.0f);
        if (scene.hasShadowLight()) {
            general.featureMask |= UniformsFeatureFlags::shadows;
            general.shadowLightPosition = glm::vec4(scene.shadowLightPosition(), scene.isShadowLightDirectional() ? 0.0f : 1.0f);
            general.shadowStrength = scene.shadowStrength();
            general.shadowRadius = scene.shadowRadius();
            general.shadowCascadeFarPlanes = _shadowCascadeFarPlanes;
            for (int i = 0; i < kNumShadowLightSpace; ++i) {
                general.shadowLightSpace[i] = _shadowLightSpace[i];
            }
        }
        if (scene.isFogEnabled()) {
            general.featureMask |= UniformsFeatureFlags::fog;
            general.fogNear = scene.fogNear();
            general.fogFar = scene.fogFar();
            general.fogColor = glm::vec4(scene.fogColor(), 1.0f);
        }
    });
    scene.fillLightingUniforms();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    glDrawBuffers(2, kColorAttachments);
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::combineOpaque));
    _graphicsContext.bind(*attachments.cbGBufferDiffuse);
    _graphicsContext.bind(*attachments.cbGBufferLightmap, TextureUnits::lightmap);
    _graphicsContext.bind(*attachments.cbGBufferEnvMap, TextureUnits::envmapColor);
    _graphicsContext.bind(*attachments.cbGBufferSelfIllum, TextureUnits::selfIllumColor);
    _graphicsContext.bind(*attachments.cbGBufferFeatures, TextureUnits::features);
    _graphicsContext.bind(*attachments.cbGBufferEyePos, TextureUnits::eyePos);
    _graphicsContext.bind(*attachments.cbGBufferEyeNormal, TextureUnits::eyeNormal);
    _graphicsContext.bind(_options.ssao ? *attachments.cbSSAO : _textureRegistry.get(TextureName::SsaoRgb), TextureUnits::ssao);
    _graphicsContext.bind(_options.ssr ? *attachments.cbSSR : _textureRegistry.get(TextureName::SsrRgba), TextureUnits::ssr);
    if (scene.hasShadowLight()) {
        if (scene.isShadowLightDirectional()) {
            _graphicsContext.bind(*attachments.dbDirectionalLightShadows, TextureUnits::shadowMapArray);
        } else {
            _graphicsContext.bind(*attachments.dbPointLightShadows, TextureUnits::shadowMapCube);
        }
    }
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawCombineGeometry(Attachments &attachments, Framebuffer &dst) {
    _uniforms.setGeneral([](auto &general) {
        general.resetGlobals();
        general.resetLocals();
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::combineGeometry));
    _graphicsContext.bind(*attachments.cbOpaqueGeometry1);
    _graphicsContext.bind(*attachments.cbOpaqueGeometry2, TextureUnits::hilights);
    _graphicsContext.bind(*attachments.cbTransparentGeometry1, TextureUnits::oitAccum);
    _graphicsContext.bind(*attachments.cbTransparentGeometry2, TextureUnits::oitRevealage);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawBoxBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst) {
    _uniforms.setGeneral([](auto &general) {
        general.resetGlobals();
        general.resetLocals();
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::boxBlur4));
    _graphicsContext.bind(srcTexture);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawGaussianBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool vertical, bool strong) {
    _uniforms.setGeneral([&dim, &vertical](auto &general) {
        general.resetGlobals();
        general.resetLocals();
        general.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
        general.blurDirection = vertical ? glm::vec2(0.0f, 1.0f) : glm::vec2(1.0f, 0.0f);
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _graphicsContext.useProgram(_shaderRegistry.get(strong ? ShaderProgramId::gaussianBlur13 : ShaderProgramId::gaussianBlur9));
    _graphicsContext.bind(srcTexture);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawMedianFilter(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool strong) {
    _uniforms.setGeneral([](auto &general) {
        general.resetGlobals();
        general.resetLocals();
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _graphicsContext.useProgram(_shaderRegistry.get(strong ? ShaderProgramId::medianFilter5 : ShaderProgramId::medianFilter3));
    _graphicsContext.bind(srcTexture);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawFXAA(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst) {
    _uniforms.setGeneral([&dim](auto &general) {
        general.resetGlobals();
        general.resetLocals();
        general.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::fxaa));
    _graphicsContext.bind(srcTexture);
    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void Pipeline::drawSharpen(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, float amount) {
    _uniforms.setGeneral([&dim, &amount](auto &general) {
        general.resetGlobals();
        general.resetLocals();
        general.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
        general.sharpenAmount = amount;
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _graphicsContext.useProgram(_shaderRegistry.get(ShaderProgramId::sharpen));
    _graphicsContext.bind(srcTexture);
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

void Pipeline::computeLightSpaceMatrices(IScene &scene) {
    if (!scene.hasShadowLight()) {
        return;
    }
    if (scene.isShadowLightDirectional()) {
        auto camera = std::static_pointer_cast<PerspectiveCamera>(scene.camera());
        auto lightDir = glm::normalize(camera->position() - scene.shadowLightPosition());
        float fovy = camera->fovy();
        float aspect = camera->aspect();
        float cameraNear = camera->zNear();
        float cameraFar = camera->zFar();
        for (int i = 0; i < kNumShadowCascades; ++i) {
            float far = cameraFar * g_shadowCascadeDivisors[i];
            float near = cameraNear;
            if (i > 0) {
                near = cameraFar * g_shadowCascadeDivisors[i - 1];
            }
            _shadowLightSpace[i] = computeDirectionalLightSpaceMatrix(fovy, aspect, near, far, lightDir, camera->view());
            _shadowCascadeFarPlanes[i] = far;
        }
    } else {
        glm::mat4 projection(glm::perspective(kPointLightShadowsFOV, 1.0f, kPointLightShadowsNearPlane, kPointLightShadowsFarPlane));
        for (int i = 0; i < kNumCubeFaces; ++i) {
            glm::mat4 lightView(getPointLightView(scene.shadowLightPosition(), static_cast<CubeMapFace>(i)));
            _shadowLightSpace[i] = projection * lightView;
        }
    }
}

} // namespace graphics

} // namespace reone
