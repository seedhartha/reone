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

#include "reone/scene/render/pipeline/pbr.h"

#include "reone/graphics/context.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/pbrtextures.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textureregistry.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/scene/render/pass/pbr.h"
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

void PBRRenderPipeline::init() {
    checkThat(!_inited, "Pipeline already initialized");
    checkMainThread();

    initRenderTargets();
    if (_options.ssao) {
        initSSAOSamples();
    }

    _inited = true;
}

void PBRRenderPipeline::initRenderTargets() {
    auto halfSize = _targetSize / 2;

    // Reusable targets

    _targets.dbCommon = std::make_unique<Renderbuffer>();
    _targets.dbCommon->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth32F);
    _targets.dbCommon->init();

    _targets.dbCommonHalf = std::make_unique<Renderbuffer>();
    _targets.dbCommonHalf->configure(halfSize.x, halfSize.y, PixelFormat::Depth32F);
    _targets.dbCommonHalf->init();

    _targets.cbPing = std::make_unique<Texture>(
        "ping_color",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbPing->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbPing->init();

    _targets.fbPing = std::make_unique<Framebuffer>();
    _targets.fbPing->attachColorDepth(_targets.cbPing, _targets.dbCommon);
    _targets.fbPing->init();

    _targets.cbPingHalf = std::make_unique<Texture>(
        "ping_color_half",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbPingHalf->clear(halfSize.x, halfSize.y, PixelFormat::RGBA8);
    _targets.cbPingHalf->init();

    _targets.fbPingHalf = std::make_unique<Framebuffer>();
    _targets.fbPingHalf->attachColorDepth(_targets.cbPingHalf, _targets.dbCommonHalf);
    _targets.fbPingHalf->init();

    _targets.cbPong = std::make_unique<Texture>(
        "pong_color",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbPong->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbPong->init();

    _targets.fbPong = std::make_unique<Framebuffer>();
    _targets.fbPong->attachColorDepth(_targets.cbPong, _targets.dbCommon);
    _targets.fbPong->init();

    _targets.cbPongHalf = std::make_unique<Texture>(
        "pong_color_half",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbPongHalf->clear(halfSize.x, halfSize.y, PixelFormat::RGBA8);
    _targets.cbPongHalf->init();

    _targets.fbPongHalf = std::make_unique<Framebuffer>();
    _targets.fbPongHalf->attachColorDepth(_targets.cbPongHalf, _targets.dbCommonHalf);
    _targets.fbPongHalf->init();

    // Directional light shadows framebuffer

    _targets.dbDirectionalLightShadows = std::make_unique<Texture>(
        "point_light_shadows_color",
        TextureType::TwoDimArray,
        getTextureProperties(TextureUsage::DepthBuffer));
    _targets.dbDirectionalLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F, kNumShadowCascades);
    _targets.dbDirectionalLightShadows->init();

    _targets.fbDirLightShadows = std::make_shared<Framebuffer>();
    _targets.fbDirLightShadows->attachDepth(_targets.dbDirectionalLightShadows);
    _targets.fbDirLightShadows->init();

    // Point light shadows framebuffer

    _targets.dbPointLightShadows = std::make_unique<Texture>(
        "directional_light_shadows_color",
        TextureType::CubeMap,
        getTextureProperties(TextureUsage::DepthBuffer));
    _targets.dbPointLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F);
    _targets.dbPointLightShadows->init();

    _targets.fbPointLightShadows = std::make_shared<Framebuffer>();
    _targets.fbPointLightShadows->attachDepth(_targets.dbPointLightShadows);
    _targets.fbPointLightShadows->init();

    // G-Buffer framebuffer

    _targets.cbGBufDiffuse = std::make_unique<Texture>(
        "gbuffer_diffuse",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbGBufDiffuse->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbGBufDiffuse->init();

    _targets.cbGBufLightmap = std::make_unique<Texture>(
        "gbuffer_lightmap",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbGBufLightmap->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbGBufLightmap->init();

    _targets.cbGBufSelfIllum = std::make_unique<Texture>(
        "gbuffer_selfillum",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbGBufSelfIllum->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbGBufSelfIllum->init();

    _targets.cbGBufEyeNormal = std::make_unique<Texture>(
        "gbuffer_eyenormal",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbGBufEyeNormal->clear(_targetSize.x, _targetSize.y, PixelFormat::RGB8);
    _targets.cbGBufEyeNormal->init();

    _targets.dbGBuffer = std::make_shared<Texture>(
        "gbuffer_depth",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::DepthBuffer));
    _targets.dbGBuffer->clear(_targetSize.x, _targetSize.y, PixelFormat::Depth32F);
    _targets.dbGBuffer->init();

    _targets.fbOpaqueGeometry = std::make_shared<Framebuffer>();
    _targets.fbOpaqueGeometry->attachColorsDepth(
        {_targets.cbGBufDiffuse,
         _targets.cbGBufEyeNormal,
         _targets.cbGBufLightmap,
         _targets.cbGBufSelfIllum},
        _targets.dbGBuffer);
    _targets.fbOpaqueGeometry->init();

    // Opaque geometry framebuffer

    _targets.cbDeferredOpaque1 = std::make_unique<Texture>(
        "deferred_opaque_color1",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbDeferredOpaque1->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbDeferredOpaque1->init();

    _targets.cbDeferredOpaque2 = std::make_unique<Texture>(
        "deferred_opaque_color2",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbDeferredOpaque2->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbDeferredOpaque2->init();

    _targets.fbDeferredCombine = std::make_shared<Framebuffer>();
    _targets.fbDeferredCombine->attachColorsDepth({_targets.cbDeferredOpaque1,
                                                   _targets.cbDeferredOpaque2},
                                                  _targets.dbCommon);
    _targets.fbDeferredCombine->init();

    // Transparent geometry framebuffer

    _targets.cbTransparentGeometry1 = std::make_unique<Texture>(
        "transparent_geometry_color1",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbTransparentGeometry1->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA16F);
    _targets.cbTransparentGeometry1->init();

    _targets.cbTransparentGeometry2 = std::make_unique<Texture>(
        "transparent_geometry_color2",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbTransparentGeometry2->clear(_targetSize.x, _targetSize.y, PixelFormat::R16F);
    _targets.cbTransparentGeometry2->init();

    _targets.fbTransparentGeometry = std::make_shared<Framebuffer>();
    _targets.fbTransparentGeometry->attachColorsDepth(
        {_targets.cbTransparentGeometry1, _targets.cbTransparentGeometry2},
        _targets.dbCommon);
    _targets.fbTransparentGeometry->init();

    // SSAO framebuffer

    if (_options.ssao) {
        _targets.cbSSAO = std::make_shared<Texture>(
            "ssao_color",
            TextureType::TwoDim,
            getTextureProperties(TextureUsage::ColorBuffer));
        _targets.cbSSAO->clear(halfSize.x, halfSize.y, PixelFormat::R8);
        _targets.cbSSAO->init();

        _targets.fbSSAO = std::make_shared<Framebuffer>();
        _targets.fbSSAO->attachColorDepth(_targets.cbSSAO, _targets.dbCommonHalf);
        _targets.fbSSAO->init();
    }

    // SSR framebuffer

    if (_options.ssr) {
        _targets.cbSSR = std::make_unique<Texture>(
            "ssr_color",
            TextureType::TwoDim,
            getTextureProperties(TextureUsage::ColorBuffer));
        _targets.cbSSR->clear(halfSize.x, halfSize.y, PixelFormat::RGBA8);
        _targets.cbSSR->init();

        _targets.fbSSR = std::make_shared<Framebuffer>();
        _targets.fbSSR->attachColorDepth(_targets.cbSSR, _targets.dbCommonHalf);
        _targets.fbSSR->init();
    }

    // Output framebuffer

    _targets.cbOutput = std::make_unique<Texture>(
        "output_color",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.cbOutput->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.cbOutput->init();

    _targets.fbOutput = std::make_unique<Framebuffer>();
    _targets.fbOutput->attachColorDepth(_targets.cbOutput, _targets.dbCommon);
    _targets.fbOutput->init();
}

void PBRRenderPipeline::initSSAOSamples() {
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

Texture &PBRRenderPipeline::render() {
    auto pass = PBRRenderPass {_options,
                               _context,
                               _shaderRegistry,
                               _meshRegistry,
                               _pbrTextures,
                               _textureRegistry,
                               _uniforms};

    glm::ivec4 screenRect {0, 0, _targetSize.x, _targetSize.y};
    _context.withViewport(screenRect, [this, &pass, &screenRect]() {
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
                applyBoxBlur(*_targets.cbSSAO, *_targets.fbPingHalf, halfSize);
                _context.blitFramebuffer(
                    *_targets.fbPingHalf,
                    *_targets.fbSSAO,
                    glm::ivec4(0, 0, halfSize),
                    glm::ivec4(0, 0, halfSize));
            }
            if (_options.ssr) {
                renderSSR(kSSRBias, kSSRPixelStride, kSSRMaxSteps);
                applyGaussianBlur(
                    *_targets.cbSSR,
                    *_targets.fbPingHalf,
                    halfSize,
                    GaussianBlurParams {false, true});
                applyGaussianBlur(
                    *_targets.cbPingHalf,
                    *_targets.fbSSR,
                    halfSize,
                    GaussianBlurParams {true, true});
            }
        }

        // Blur highlights
        combineOpaqueGeometry();
        applyGaussianBlur(
            *_targets.cbDeferredOpaque2,
            *_targets.fbPing,
            _targetSize,
            GaussianBlurParams {false, true});
        applyGaussianBlur(
            *_targets.cbPing,
            *_targets.fbPong,
            _targetSize,
            GaussianBlurParams {true, true});
        _context.blitFramebuffer(
            *_targets.fbPong,
            *_targets.fbDeferredCombine,
            screenRect, screenRect,
            0, 1);

        // Blit opaque geometry depth into transparent geometry
        _context.blitFramebuffer(*_targets.fbOpaqueGeometry,
                                 *_targets.fbTransparentGeometry,
                                 screenRect, screenRect,
                                 0, 0,
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
            applyFXAA(*_targets.cbOutput, *_targets.fbPing, _targetSize);
            applySharpen(*_targets.cbPing, *_targets.fbOutput, _targetSize, kSharpenAmount);
        } else if (_options.fxaa || _options.sharpen) {
            if (_options.fxaa) {
                applyFXAA(*_targets.cbOutput, *_targets.fbPing, _targetSize);
            } else {
                applySharpen(*_targets.cbOutput, *_targets.fbPing, _targetSize, kSharpenAmount);
            }
            _context.blitFramebuffer(
                *_targets.fbPing,
                *_targets.fbOutput,
                screenRect, screenRect);
        }

        _context.resetDrawFramebuffer();
    });

    return *_targets.cbOutput;
}

void PBRRenderPipeline::beginDirLightShadowsPass() {
    _context.pushViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution));
    _context.bindDrawFramebuffer(*_targets.fbDirLightShadows);
    _context.clearDepth();
}

void PBRRenderPipeline::endDirLightShadowsPass() {
    _context.popViewport();
}

void PBRRenderPipeline::beginPointLightShadowsPass() {
    _context.pushViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution));
    _context.bindDrawFramebuffer(*_targets.fbPointLightShadows);
    _context.clearDepth();
}

void PBRRenderPipeline::endPointLightShadowsPass() {
    _context.popViewport();
}

void PBRRenderPipeline::beginOpaqueGeometryPass() {
    _context.bindDrawFramebuffer(*_targets.fbOpaqueGeometry, {0, 1, 2, 3});
    _context.clearColorDepth();
}

void PBRRenderPipeline::endOpaqueGeometryPass() {
}

void PBRRenderPipeline::renderSSAO(float sampleRadius, float bias) {
    auto size = _targetSize / 2;
    _uniforms.setScreenEffect([&size, &sampleRadius, &bias](auto &se) {
        se.screenResolution = glm::vec2(size);
        se.screenResolutionRcp = 1.0f / se.screenResolution;
        se.ssaoSampleRadius = sampleRadius;
        se.ssaoBias = bias;
    });
    _context.bindDrawFramebuffer(*_targets.fbSSAO, {0});
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::pbrSSAO));
    _context.bindTexture(*_targets.dbGBuffer, TextureUnits::gBufDepth);
    _context.bindTexture(*_targets.cbGBufEyeNormal, TextureUnits::gBufEyeNormal);
    _context.bindTexture(_textureRegistry.get(TextureName::noiseRg), TextureUnits::noise);
    _context.withViewport(glm::ivec4(0, 0, size.x, size.y), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void PBRRenderPipeline::renderSSR(float bias, float pixelStride, float maxSteps) {
    auto size = _targetSize / 2;
    _uniforms.setScreenEffect([&](auto &se) {
        se.screenResolution = glm::vec2(size);
        se.screenResolutionRcp = 1.0f / se.screenResolution;
        se.ssrBias = bias;
        se.ssrPixelStride = pixelStride;
        se.ssrMaxSteps = maxSteps;
    });
    _context.bindDrawFramebuffer(*_targets.fbSSR, {0});
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::pbrSSR));
    _context.bindTexture(*_targets.cbGBufDiffuse);
    _context.bindTexture(*_targets.cbGBufLightmap, TextureUnits::lightmap);
    _context.bindTexture(*_targets.dbGBuffer, TextureUnits::gBufDepth);
    _context.bindTexture(*_targets.cbGBufEyeNormal, TextureUnits::gBufEyeNormal);
    _context.withViewport(glm::ivec4(0, 0, size.x, size.y), [this]() {
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
    });
}

void PBRRenderPipeline::combineOpaqueGeometry() {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::pbrCombine));
    _context.bindDrawFramebuffer(*_targets.fbDeferredCombine, {0, 1});
    _context.bindTexture(*_targets.cbGBufDiffuse);
    _context.bindTexture(*_targets.cbGBufLightmap, TextureUnits::lightmap);
    _context.bindTexture(*_targets.cbGBufSelfIllum, TextureUnits::gBufSelfIllum);
    _context.bindTexture(*_targets.dbGBuffer, TextureUnits::gBufDepth);
    _context.bindTexture(*_targets.cbGBufEyeNormal, TextureUnits::gBufEyeNormal);
    _context.bindTexture(*_targets.dbDirectionalLightShadows, TextureUnits::shadowMapArray);
    _context.bindTexture(*_targets.dbPointLightShadows, TextureUnits::shadowMapCube);
    _context.bindTexture(_pbrTextures.brdf(), TextureUnits::brdfLUT);
    _context.bindTexture(_pbrTextures.irradianceMapArray(), TextureUnits::irradianceMapArray);
    _context.bindTexture(_pbrTextures.prefilteredEnvMapArray(), TextureUnits::prefilteredEnvMapArray);
    if (_options.ssao) {
        _context.bindTexture(*_targets.cbSSAO, TextureUnits::ssao);
    }
    if (_options.ssr) {
        _context.bindTexture(*_targets.cbSSR, TextureUnits::ssr);
    }
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void PBRRenderPipeline::beginTransparentGeometryPass() {
    _context.bindDrawFramebuffer(*_targets.fbTransparentGeometry, {0, 1});
    _context.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
    _context.pushBlendMode(BlendMode::OIT_Transparent);
    _context.pushDepthMask(false);
}

void PBRRenderPipeline::endTransparentGeometryPass() {
    _context.popDepthMask();
    _context.popBlendMode();
}

void PBRRenderPipeline::blendTransparentGeometry() {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::oitBlend));
    _context.bindDrawFramebuffer(*_targets.fbOutput, {0});
    _context.bindTexture(*_targets.cbDeferredOpaque1);
    _context.bindTexture(*_targets.cbDeferredOpaque2, TextureUnits::hilights);
    _context.bindTexture(*_targets.cbTransparentGeometry1, TextureUnits::oitAccum);
    _context.bindTexture(*_targets.cbTransparentGeometry2, TextureUnits::oitRevealage);
    _context.clearColorDepth();
    _meshRegistry.get(MeshName::quadNDC).draw();
}

void PBRRenderPipeline::beginPostProcessingPass() {
    _context.bindDrawFramebuffer(*_targets.fbOutput, {0});
}

void PBRRenderPipeline::endPostProcessingPass() {
}

} // namespace scene

} // namespace reone
