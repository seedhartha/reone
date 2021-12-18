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

#include "control.h"

#include "../context.h"
#include "../mesh.h"
#include "../meshes.h"
#include "../renderbuffer.h"
#include "../scene.h"
#include "../shaders.h"
#include "../texture.h"
#include "../textures.h"
#include "../textureutil.h"
#include "../types.h"

using namespace std;

namespace reone {

namespace graphics {

void ControlPipeline::init() {
    _geometry1 = make_shared<Framebuffer>();
    _geometry1->init();

    _geometry2 = make_shared<Framebuffer>();
    _geometry2->init();
}

void ControlPipeline::prepareFor(const glm::ivec4 &extent) {
    AttachmentsId attachmentsId {extent};
    if (_attachments.count(attachmentsId) > 0) {
        return;
    }
    int w = extent[2];
    int h = extent[3];

    auto colorBuffer1 = make_unique<Texture>("color1", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    colorBuffer1->clearPixels(w, h, PixelFormat::RGBA);
    colorBuffer1->init();

    auto colorBuffer2 = make_unique<Texture>("color2", getTextureProperties(TextureUsage::ColorBuffer));
    colorBuffer2->clearPixels(w, h, PixelFormat::RGBA);
    colorBuffer2->init();

    auto depthBuffer1 = make_unique<Renderbuffer>(_options.aaSamples);
    depthBuffer1->configure(w, h, PixelFormat::Depth);
    depthBuffer1->init();

    auto depthBuffer2 = make_unique<Renderbuffer>();
    depthBuffer2->configure(w, h, PixelFormat::Depth);
    depthBuffer2->init();

    Attachments attachments;
    attachments.colorBuffer1 = move(colorBuffer1);
    attachments.colorBuffer2 = move(colorBuffer2);
    attachments.depthBuffer1 = move(depthBuffer1);
    attachments.depthBuffer2 = move(depthBuffer2);
    _attachments.insert(make_pair(attachmentsId, move(attachments)));
}

void ControlPipeline::draw(graphics::IScene &scene, const glm::ivec4 &extent, const glm::ivec2 &offset) {
    if (!scene.hasCamera()) {
        return;
    }
    AttachmentsId attachmentsId {extent};
    auto maybeAttachments = _attachments.find(attachmentsId);
    if (maybeAttachments == _attachments.end()) {
        return;
    }
    auto &attachments = maybeAttachments->second;

    // Set uniforms prototype

    auto &uniformsPrototype = scene.uniformsPrototype();
    uniformsPrototype.general = GeneralUniforms();
    uniformsPrototype.general.projection = scene.cameraProjection();
    uniformsPrototype.general.view = scene.cameraView();
    uniformsPrototype.general.cameraPosition = glm::vec4(scene.cameraPosition(), 1.0f);

    // Draw to multi-sampled framebuffer

    glm::ivec4 oldViewport(_graphicsContext.viewport());
    _graphicsContext.setViewport(glm::ivec4(0, 0, extent[2], extent[3]));

    bool oldDepthTest = _graphicsContext.isDepthTestEnabled();
    _graphicsContext.setDepthTestEnabled(true);

    _graphicsContext.bindDrawFramebuffer(_geometry1);
    _geometry1->attachColor(*attachments.colorBuffer1);
    _geometry1->attachDepth(*attachments.depthBuffer1);

    _graphicsContext.clear(ClearBuffers::colorDepth);
    scene.draw();

    // Blit multi-sampled framebuffer to normal

    _graphicsContext.bindReadFramebuffer(_geometry1);
    _graphicsContext.bindDrawFramebuffer(_geometry2);
    _geometry2->attachColor(*attachments.colorBuffer2);
    _geometry2->attachDepth(*attachments.depthBuffer2);
    for (int i = 0; i < 2; ++i) {
        _geometry1->setReadBuffer(i);
        _geometry2->setDrawBuffer(i);
        _geometry1->blit(extent[2], extent[3]);
    }

    // Draw control

    _graphicsContext.setDepthTestEnabled(oldDepthTest);
    _graphicsContext.setViewport(oldViewport);
    _graphicsContext.unbindDrawFramebuffer();
    _graphicsContext.bindTexture(0, attachments.colorBuffer2);

    glm::mat4 projection(glm::ortho(
        0.0f,
        static_cast<float>(oldViewport[2]),
        static_cast<float>(oldViewport[3]),
        0.0f));

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(extent[0] + offset.x, extent[1] + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(extent[2], extent[3], 1.0f));

    auto &uniforms = _shaders.uniforms();
    uniforms.general = GeneralUniforms();
    uniforms.general.projection = move(projection);
    uniforms.general.model = move(transform);

    _graphicsContext.useShaderProgram(_shaders.gui());
    _shaders.refreshUniforms();
    _meshes.quad().draw();
}

} // namespace graphics

} // namespace reone
