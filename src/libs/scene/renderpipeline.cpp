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
#include "reone/graphics/pbrtextures.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textureregistry.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/system/checkutil.h"
#include "reone/system/randomutil.h"
#include "reone/system/threadutil.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kSSAOSampleRadius = 0.5f;
static constexpr float kSSAOBias = 0.1f;

static constexpr float kSSRBias = 0.25f;
static constexpr float kSSRPixelStride = 4.0f;
static constexpr float kSSRMaxSteps = 64.0f;

static constexpr float kSharpenAmount = 0.25f;

void RenderPipelineBase::init() {
    checkThat(!_inited, "Pipeline already initialized");
    checkMainThread();

    initRenderTargets();
    if (_options.ssao) {
        initSSAOSamples();
    }

    _inited = true;
}

void RenderPipelineBase::initRenderTargets() {
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

    _renderTargets.cbGBufDiffuse = std::make_unique<Texture>("gbuffer_color_diffuse", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufDiffuse->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbGBufDiffuse->init();

    _renderTargets.cbGBufLightmap = std::make_unique<Texture>("gbuffer_color_lightmap", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufLightmap->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbGBufLightmap->init();

    _renderTargets.cbGBufEnvMap = std::make_unique<Texture>("gbuffer_color_envmap", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufEnvMap->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA16F);
    _renderTargets.cbGBufEnvMap->init();

    _renderTargets.cbGBufSelfIllum = std::make_unique<Texture>("gbuffer_color_selfillum", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufSelfIllum->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB8);
    _renderTargets.cbGBufSelfIllum->init();

    _renderTargets.cbGBufFeatures = std::make_unique<Texture>("gbuffer_color_features", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufFeatures->clear(_targetSize.x, _targetSize.y, PixelFormat::RG8);
    _renderTargets.cbGBufFeatures->init();

    _renderTargets.cbGBufEyePos = std::make_unique<Texture>("gbuffer_color_eyepos", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufEyePos->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB16F);
    _renderTargets.cbGBufEyePos->init();

    _renderTargets.cbGBufEyeNormal = std::make_unique<Texture>("gbuffer_color_eyenormal", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufEyeNormal->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB8);
    _renderTargets.cbGBufEyeNormal->init();

    _renderTargets.cbGBufPBRIrradiance = std::make_unique<Texture>("gbuffer_color_pbr_irradiance", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbGBufPBRIrradiance->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB16F);
    _renderTargets.cbGBufPBRIrradiance->init();

    _renderTargets.dbGBuffer = std::make_shared<Renderbuffer>();
    _renderTargets.dbGBuffer->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth32F);
    _renderTargets.dbGBuffer->init();

    _renderTargets.fbOpaqueGeometry = std::make_shared<Framebuffer>();
    _renderTargets.fbOpaqueGeometry->attachColorsDepth(
        {_renderTargets.cbGBufDiffuse,
         _renderTargets.cbGBufLightmap,
         _renderTargets.cbGBufEnvMap,
         _renderTargets.cbGBufSelfIllum,
         _renderTargets.cbGBufFeatures,
         _renderTargets.cbGBufEyePos,
         _renderTargets.cbGBufEyeNormal,
         _renderTargets.cbGBufPBRIrradiance},
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

    if (_options.ssao) {
        _renderTargets.cbSSAO = std::make_shared<Texture>("ssao_color", getTextureProperties(TextureUsage::ColorBuffer));
        _renderTargets.cbSSAO->clear(halfSize.x, halfSize.y, PixelFormat::R8);
        _renderTargets.cbSSAO->init();

        _renderTargets.fbSSAO = std::make_shared<Framebuffer>();
        _renderTargets.fbSSAO->attachColorDepth(_renderTargets.cbSSAO, _renderTargets.dbCommonHalf);
        _renderTargets.fbSSAO->init();
    }

    // SSR framebuffer

    if (_options.ssr) {
        _renderTargets.cbSSR = std::make_unique<Texture>("ssr_color", getTextureProperties(TextureUsage::ColorBuffer));
        _renderTargets.cbSSR->clear(halfSize.x, halfSize.y, PixelFormat::RGBA8);
        _renderTargets.cbSSR->init();

        _renderTargets.fbSSR = std::make_shared<Framebuffer>();
        _renderTargets.fbSSR->attachColorDepth(_renderTargets.cbSSR, _renderTargets.dbCommonHalf);
        _renderTargets.fbSSR->init();
    }

    // Output framebuffer

    _renderTargets.cbOutput = std::make_unique<Texture>("output_color", getTextureProperties(TextureUsage::ColorBuffer));
    _renderTargets.cbOutput->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _renderTargets.cbOutput->init();

    _renderTargets.fbOutput = std::make_unique<Framebuffer>();
    _renderTargets.fbOutput->attachColorDepth(_renderTargets.cbOutput, _renderTargets.dbCommon);
    _renderTargets.fbOutput->init();
}

void RenderPipelineBase::initSSAOSamples() {
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

Texture &RenderPipelineBase::render() {
    _context.withViewport(glm::ivec4(0, 0, _targetSize), [this]() {
        auto pass = RenderPass(
            _options,
            _context,
            _shaderRegistry,
            _meshRegistry,
            _pbrTextures,
            _textureRegistry,
            _uniforms);

        // Shadows pass
        if (_passCallbacks.count(RenderPassName::DirLightShadowsPass) > 0) {
            beginDirLightShadowsPass();
            _passCallbacks.at(RenderPassName::DirLightShadowsPass)(pass);
            endDirLightShadowsPass();
        } else if (_passCallbacks.count(RenderPassName::PointLightShadows) > 0) {
            beginPointLightShadowsPass();
            _passCallbacks.at(RenderPassName::PointLightShadows)(pass);
            endPointLightShadowsPass();
        }

        // Opaque geometry pass
        beginOpaqueGeometryPass();
        if (_passCallbacks.count(RenderPassName::OpaqueGeometry) > 0) {
            _passCallbacks.at(RenderPassName::OpaqueGeometry)(pass);
        }
        endOpaqueGeometryPass();
        if (_options.ssao || _options.ssr) {
            auto halfSize = _targetSize / 2;
            if (_options.ssao) {
                renderSSAO(kSSAOSampleRadius, kSSAOBias);
                applyBoxBlur(*_renderTargets.cbSSAO, *_renderTargets.fbPingHalf, halfSize);
                _context.blitFramebuffer(
                    *_renderTargets.fbPingHalf,
                    *_renderTargets.fbSSAO,
                    glm::ivec4(0, 0, halfSize),
                    glm::ivec4(0, 0, halfSize));
            }
            if (_options.ssr) {
                renderSSR(kSSRBias, kSSRPixelStride, kSSRMaxSteps);
                applyGaussianBlur(*_renderTargets.cbSSR, *_renderTargets.fbPingHalf, halfSize, GaussianBlurParams {false, true});
                applyGaussianBlur(*_renderTargets.cbPingHalf, *_renderTargets.fbSSR, halfSize, GaussianBlurParams {true, true});
            }
        }

        // Blur highlights
        applyGaussianBlur(*_renderTargets.cbDeferredOpaque2, *_renderTargets.fbPing, _targetSize, GaussianBlurParams {false, true});
        applyGaussianBlur(*_renderTargets.cbPing, *_renderTargets.fbPong, _targetSize, GaussianBlurParams {true, true});
        _context.blitFramebuffer(
            *_renderTargets.fbPong,
            *_renderTargets.fbDeferredCombine,
            glm::ivec4(0, 0, _targetSize),
            glm::ivec4(0, 0, _targetSize),
            0, 1);

        combineOpaqueGeometry();

        // Blit opaque geometry colors into transparent geometry
        _context.blitFramebuffer(*_renderTargets.fbOpaqueGeometry,
                                 *_renderTargets.fbTransparentGeometry,
                                 glm::ivec4(0, 0, _targetSize),
                                 glm::ivec4(0, 0, _targetSize),
                                 0,
                                 0,
                                 FramebufferBlitFlags::depth);

        // Transparent geometry pass
        beginTransparentGeometryPass();
        if (_passCallbacks.count(RenderPassName::TransparentGeometry) > 0) {
            _passCallbacks.at(RenderPassName::TransparentGeometry)(pass);
        }
        endTransparentGeometryPass();
        blendTransparentGeometry();

        // Post-processing pass
        beginPostProcessingPass();
        if (_passCallbacks.count(RenderPassName::PostProcessing) > 0) {
            _passCallbacks.at(RenderPassName::PostProcessing)(pass);
        }
        endPostProcessingPass();
        if (_options.fxaa && _options.sharpen) {
            applyFXAA(*_renderTargets.cbOutput, *_renderTargets.fbPing, _targetSize);
            applySharpen(*_renderTargets.cbPing, *_renderTargets.fbOutput, _targetSize, kSharpenAmount);
        } else if (_options.fxaa || _options.sharpen) {
            if (_options.fxaa) {
                applyFXAA(*_renderTargets.cbOutput, *_renderTargets.fbPing, _targetSize);
            } else {
                applySharpen(*_renderTargets.cbOutput, *_renderTargets.fbPing, _targetSize, kSharpenAmount);
            }
            _context.blitFramebuffer(
                *_renderTargets.fbPing,
                *_renderTargets.fbOutput,
                glm::ivec4(0, 0, _targetSize),
                glm::ivec4(0, 0, _targetSize));
        }
    });

    _context.resetDrawFramebuffer();
    _context.resetReadFramebuffer();

    return *_renderTargets.cbOutput;
}

void RenderPipelineBase::beginDirLightShadowsPass() {
    _context.pushViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution));
    _context.bindDrawFramebuffer(*_renderTargets.fbDirLightShadows);
    _context.clearDepth();
}

void RenderPipelineBase::endDirLightShadowsPass() {
    _context.popViewport();
}

void RenderPipelineBase::beginPointLightShadowsPass() {
    _context.pushViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution));
    _context.bindDrawFramebuffer(*_renderTargets.fbPointLightShadows);
    _context.clearDepth();
}

void RenderPipelineBase::endPointLightShadowsPass() {
    _context.popViewport();
}

void RenderPipelineBase::beginOpaqueGeometryPass() {
    _context.bindDrawFramebuffer(*_renderTargets.fbOpaqueGeometry, {0, 1, 2, 3, 4, 5, 6, 7});
    _context.clearColorDepth();
}

void RenderPipelineBase::endOpaqueGeometryPass() {
}

void RenderPipelineBase::renderSSAO(float sampleRadius, float bias) {
    auto size = _targetSize / 2;
    _uniforms.setScreenEffect([&size, &sampleRadius, &bias](auto &se) {
        se.screenResolution = glm::vec2(size);
        se.screenResolutionRcp = 1.0f / se.screenResolution;
        se.ssaoSampleRadius = sampleRadius;
        se.ssaoBias = bias;
    });
    _context.bindDrawFramebuffer(*_renderTargets.fbSSAO, {0});
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::deferredSSAO));
    _context.bindTexture(*_renderTargets.cbGBufEyePos, TextureUnits::gBufEyePos);
    _context.bindTexture(*_renderTargets.cbGBufEyeNormal, TextureUnits::gBufEyeNormal);
    _context.bindTexture(_textureRegistry.get(TextureName::noiseRg), TextureUnits::noise);
    _context.withViewport(glm::ivec4(0, 0, size.x, size.y), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void RenderPipelineBase::renderSSR(float bias, float pixelStride, float maxSteps) {
    auto size = _targetSize / 2;
    _uniforms.setScreenEffect([&](auto &se) {
        se.screenResolution = glm::vec2(size);
        se.screenResolutionRcp = 1.0f / se.screenResolution;
        se.ssrBias = bias;
        se.ssrPixelStride = pixelStride;
        se.ssrMaxSteps = maxSteps;
    });
    _context.bindDrawFramebuffer(*_renderTargets.fbSSR, {0});
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::deferredSSR));
    _context.bindTexture(*_renderTargets.cbGBufDiffuse);
    _context.bindTexture(*_renderTargets.cbGBufLightmap, TextureUnits::lightmap);
    _context.bindTexture(*_renderTargets.cbGBufEnvMap, TextureUnits::gBufEnvMap);
    _context.bindTexture(*_renderTargets.cbGBufEyePos, TextureUnits::gBufEyePos);
    _context.bindTexture(*_renderTargets.cbGBufEyeNormal, TextureUnits::gBufEyeNormal);
    _context.withViewport(glm::ivec4(0, 0, size.x, size.y), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void RenderPipelineBase::combineOpaqueGeometry() {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::deferredCombine));
    _context.bindDrawFramebuffer(*_renderTargets.fbDeferredCombine, {0, 1});
    _context.bindTexture(*_renderTargets.cbGBufDiffuse);
    _context.bindTexture(*_renderTargets.cbGBufLightmap, TextureUnits::lightmap);
    _context.bindTexture(*_renderTargets.cbGBufEnvMap, TextureUnits::gBufEnvMap);
    _context.bindTexture(*_renderTargets.cbGBufSelfIllum, TextureUnits::gBufSelfIllum);
    _context.bindTexture(*_renderTargets.cbGBufFeatures, TextureUnits::gBufFeatures);
    _context.bindTexture(*_renderTargets.cbGBufEyePos, TextureUnits::gBufEyePos);
    _context.bindTexture(*_renderTargets.cbGBufEyeNormal, TextureUnits::gBufEyeNormal);
    _context.bindTexture(*_renderTargets.dbDirectionalLightShadows, TextureUnits::shadowMapArray);
    _context.bindTexture(*_renderTargets.dbPointLightShadows, TextureUnits::shadowMapCube);
    if (_options.ssao) {
        _context.bindTexture(*_renderTargets.cbSSAO, TextureUnits::ssao);
    }
    if (_options.ssr) {
        _context.bindTexture(*_renderTargets.cbSSR, TextureUnits::ssr);
    }
    if (_options.pbr) {
        _context.bindTexture(_pbrTextures.brdf(), TextureUnits::pbrBRDF);
        _context.bindTexture(*_renderTargets.cbGBufPBRIrradiance, TextureUnits::gBufPBRIrradiance);
    }
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipelineBase::beginTransparentGeometryPass() {
    _context.bindDrawFramebuffer(*_renderTargets.fbTransparentGeometry, {0, 1});
    _context.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
    _context.pushBlending(BlendMode::OIT_Transparent);
    glDepthMask(GL_FALSE);
}

void RenderPipelineBase::endTransparentGeometryPass() {
    glDepthMask(GL_TRUE);
    _context.popBlending();
}

void RenderPipelineBase::blendTransparentGeometry() {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::oitBlend));
    _context.bindDrawFramebuffer(*_renderTargets.fbOutput, {0});
    _context.bindTexture(*_renderTargets.cbDeferredOpaque1);
    _context.bindTexture(*_renderTargets.cbDeferredOpaque2, TextureUnits::hilights);
    _context.bindTexture(*_renderTargets.cbTransparentGeometry1, TextureUnits::oitAccum);
    _context.bindTexture(*_renderTargets.cbTransparentGeometry2, TextureUnits::oitRevealage);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void RenderPipelineBase::beginPostProcessingPass() {
    _context.bindDrawFramebuffer(*_renderTargets.fbOutput, {0});
}

void RenderPipelineBase::endPostProcessingPass() {
}

void RenderPipelineBase::applyBoxBlur(Texture &srcTexture, Framebuffer &dst, const glm::ivec2 &size) {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::postBoxBlur4));
    _context.bindDrawFramebuffer(dst, {0});
    _context.bindTexture(srcTexture);
    _context.withViewport(glm::ivec4(0, 0, size), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void RenderPipelineBase::applyGaussianBlur(Texture &tex,
                                           Framebuffer &dst,
                                           const glm::ivec2 &size,
                                           const GaussianBlurParams &params) {
    _uniforms.setScreenEffect([&size, &params](auto &se) {
        se.screenResolution = glm::vec2(size);
        se.screenResolutionRcp = 1.0f / se.screenResolution;
        se.blurDirection = params.vertical ? glm::vec2(0.0f, 1.0f) : glm::vec2(1.0f, 0.0f);
    });
    _context.useProgram(_shaderRegistry.get(params.strong
                                                ? ShaderProgramId::postGaussianBlur13
                                                : ShaderProgramId::postGaussianBlur9));
    _context.bindDrawFramebuffer(dst, {0});
    _context.bindTexture(tex);
    _context.withViewport(glm::ivec4(0, 0, size), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void RenderPipelineBase::applyMedianFilter(Texture &tex,
                                           Framebuffer &dst,
                                           const glm::ivec2 &size,
                                           bool strong) {
    _context.useProgram(_shaderRegistry.get(strong
                                                ? ShaderProgramId::postMedianFilter5
                                                : ShaderProgramId::postMedianFilter3));
    _context.bindDrawFramebuffer(dst, {0});
    _context.bindTexture(tex);
    _context.withViewport(glm::ivec4(0, 0, size), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void RenderPipelineBase::applyFXAA(Texture &tex, Framebuffer &dst, const glm::ivec2 &size) {
    _uniforms.setScreenEffect([&size](auto &se) {
        se.screenResolution = glm::vec2(size);
        se.screenResolutionRcp = 1.0f / se.screenResolution;
    });
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::postFXAA));
    _context.bindDrawFramebuffer(dst, {0});
    _context.bindTexture(tex);
    _context.withViewport(glm::ivec4(0, 0, size), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void RenderPipelineBase::applySharpen(Texture &tex,
                                      Framebuffer &dst,
                                      const glm::ivec2 &size,
                                      float amount) {
    _uniforms.setScreenEffect([&size, &amount](auto &se) {
        se.screenResolution = glm::vec2(size);
        se.screenResolutionRcp = 1.0f / se.screenResolution;
        se.sharpenAmount = amount;
    });
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::postSharpen));
    _context.bindDrawFramebuffer(dst, {0});
    _context.bindTexture(tex);
    _context.withViewport(glm::ivec4(0, 0, size), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

} // namespace scene

} // namespace reone
