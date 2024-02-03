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

#include "reone/scene/render/pipeline/retro.h"

#include "reone/graphics/context.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/pbrtextures.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textureregistry.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/scene/render/pass/retro.h"
#include "reone/system/checkutil.h"
#include "reone/system/threadutil.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kSharpenAmount = 0.25f;

void RetroRenderPipeline::init() {
    checkThat(!_inited, "Pipeline already initialized");
    checkMainThread();

    initRenderTargets();

    _inited = true;
}

void RetroRenderPipeline::initRenderTargets() {
    // Shadows framebuffer

    _targets.dirLightShadowsDepth = std::make_shared<Texture>(
        "retro_dir_light_shadows_depth",
        TextureType::TwoDimArray,
        getTextureProperties(TextureUsage::DepthBuffer));
    _targets.dirLightShadowsDepth->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F, kNumShadowCascades);
    _targets.dirLightShadowsDepth->init();

    _targets.pointLightShadowsDepth = std::make_shared<Texture>(
        "retro_point_light_shadows_depth",
        TextureType::CubeMap,
        getTextureProperties(TextureUsage::DepthBuffer));
    _targets.pointLightShadowsDepth->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth32F, kNumCubeFaces);
    _targets.pointLightShadowsDepth->init();

    _targets.shadows = std::make_shared<Framebuffer>();
    _targets.shadows->init();

    // Opaque framebuffer

    _targets.opaqueColor = std::make_unique<Texture>(
        "retro_opaque_color",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.opaqueColor->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.opaqueColor->init();

    _targets.opaqueHilights = std::make_unique<Texture>(
        "retro_opaque_hilights",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.opaqueHilights->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.opaqueHilights->init();

    _targets.opaqueDepth = std::make_unique<Renderbuffer>();
    _targets.opaqueDepth->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth24);
    _targets.opaqueDepth->init();

    _targets.opaque = std::make_unique<Framebuffer>();
    _targets.opaque->attachColorsDepth({_targets.opaqueColor, _targets.opaqueHilights}, _targets.opaqueDepth);
    _targets.opaque->init();

    // Transparent framebuffer

    _targets.transparentAccum = std::make_unique<Texture>(
        "retro_transparent_accum",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.transparentAccum->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA16F);
    _targets.transparentAccum->init();

    _targets.transparentRevealage = std::make_unique<Texture>(
        "retro_transparent_revealage",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.transparentRevealage->clear(_targetSize.x, _targetSize.y, PixelFormat::R16F);
    _targets.transparentRevealage->init();

    _targets.transparentDepth = std::make_unique<Renderbuffer>();
    _targets.transparentDepth->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth24);
    _targets.transparentDepth->init();

    _targets.transparent = std::make_unique<Framebuffer>();
    _targets.transparent->attachColorsDepth({_targets.transparentAccum,
                                             _targets.transparentRevealage},
                                            _targets.transparentDepth);
    _targets.transparent->init();

    // Ping-pong framebuffers

    _targets.pingColor = std::make_unique<Texture>(
        "retro_ping_color",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.pingColor->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.pingColor->init();

    _targets.pongColor = std::make_unique<Texture>(
        "retro_pong_color",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.pongColor->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.pongColor->init();

    _targets.pingPongDepth = std::make_unique<Renderbuffer>();
    _targets.pingPongDepth->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth24);
    _targets.pingPongDepth->init();

    _targets.ping = std::make_unique<Framebuffer>();
    _targets.ping->attachColorDepth(_targets.pingColor, _targets.pingPongDepth);
    _targets.ping->init();

    _targets.pong = std::make_unique<Framebuffer>();
    _targets.pong->attachColorDepth(_targets.pongColor, _targets.pingPongDepth);
    _targets.pong->init();

    // Output framebuffer

    _targets.outputColor = std::make_unique<Texture>(
        "retro_output_color",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _targets.outputColor->clear(_targetSize.x, _targetSize.y, PixelFormat::RGBA8);
    _targets.outputColor->init();

    _targets.outputDepth = std::make_unique<Renderbuffer>();
    _targets.outputDepth->configure(_targetSize.x, _targetSize.y, PixelFormat::Depth24);
    _targets.outputDepth->init();

    _targets.output = std::make_unique<Framebuffer>();
    _targets.output->attachColorDepth(_targets.outputColor, _targets.outputDepth);
    _targets.output->init();
}

Texture &RetroRenderPipeline::render() {
    auto pass = RetroRenderPass {_options,
                                 _context,
                                 _shaderRegistry,
                                 _statistic,
                                 _meshRegistry,
                                 _textureRegistry,
                                 _uniforms};

    bool dirLightShadows = _passCallbacks.count(RenderPassName::DirLightShadowsPass) > 0;
    bool pointLightShadows = _passCallbacks.count(RenderPassName::PointLightShadows) > 0;
    if (dirLightShadows || pointLightShadows) {
        _context.bindDrawFramebuffer(*_targets.shadows, {});
        _context.withViewport(glm::ivec4 {0, 0, _options.shadowResolution, _options.shadowResolution}, [this, &pass, &dirLightShadows]() {
            _context.clearDepth();
            if (dirLightShadows) {
                _targets.shadows->attachTexture(*_targets.dirLightShadowsDepth, Framebuffer::Attachment::Depth);
                _passCallbacks.at(RenderPassName::DirLightShadowsPass)(pass);
            } else {
                _targets.shadows->attachTexture(*_targets.pointLightShadowsDepth, Framebuffer::Attachment::Depth);
                _passCallbacks.at(RenderPassName::PointLightShadows)(pass);
            }
        });
    }

    glm::ivec4 screenRect {0, 0, _targetSize};
    _context.withViewport(screenRect, [this, &pass, &dirLightShadows, &pointLightShadows, &screenRect]() {
        // Render opaque geometry
        _context.bindDrawFramebuffer(*_targets.opaque, {0, 1});
        _context.clearColorDepth();
        if (_passCallbacks.count(RenderPassName::OpaqueGeometry) > 0) {
            if (dirLightShadows) {
                _context.bindTexture(*_targets.dirLightShadowsDepth, TextureUnits::shadowMapArray);
            } else if (pointLightShadows) {
                _context.bindTexture(*_targets.pointLightShadowsDepth, TextureUnits::shadowMapCube);
            }
            _passCallbacks.at(RenderPassName::OpaqueGeometry)(pass);
        }

        // Blur hilights
        GaussianBlurParams blurParams;
        blurParams.vertical = false;
        blurParams.strong = true;
        applyGaussianBlur(
            *_targets.opaqueHilights,
            *_targets.ping,
            _targetSize,
            blurParams);
        blurParams.vertical = true;
        applyGaussianBlur(
            *_targets.pingColor,
            *_targets.pong,
            _targetSize,
            blurParams);
        _context.blitFramebuffer(
            *_targets.pong,
            *_targets.opaque,
            screenRect, screenRect,
            0, 1);

        // Render transparent geometry
        if (_passCallbacks.count(RenderPassName::TransparentGeometry) > 0) {
            _context.blitFramebuffer(
                *_targets.opaque,
                *_targets.transparent,
                screenRect, screenRect,
                0, 0,
                FramebufferBlitFlags::depth);
            _context.bindDrawFramebuffer(*_targets.transparent, {0, 1});
            _context.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
            _context.withBlendMode(BlendMode::OIT_Transparent, [this, &pass]() {
                _context.withDepthMask(false, [this, &pass]() {
                    _passCallbacks.at(RenderPassName::TransparentGeometry)(pass);
                });
            });
        }

        // Blend opaque and transparent geometry
        _context.bindDrawFramebuffer(*_targets.output, {0});
        _context.useProgram(_shaderRegistry.get(ShaderProgramId::oitBlend));
        _context.bindTexture(*_targets.opaqueColor, TextureUnits::mainTex);
        _context.bindTexture(*_targets.opaqueHilights, TextureUnits::hilights);
        _context.bindTexture(*_targets.transparentAccum, TextureUnits::oitAccum);
        _context.bindTexture(*_targets.transparentRevealage, TextureUnits::oitRevealage);
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw(_statistic);

        if (_options.fxaa && _options.sharpen) {
            applyFXAA(*_targets.outputColor, *_targets.ping, _targetSize);
            applySharpen(*_targets.pingColor, *_targets.pong, _targetSize, kSharpenAmount);
            _context.blitFramebuffer(
                *_targets.pong,
                *_targets.output,
                screenRect, screenRect,
                0, 0);
        } else if (_options.fxaa || _options.sharpen) {
            if (_options.fxaa) {
                applyFXAA(*_targets.outputColor, *_targets.ping, _targetSize);
            } else if (_options.sharpen) {
                applySharpen(*_targets.outputColor, *_targets.ping, _targetSize, kSharpenAmount);
            }
            _context.blitFramebuffer(
                *_targets.ping,
                *_targets.output,
                screenRect, screenRect,
                0, 0);
        }

        // Post-processing effects
        if (_passCallbacks.count(RenderPassName::PostProcessing) > 0) {
            _passCallbacks.at(RenderPassName::PostProcessing)(pass);
        }

        _context.resetDrawFramebuffer();
    });

    return *_targets.outputColor;
}

} // namespace scene

} // namespace reone
