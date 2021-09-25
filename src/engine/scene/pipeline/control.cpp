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

#include "../../graphics/context.h"
#include "../../graphics/mesh/mesh.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/texture/texture.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/texture/textureutil.h"

#include "../node/camera.h"
#include "../scenegraph.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

void ControlRenderPipeline::init() {
    _geometryColor = make_unique<Texture>("geometry_color", getTextureProperties(TextureUsage::ColorBuffer));
    _geometryColor->init();
    _geometryColor->bind();
    _geometryColor->clearPixels(static_cast<int>(_extent[2]), static_cast<int>(_extent[3]), PixelFormat::RGBA);
    _geometryColor->unbind();

    _geometryDepth = make_unique<Renderbuffer>();
    _geometryDepth->init();
    _geometryDepth->bind();
    _geometryDepth->configure(static_cast<int>(_extent[2]), static_cast<int>(_extent[3]), PixelFormat::Depth);
    _context.unbindRenderbuffer();

    _geometry.init();
    _geometry.bind();
    _geometry.attachColor(*_geometryColor);
    _geometry.attachDepth(*_geometryDepth);
    _geometry.checkCompleteness();
    _context.unbindFramebuffer();
}

void ControlRenderPipeline::render(const glm::ivec2 &offset) {
    // Set uniforms prototype

    shared_ptr<CameraSceneNode> camera(_sceneGraph.activeCamera());

    ShaderUniforms uniforms(_shaders.defaultUniforms());
    uniforms.combined.general.projection = camera->projection();
    uniforms.combined.general.view = camera->view();
    uniforms.combined.general.cameraPosition = camera->absoluteTransform()[3];

    _sceneGraph.setUniformsPrototype(move(uniforms));

    // Draw to framebuffer

    glm::ivec4 oldViewport(_context.viewport());
    _context.setViewport(glm::ivec4(0, 0, _extent[2], _extent[3]));

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    _geometry.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _sceneGraph.draw();

    _context.unbindFramebuffer();
    _context.setDepthTestEnabled(oldDepthTest);
    _context.setViewport(oldViewport);

    // Draw control

    _context.setActiveTextureUnit(TextureUnits::diffuseMap);
    _geometryColor->bind();

    glm::mat4 projection(glm::ortho(
        0.0f,
        static_cast<float>(oldViewport[2]),
        static_cast<float>(oldViewport[3]),
        0.0f));

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent[0] + offset.x, _extent[1] + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent[2], _extent[3], 1.0f));

    uniforms = ShaderUniforms();
    uniforms.combined.general.projection = move(projection);
    uniforms.combined.general.model = move(transform);

    _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
    _meshes.quad().draw();
}

} // namespace scene

} // namespace reone
