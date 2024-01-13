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

void RetroRenderPipeline::init() {
    checkThat(!_inited, "Pipeline already initialized");
    checkMainThread();

    initRenderTargets();

    _inited = true;
}

void RetroRenderPipeline::initRenderTargets() {
    _targets.outputColor = std::make_unique<Texture>("retro_output_color", getTextureProperties(TextureUsage::ColorBuffer));
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
    _context.withViewport(glm::ivec4 {0, 0, _targetSize}, [this]() {
        _context.bindDrawFramebuffer(*_targets.output, {0});
        auto pass = RetroRenderPass {_options,
                                     _context,
                                     _shaderRegistry,
                                     _meshRegistry,
                                     _textureRegistry,
                                     _uniforms};
        _context.clearColorDepth();
        // if (_passCallbacks.count(RenderPassName::OpaqueGeometry) > 0) {
        //     _passCallbacks.at(RenderPassName::OpaqueGeometry)(pass);
        // }
        _context.resetDrawFramebuffer();
    });
    return *_targets.outputColor;
}

} // namespace scene

} // namespace reone
