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

void ControlPipeline::prepareFor(const glm::ivec4 &extent) {
    AttachmentsId attachmentsId {extent};
    if (_attachments.count(attachmentsId) > 0) {
        return;
    }
    int w = extent[2];
    int h = extent[3];

    auto colorBuffer1 = make_shared<Texture>("color1", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    colorBuffer1->clear(w, h, PixelFormat::RGBA);
    colorBuffer1->init();

    auto depthBuffer1 = make_shared<Renderbuffer>(_options.aaSamples);
    depthBuffer1->configure(w, h, PixelFormat::Depth);
    depthBuffer1->init();

    auto framebuffer1 = make_unique<Framebuffer>();
    framebuffer1->attachColorDepth(colorBuffer1, depthBuffer1);
    framebuffer1->init();

    auto colorBuffer2 = make_shared<Texture>("color2", getTextureProperties(TextureUsage::ColorBuffer));
    colorBuffer2->clear(w, h, PixelFormat::RGBA);
    colorBuffer2->init();

    auto depthBuffer2 = make_shared<Renderbuffer>();
    depthBuffer2->configure(w, h, PixelFormat::Depth);
    depthBuffer2->init();

    auto framebuffer2 = make_unique<Framebuffer>();
    framebuffer2->attachColorDepth(colorBuffer2, depthBuffer2);
    framebuffer2->init();

    Attachments attachments;
    attachments.colorBuffer1 = move(colorBuffer1);
    attachments.colorBuffer2 = move(colorBuffer2);
    attachments.depthBuffer1 = move(depthBuffer1);
    attachments.depthBuffer2 = move(depthBuffer2);
    attachments.framebuffer1 = move(framebuffer1);
    attachments.framebuffer2 = move(framebuffer2);
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
    auto &framebuffer1 = *attachments.framebuffer1;
    auto &framebuffer2 = *attachments.framebuffer2;

    // Set global uniforms

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = scene.cameraProjection();
    uniforms.general.view = scene.cameraView();
    uniforms.general.cameraPosition = glm::vec4(scene.cameraPosition(), 1.0f);
    uniforms.general.worldAmbientColor = glm::vec4(scene.ambientLightColor(), 1.0f);

    // Draw to multi-sampled framebuffer

    glViewport(0, 0, extent[2], extent[3]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer1.nameGL());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.draw();

    // Blit multi-sampled framebuffer to standard framebuffer

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer1.nameGL());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer2.nameGL());
    for (int i = 0; i < 2; ++i) {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, extent[2], extent[3], 0, 0, extent[2], extent[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // Draw control

    glViewport(0, 0, _options.width, _options.height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _graphicsContext.bindTexture(0, attachments.colorBuffer2);

    glm::mat4 projection(glm::ortho(
        0.0f,
        static_cast<float>(_options.width),
        static_cast<float>(_options.height),
        0.0f));

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(extent[0] + offset.x, extent[1] + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(extent[2], extent[3], 1.0f));

    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.projection = move(projection);
    uniforms.general.model = move(transform);

    glDisable(GL_DEPTH_TEST);
    _shaders.use(_shaders.gui(), true);
    _meshes.quad().draw();
    glEnable(GL_DEPTH_TEST);
}

} // namespace graphics

} // namespace reone
