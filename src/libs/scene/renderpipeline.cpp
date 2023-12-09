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

#include "reone/scene/renderpipeline.h"

#include "reone/graphics/context.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textureregistry.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/system/checkutil.h"
#include "reone/system/randomutil.h"
#include "reone/system/threadutil.h"

#define R_GAUSSIAN_BLUR_HORIZONTAL false
#define R_GAUSSIAN_BLUR_VERTICAL true
#define R_GAUSSIAN_BLUR_STRONG true

#define R_MEDIAN_FILTER_STRONG true

using namespace reone::graphics;

namespace reone {

namespace scene {

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

void RenderPipeline::init() {
    checkThat(!_inited, "Pipeline already initialized");
    checkMainThread();

    initRenderTargets();
    initSSAOSamples();

    _inited = true;
}

void RenderPipeline::initRenderTargets() {
    auto halfSize = _targetSize / 2;

    // Reusable targets

    _renderTargets.dbCommon = std::make_unique<Renderbuffer>();
    _renderTargets.dbCommon->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth32F);
    _renderTargets.dbCommon->init();

    _renderTargets.dbCommonHalf = std::make_unique<Renderbuffer>();
    _renderTargets.dbCommonHalf->configure(halfSize.x, halfSize.y, PixelFormat::Depth32F);
    _renderTargets.dbCommonHalf->init();

    _renderTargets.cbPing = std::make_unique<Texture>("ping_color", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbPing->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbPing->init();

    _renderTargets.fbPing = std::make_unique<Framebuffer>();
    _renderTargets.fbPing->attachColorDepth(_renderTargets.cbPing, _renderTargets.dbCommon);
    _renderTargets.fbPing->init();

    _renderTargets.cbPingHalf = std::make_unique<Texture>("ping_color_half", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbPingHalf->clear(halfSize.x, halfSize.y, PixelFormat::RGBA8);
    _renderTargets.cbPingHalf->init();

    _renderTargets.fbPingHalf = std::make_unique<Framebuffer>();
    _renderTargets.fbPingHalf->attachColorDepth(_renderTargets.cbPingHalf, _renderTargets.dbCommonHalf);
    _renderTargets.fbPingHalf->init();

    _renderTargets.cbPong = std::make_unique<Texture>("pong_color", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbPong->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbPong->init();

    _renderTargets.fbPong = std::make_unique<Framebuffer>();
    _renderTargets.fbPong->attachColorDepth(_renderTargets.cbPong, _renderTargets.dbCommon);
    _renderTargets.fbPong->init();

    _renderTargets.cbPongHalf = std::make_unique<Texture>("pong_color_half", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbPongHalf->clear(halfSize.x, halfSize.y, PixelFormat::RGBA8);
    _renderTargets.cbPongHalf->init();

    _renderTargets.fbPongHalf = std::make_unique<Framebuffer>();
    _renderTargets.fbPongHalf->attachColorDepth(_renderTargets.cbPongHalf, _renderTargets.dbCommonHalf);
    _renderTargets.fbPongHalf->init();

    // Directional light shadows framebuffer

    _renderTargets.dbDirectionalLightShadows = std::make_unique<Texture>("point_light_shadows_color", getTextureProperties(TextureUsage::DepthBuffer));
    _renderTargets.dbDirectionalLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F, kNumShadowCascades);
    _renderTargets.dbDirectionalLightShadows->init();

    _renderTargets.fbDirLightShadows = std::make_shared<Framebuffer>();
    _renderTargets.fbDirLightShadows->attachDepth(_renderTargets.dbDirectionalLightShadows);
    _renderTargets.fbDirLightShadows->init();

    // Point light shadows framebuffer

    _renderTargets.dbPointLightShadows = std::make_unique<Texture>("directional_light_shadows_color", getTextureProperties(TextureUsage::DepthBuffer));
    _renderTargets.dbPointLightShadows->setCubemap(true);
    _renderTargets.dbPointLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F);
    _renderTargets.dbPointLightShadows->init();

    _renderTargets.fbPointLightShadows = std::make_shared<Framebuffer>();
    _renderTargets.fbPointLightShadows->attachDepth(_renderTargets.dbPointLightShadows);
    _renderTargets.fbPointLightShadows->init();

    // G-Buffer framebuffer

    _renderTargets.cbGBufferDiffuse = std::make_unique<Texture>("gbuffer_color_diffuse", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufferDiffuse->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbGBufferDiffuse->init();

    _renderTargets.cbGBufferLightmap = std::make_unique<Texture>("gbuffer_color_lightmap", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufferLightmap->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbGBufferLightmap->init();

    _renderTargets.cbGBufferEnvMap = std::make_unique<Texture>("gbuffer_color_envmap", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufferEnvMap->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbGBufferEnvMap->init();

    _renderTargets.cbGBufferSelfIllum = std::make_unique<Texture>("gbuffer_color_selfillum", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufferSelfIllum->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB8);
    _renderTargets.cbGBufferSelfIllum->init();

    _renderTargets.cbGBufferFeatures = std::make_unique<Texture>("gbuffer_color_features", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufferFeatures->clear(_targetSize.x, _targetSize.y, PixelFormat::RG8);
    _renderTargets.cbGBufferFeatures->init();

    _renderTargets.cbGBufferEyePos = std::make_unique<Texture>("gbuffer_color_eyepos", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufferEyePos->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB16F);
    _renderTargets.cbGBufferEyePos->init();

    _renderTargets.cbGBufferEyeNormal = std::make_unique<Texture>("gbuffer_color_eyenormal", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufferEyeNormal->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB8);
    _renderTargets.cbGBufferEyeNormal->init();

    _renderTargets.dbGBuffer = std::make_shared<Renderbuffer>();
    _renderTargets.dbGBuffer->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth32F);
    _renderTargets.dbGBuffer->init();

    _renderTargets.fbOpaqueGeometry = std::make_shared<Framebuffer>();
    _renderTargets.fbOpaqueGeometry->attachColorsDepth(
        {_renderTargets.cbGBufferDiffuse,
         _renderTargets.cbGBufferLightmap,
         _renderTargets.cbGBufferEnvMap,
         _renderTargets.cbGBufferSelfIllum,
         _renderTargets.cbGBufferFeatures,
         _renderTargets.cbGBufferEyePos,
         _renderTargets.cbGBufferEyeNormal},
        _renderTargets.dbGBuffer);
    _renderTargets.fbOpaqueGeometry->init();

    // Opaque geometry framebuffer

    _renderTargets.cbDeferredOpaque1 = std::make_unique<Texture>("deferred_opaque_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbDeferredOpaque1->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbDeferredOpaque1->init();

    _renderTargets.cbDeferredOpaque2 = std::make_unique<Texture>("deferred_opaque_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbDeferredOpaque2->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbDeferredOpaque2->init();

    _renderTargets.fbDeferredCombine = std::make_shared<Framebuffer>();
    _renderTargets.fbDeferredCombine->attachColorsDepth({_renderTargets.cbDeferredOpaque1, _renderTargets.cbDeferredOpaque2}, _renderTargets.dbCommon);
    _renderTargets.fbDeferredCombine->init();

    // Transparent geometry framebuffer

    _renderTargets.cbTransparentGeometry1 = std::make_unique<Texture>("transparent_geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbTransparentGeometry1->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA16F);
    _renderTargets.cbTransparentGeometry1->init();

    _renderTargets.cbTransparentGeometry2 = std::make_unique<Texture>("transparent_geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbTransparentGeometry2->clear(_targetSize.x, _targetSize.y, PixelFormat::R16F);
    _renderTargets.cbTransparentGeometry2->init();

    _renderTargets.fbTransparentGeometry = std::make_shared<Framebuffer>();
    _renderTargets.fbTransparentGeometry->attachColorsDepth(
        {_renderTargets.cbTransparentGeometry1, _renderTargets.cbTransparentGeometry2},
        _renderTargets.dbCommon);
    _renderTargets.fbTransparentGeometry->init();

    // SSAO framebuffer

    _renderTargets.cbSSAO = std::make_shared<Texture>("ssao_color", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbSSAO->clear(halfSize.x, halfSize.y, PixelFormat::R8);
    _renderTargets.cbSSAO->init();

    _renderTargets.fbSSAO = std::make_shared<Framebuffer>();
    _renderTargets.fbSSAO->attachColorDepth(_renderTargets.cbSSAO, _renderTargets.dbCommonHalf);
    _renderTargets.fbSSAO->init();

    // SSR framebuffer

    _renderTargets.cbSSR = std::make_unique<Texture>("ssr_color", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbSSR->clear(halfSize.x, halfSize.y, PixelFormat::RGBA8);
    _renderTargets.cbSSR->init();

    _renderTargets.fbSSR = std::make_shared<Framebuffer>();
    _renderTargets.fbSSR->attachColorDepth(_renderTargets.cbSSR, _renderTargets.dbCommonHalf);
    _renderTargets.fbSSR->init();

    // Output framebuffer

    _renderTargets.cbOutput = std::make_unique<Texture>("output_color", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbOutput->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbOutput->init();

    _renderTargets.fbOutput = std::make_unique<Framebuffer>();
    _renderTargets.fbOutput->attachColorDepth(_renderTargets.cbOutput, _renderTargets.dbCommon);
    _renderTargets.fbOutput->init();
}

void RenderPipeline::initSSAOSamples() {
    _uniforms.setScreenEffect([](auto &screenEffect) {
        for (int i = 0; i < kNumSSAOSamples; ++i) {
            float scale = i / static_cast<float>(kNumSSAOSamples);
            scale = glm::mix(0.1f, 1.0f, scale * scale);
            auto sample = glm::vec3(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f), randomFloat(0.0f, 1.0f));
            sample = glm::normalize(sample);
            sample *= scale;
            screenEffect.ssaoSamples[i] = glm::vec4(std::move(sample), 0.0f);
        }
    });
}

void RenderPipeline::drawSSAO(const glm::ivec2 &dim, float sampleRadius, float bias) {
    _uniforms.setScreenEffect([&dim, &sampleRadius, &bias](auto &screenEffect) {
        screenEffect.screenResolution = glm::vec2(static_cast<float>(dim.x), static_cast<float>(dim.y));
        screenEffect.ssaoSampleRadius = sampleRadius;
        screenEffect.ssaoBias = bias;
    });
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbSSAO->nameGL());
    glDrawBuffer(kColorAttachments[0]);
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::deferredSSAO));
    _context.bindTexture(*_renderTargets.cbGBufferEyePos, TextureUnits::eyePos);
    _context.bindTexture(*_renderTargets.cbGBufferEyeNormal, TextureUnits::eyeNormal);
    _context.bindTexture(_textureRegistry.get(TextureName::noiseRg), TextureUnits::noise);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::drawSSR(const glm::ivec2 &dim, float bias, float pixelStride, float maxSteps) {
    _uniforms.setScreenEffect([&](auto &screenEffect) {
        screenEffect.screenResolution = glm::vec2(dim.x, dim.y);
        screenEffect.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
        screenEffect.ssrBias = bias;
        screenEffect.ssrPixelStride = pixelStride;
        screenEffect.ssrMaxSteps = maxSteps;
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbSSR->nameGL());
    glDrawBuffer(kColorAttachments[0]);
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::deferredSSR));
    _context.bindTexture(*_renderTargets.cbGBufferDiffuse);
    _context.bindTexture(*_renderTargets.cbGBufferLightmap, TextureUnits::lightmap);
    _context.bindTexture(*_renderTargets.cbGBufferEnvMap, TextureUnits::envmapColor);
    _context.bindTexture(*_renderTargets.cbGBufferEyePos, TextureUnits::eyePos);
    _context.bindTexture(*_renderTargets.cbGBufferEyeNormal, TextureUnits::eyeNormal);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::drawOITBlend(Framebuffer &dst) {
    _uniforms.setGlobals([](auto &globals) {
        globals.reset();
    });
    _uniforms.setLocals([](auto &locals) {
        locals.reset();
    });
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::oitBlend));
    _context.bindTexture(*_renderTargets.cbDeferredOpaque1);
    _context.bindTexture(*_renderTargets.cbDeferredOpaque2, TextureUnits::hilights);
    _context.bindTexture(*_renderTargets.cbTransparentGeometry1, TextureUnits::oitAccum);
    _context.bindTexture(*_renderTargets.cbTransparentGeometry2, TextureUnits::oitRevealage);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::drawBoxBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::postBoxBlur4));
    _context.bindTexture(srcTexture);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::drawGaussianBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool vertical, bool strong) {
    _uniforms.setScreenEffect([&dim, &vertical](auto &screenEffect) {
        screenEffect.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
        screenEffect.blurDirection = vertical ? glm::vec2(0.0f, 1.0f) : glm::vec2(1.0f, 0.0f);
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _context.useProgram(_shaderRegistry.get(strong ? ShaderProgramId::postGaussianBlur13 : ShaderProgramId::postGaussianBlur9));
    _context.bindTexture(srcTexture);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::drawMedianFilter(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool strong) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _context.useProgram(_shaderRegistry.get(strong ? ShaderProgramId::postMedianFilter5 : ShaderProgramId::postMedianFilter3));
    _context.bindTexture(srcTexture);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::drawFXAA(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst) {
    _uniforms.setGlobals([](auto &globals) {
        globals.reset();
    });
    _uniforms.setLocals([](auto &locals) {
        locals.reset();
    });
    _uniforms.setScreenEffect([&dim](auto &screenEffect) {
        screenEffect.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
    });
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::postFXAA));
    _context.bindTexture(srcTexture);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::drawSharpen(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, float amount) {
    _uniforms.setScreenEffect([&dim, &amount](auto &screenEffect) {
        screenEffect.screenResolutionRcp = glm::vec2(1.0f / static_cast<float>(dim.x), 1.0f / static_cast<float>(dim.y));
        screenEffect.sharpenAmount = amount;
    });

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.nameGL());
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::postSharpen));
    _context.bindTexture(srcTexture);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipeline::blitFramebuffer(const glm::ivec2 &dim, Framebuffer &src, int srcColorIdx, Framebuffer &dst, int dstColorIdx, int flags) {
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

void RenderPipeline::inRenderPass(RenderPassName name, std::function<void(IRenderPass &)> block) {
    checkEqual("Current pass", _passName, RenderPassName::None);
    checkNotEqual("New pass", name, RenderPassName::None);

    auto pass = RenderPass(
        _context,
        _shaderRegistry,
        _meshRegistry,
        _textureRegistry,
        _uniforms);
    switch (name) {
    case RenderPassName::DirLightShadowsPass:
        beginDirLightShadowsPass();
        block(pass);
        endDirLightShadowsPass();
        break;
    case RenderPassName::PointLightShadows:
        beginPointLightShadowsPass();
        block(pass);
        endPointLightShadowsPass();
        break;
    case RenderPassName::OpaqueGeometry:
        beginOpaqueGeometryPass();
        block(pass);
        endOpaqueGeometryPass();
        break;
    case RenderPassName::TransparentGeometry:
        beginTransparentGeometryPass();
        block(pass);
        endTransparentGeometryPass();
        break;
    case RenderPassName::PostProcessing:
        beginPostProcessingPass();
        block(pass);
        endPostProcessingPass();
        break;
    default:
        throw std::invalid_argument("Unexpected new render pass");
    }
}

void RenderPipeline::beginDirLightShadowsPass() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbDirLightShadows->nameGL());
    glDrawBuffer(GL_NONE);
    _context.pushViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution));
    _context.clearDepth();
}

void RenderPipeline::beginPointLightShadowsPass() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbPointLightShadows->nameGL());
    glDrawBuffer(GL_NONE);
    _context.pushViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution));
    _context.clearDepth();
}

void RenderPipeline::beginOpaqueGeometryPass() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbOpaqueGeometry->nameGL());
    glDrawBuffers(7, kColorAttachments);
    _context.clearColorDepth();
}

void RenderPipeline::beginTransparentGeometryPass() {
    blitFramebuffer(_targetSize, *_renderTargets.fbOpaqueGeometry, 0, *_renderTargets.fbTransparentGeometry, 0, BlitFlags::depth);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbTransparentGeometry->nameGL());
    glDrawBuffers(2, kColorAttachments);
    glDepthMask(GL_FALSE);
    _context.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
    _context.pushBlending(BlendMode::OIT_Transparent);
}

void RenderPipeline::beginPostProcessingPass() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbOutput->nameGL());
}

void RenderPipeline::endDirLightShadowsPass() {
    _context.popViewport();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderPipeline::endPointLightShadowsPass() {
    _context.popViewport();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderPipeline::endOpaqueGeometryPass() {
    auto halfSize = _targetSize / 2;
    _context.withViewport(glm::ivec4(0, 0, halfSize.x, halfSize.y), [this, &halfSize]() {
        if (_options.ssao) {
            drawSSAO(halfSize, 0.5f, 0.1f);
            drawBoxBlur(halfSize, *_renderTargets.cbSSAO, *_renderTargets.fbPingHalf);
            blitFramebuffer(halfSize, *_renderTargets.fbPingHalf, 0, *_renderTargets.fbSSAO, 0);
        }
        if (_options.ssr) {
            drawSSR(halfSize, 0.25f, 4.0f, 64.0f);
            drawGaussianBlur(halfSize, *_renderTargets.cbSSR, *_renderTargets.fbPingHalf, R_GAUSSIAN_BLUR_HORIZONTAL, R_GAUSSIAN_BLUR_STRONG);
            drawGaussianBlur(halfSize, *_renderTargets.cbPingHalf, *_renderTargets.fbSSR, R_GAUSSIAN_BLUR_VERTICAL, R_GAUSSIAN_BLUR_STRONG);
        }
    });

    _context.bindTexture(*_renderTargets.cbGBufferDiffuse);
    _context.bindTexture(*_renderTargets.cbGBufferLightmap, TextureUnits::lightmap);
    _context.bindTexture(*_renderTargets.cbGBufferEnvMap, TextureUnits::envmapColor);
    _context.bindTexture(*_renderTargets.cbGBufferSelfIllum, TextureUnits::selfIllumColor);
    _context.bindTexture(*_renderTargets.cbGBufferFeatures, TextureUnits::features);
    _context.bindTexture(*_renderTargets.cbGBufferEyePos, TextureUnits::eyePos);
    _context.bindTexture(*_renderTargets.cbGBufferEyeNormal, TextureUnits::eyeNormal);
    _context.bindTexture(_options.ssao ? *_renderTargets.cbSSAO : _textureRegistry.get(TextureName::ssaoRgb), TextureUnits::ssao);
    _context.bindTexture(_options.ssr ? *_renderTargets.cbSSR : _textureRegistry.get(TextureName::ssrRgb), TextureUnits::ssr);
    _context.bindTexture(*_renderTargets.dbDirectionalLightShadows, TextureUnits::shadowMapArray);
    _context.bindTexture(*_renderTargets.dbPointLightShadows, TextureUnits::shadowMapCube);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTargets.fbDeferredCombine->nameGL());
    glDrawBuffers(2, kColorAttachments);
    _context.clearColorDepth();
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::deferredCombine));
    _meshRegistry.get(MeshName::quadNDC).draw();

    drawGaussianBlur(_targetSize, *_renderTargets.cbDeferredOpaque2, *_renderTargets.fbPing, R_GAUSSIAN_BLUR_HORIZONTAL, R_GAUSSIAN_BLUR_STRONG);
    drawGaussianBlur(_targetSize, *_renderTargets.cbPing, *_renderTargets.fbPong, R_GAUSSIAN_BLUR_VERTICAL, R_GAUSSIAN_BLUR_STRONG);
    blitFramebuffer(_targetSize, *_renderTargets.fbPong, 0, *_renderTargets.fbDeferredCombine, 1);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderPipeline::endTransparentGeometryPass() {
    _context.popBlending();
    glDepthMask(GL_TRUE);
    drawOITBlend(*_renderTargets.fbOutput);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderPipeline::endPostProcessingPass() {
    if (_options.fxaa && _options.sharpen) {
        drawFXAA(_targetSize, *_renderTargets.cbOutput, *_renderTargets.fbPing);
        drawSharpen(_targetSize, *_renderTargets.cbPing, *_renderTargets.fbPong, 0.25f);
        blitFramebuffer(_targetSize, *_renderTargets.fbPong, 0, *_renderTargets.fbOutput, 0);
    } else if (_options.fxaa) {
        drawFXAA(_targetSize, *_renderTargets.cbOutput, *_renderTargets.fbPing);
        blitFramebuffer(_targetSize, *_renderTargets.fbPing, 0, *_renderTargets.fbOutput, 0);
    } else if (_options.sharpen) {
        drawSharpen(_targetSize, *_renderTargets.cbOutput, *_renderTargets.fbPing, 0.25f);
        blitFramebuffer(_targetSize, *_renderTargets.fbPing, 0, *_renderTargets.fbOutput, 0);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

Texture &RenderPipeline::output() {
    return *_renderTargets.cbOutput;
}

} // namespace scene

} // namespace reone
