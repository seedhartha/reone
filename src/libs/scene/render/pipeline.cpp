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

#include "reone/scene/render/pipeline.h"

#include "reone/graphics/context.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/scene/render/pipeline/pbr.h"
#include "reone/scene/render/pipeline/retro.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

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

std::unique_ptr<IRenderPipeline> RenderPipelineFactory::create(RendererType type, glm::ivec2 targetSize) {
    switch (type) {
    case RendererType::Retro:
        return std::make_unique<RetroRenderPipeline>(
            std::move(targetSize),
            _options,
            _context,
            _meshRegistry,
            _pbrTextures,
            _shaderRegistry,
            _textureRegistry,
            _uniforms);
    case RendererType::PBR:
        return std::make_unique<PBRRenderPipeline>(
            std::move(targetSize),
            _options,
            _context,
            _meshRegistry,
            _pbrTextures,
            _shaderRegistry,
            _textureRegistry,
            _uniforms);
    default:
        throw std::invalid_argument("Unsupported renderer type: " + std::to_string(static_cast<int>(type)));
    }
}

} // namespace scene

} // namespace reone
