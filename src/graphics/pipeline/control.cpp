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

#include "../camera.h"
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

    // Multi-sample geometry framebuffer

    auto cbGeometryMS = make_shared<Texture>("geometry_color_ms", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    cbGeometryMS->clear(w, h, PixelFormat::RGBA);
    cbGeometryMS->init();

    auto dbGeometryMS = make_shared<Renderbuffer>(_options.aaSamples);
    dbGeometryMS->configure(w, h, PixelFormat::Depth);
    dbGeometryMS->init();

    auto fbGeometryMS = make_unique<Framebuffer>();
    fbGeometryMS->attachColorDepth(cbGeometryMS, dbGeometryMS);
    fbGeometryMS->init();

    // Geometry framebuffer

    auto cbGeometry = make_shared<Texture>("geometry_color", getTextureProperties(TextureUsage::ColorBuffer));
    cbGeometry->clear(w, h, PixelFormat::RGBA);
    cbGeometry->init();

    auto dbGeometry = make_shared<Renderbuffer>();
    dbGeometry->configure(w, h, PixelFormat::Depth);
    dbGeometry->init();

    auto fbGeometry = make_unique<Framebuffer>();
    fbGeometry->attachColorDepth(cbGeometry, dbGeometry);
    fbGeometry->init();

    Attachments attachments;
    attachments.cbGeometryMS = move(cbGeometryMS);
    attachments.dbGeometryMS = move(dbGeometryMS);
    attachments.fbGeometryMS = move(fbGeometryMS);
    attachments.cbGeometry = move(cbGeometry);
    attachments.dbGeometry = move(dbGeometry);
    attachments.fbGeometry = move(fbGeometry);
    _attachments.insert(make_pair(attachmentsId, move(attachments)));
}

void ControlPipeline::draw(graphics::IScene &scene, const glm::ivec4 &extent, const glm::ivec2 &offset) {
    auto camera = scene.camera();
    if (!camera) {
        return;
    }
    AttachmentsId attachmentsId {extent};
    auto maybeAttachments = _attachments.find(attachmentsId);
    if (maybeAttachments == _attachments.end()) {
        return;
    }
    auto &attachments = maybeAttachments->second;
    auto &fbGeometryMS = *attachments.fbGeometryMS;
    auto &fbGeometry = *attachments.fbGeometry;
    int w = extent[2];
    int h = extent[3];

    // Set global uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();
    uniforms.general.cameraPosition = glm::vec4(camera->position(), 1.0f);
    uniforms.general.worldAmbientColor = glm::vec4(scene.ambientLightColor(), 1.0f);

    // Draw scene to multi-sample framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbGeometryMS.nameGL());
    _graphicsContext.withViewport(glm::ivec4(0, 0, w, h), [this, &w, &h, &scene]() {
        _graphicsContext.clearColorDepth();
        scene.draw();
    });

    // Blit multi-sample geometry framebuffer to geometry framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbGeometryMS.nameGL());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbGeometry.nameGL());
    for (int i = 0; i < 2; ++i) {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // Reset framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Present geometry framebuffer
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
    _shaders.use(_shaders.gui(), true);
    _textures.bind(*attachments.cbGeometry);
    _graphicsContext.withoutDepthTest([this]() {
        _meshes.quad().draw();
    });
}

} // namespace graphics

} // namespace reone
