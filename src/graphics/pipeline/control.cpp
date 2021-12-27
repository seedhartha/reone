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

    shared_ptr<Texture> cbGeometry1MS;
    shared_ptr<Texture> cbGeometry2MS;
    shared_ptr<Renderbuffer> dbGeometryMS;
    shared_ptr<Framebuffer> fbGeometryMS;

    // Multi-sample geometry framebuffer

    if (_options.aaMethod >= AntiAliasingMethods::msaa2) {
        int samples = 2 << (_options.aaMethod - AntiAliasingMethods::msaa2);

        cbGeometry1MS = make_shared<Texture>("geometry_color1_ms", getTextureProperties(TextureUsage::ColorBuffer, samples));
        cbGeometry1MS->clear(w, h, PixelFormat::RGBA);
        cbGeometry1MS->init();

        cbGeometry2MS = make_shared<Texture>("geometry_color2_ms", getTextureProperties(TextureUsage::ColorBuffer, samples));
        cbGeometry2MS->clear(w, h, PixelFormat::RGBA);
        cbGeometry2MS->init();

        dbGeometryMS = make_shared<Renderbuffer>(samples);
        dbGeometryMS->configure(w, h, PixelFormat::Depth);
        dbGeometryMS->init();

        fbGeometryMS = make_unique<Framebuffer>();
        fbGeometryMS->attachColorsDepth(cbGeometry1MS, cbGeometry2MS, dbGeometryMS);
        fbGeometryMS->init();
    }

    // Geometry framebuffer

    auto cbGeometry1 = make_shared<Texture>("geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    cbGeometry1->clear(w, h, PixelFormat::RGBA);
    cbGeometry1->init();

    auto cbGeometry2 = make_shared<Texture>("geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    cbGeometry2->clear(w, h, PixelFormat::RGBA);
    cbGeometry2->init();

    auto dbGeometry = make_shared<Renderbuffer>();
    dbGeometry->configure(w, h, PixelFormat::Depth);
    dbGeometry->init();

    auto fbGeometry = make_unique<Framebuffer>();
    fbGeometry->attachColorsDepth(cbGeometry1, cbGeometry2, dbGeometry);
    fbGeometry->init();

    // Reusable ping-pong framebuffers

    auto cbPing = make_shared<Texture>("ping_color", getTextureProperties(TextureUsage::ColorBuffer));
    cbPing->clear(w, h, PixelFormat::RGBA);
    cbPing->init();

    auto cbPong = make_shared<Texture>("pong_color", getTextureProperties(TextureUsage::ColorBuffer));
    cbPong->clear(w, h, PixelFormat::RGBA);
    cbPong->init();

    auto dbCommon = make_shared<Renderbuffer>();
    dbCommon->configure(w, h, PixelFormat::Depth);
    dbCommon->init();

    auto fbPing = make_unique<Framebuffer>();
    fbPing->attachColorDepth(cbPing, dbCommon);
    fbPing->init();

    auto fbPong = make_unique<Framebuffer>();
    fbPong->attachColorDepth(cbPong, dbCommon);
    fbPong->init();

    // Attachments

    Attachments attachments;
    attachments.cbGeometry1MS = move(cbGeometry1MS);
    attachments.cbGeometry2MS = move(cbGeometry2MS);
    attachments.dbGeometryMS = move(dbGeometryMS);
    attachments.fbGeometryMS = move(fbGeometryMS);
    attachments.cbGeometry1 = move(cbGeometry1);
    attachments.cbGeometry2 = move(cbGeometry2);
    attachments.dbGeometry = move(dbGeometry);
    attachments.fbGeometry = move(fbGeometry);
    attachments.cbPing = move(cbPing);
    attachments.cbPong = move(cbPong);
    attachments.dbCommon = move(dbCommon);
    attachments.fbPing = move(fbPing);
    attachments.fbPong = move(fbPong);
    _attachments.insert(make_pair(attachmentsId, move(attachments)));
}

void ControlPipeline::draw(IScene &scene, const glm::ivec4 &extent, const glm::ivec2 &offset) {
    AttachmentsId attachmentsId {extent};
    auto maybeAttachments = _attachments.find(attachmentsId);
    if (maybeAttachments == _attachments.end()) {
        return;
    }
    auto &attachments = maybeAttachments->second;
    drawGeometry(scene, attachments, extent);
    applyBloom(attachments, extent);
    applyFXAA(attachments, extent);
    presentControl(attachments, extent, offset);
}

void ControlPipeline::drawGeometry(IScene &scene, Attachments &attachments, const glm::ivec4 &extent) {
    static constexpr GLenum colors[] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    auto camera = scene.camera();
    if (!camera) {
        return;
    }
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

    if (_options.aaMethod >= AntiAliasingMethods::msaa2) {
        // Draw scene to multi-sample geometry framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbGeometryMS.nameGL());
        glDrawBuffers(2, colors);
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
    } else {
        // Draw scene to geometry framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbGeometry.nameGL());
        glDrawBuffers(2, colors);
        _graphicsContext.withViewport(glm::ivec4(0, 0, w, h), [this, &w, &h, &scene]() {
            _graphicsContext.clearColorDepth();
            scene.draw();
        });
    }
}

void ControlPipeline::applyBloom(Attachments &attachments, const glm::ivec4 &extent) {
    int w = extent[2];
    int h = extent[3];

    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();

    // Combine geometry and bright geometry color buffers
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbPing->nameGL());
    _shaders.use(_shaders.bloom(), true);
    _textures.bind(*attachments.cbGeometry1);
    _textures.bind(*attachments.cbGeometry2, TextureUnits::bloom);
    _graphicsContext.withViewport(glm::ivec4(0, 0, w, h), [this, &attachments]() {
        _graphicsContext.clearColorDepth();
        _meshes.quadNDC().draw();
    });
}

void ControlPipeline::applyFXAA(Attachments &attachments, const glm::ivec4 &extent) {
    if (_options.aaMethod != AntiAliasingMethods::fxaa) {
        return;
    }
    int w = extent[2];
    int h = extent[3];

    // Reset uniforms
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.screenResolution = glm::vec2(extent[2], extent[3]);

    // Apply FXAA to ping (bloom) color buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, attachments.fbPong->nameGL());
    _shaders.use(_shaders.fxaa(), true);
    _textures.bind(*attachments.cbPing);
    _graphicsContext.withViewport(glm::ivec4(0, 0, w, h), [this, &attachments]() {
        _graphicsContext.clearColorDepth();
        _meshes.quadNDC().draw();
    });
}

void ControlPipeline::presentControl(Attachments &attachments, const glm::ivec4 &extent, const glm::ivec2 &offset) {
    // Set uniforms
    glm::mat4 projection(glm::ortho(
        0.0f,
        static_cast<float>(_options.width),
        static_cast<float>(_options.height),
        0.0f));
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(extent[0] + offset.x, extent[1] + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(extent[2], extent[3], 1.0f));

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.projection = move(projection);
    uniforms.general.model = move(transform);

    // Present ping (bloom) or pong (FXAA) color buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _shaders.use(_shaders.gui(), true);
    _textures.bind(_options.aaMethod == AntiAliasingMethods::fxaa ? *attachments.cbPong : *attachments.cbPing);
    _graphicsContext.withoutDepthTest([this]() {
        _meshes.quad().draw();
    });
}

} // namespace graphics

} // namespace reone
