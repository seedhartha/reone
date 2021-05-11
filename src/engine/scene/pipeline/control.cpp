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

#include "GL/glew.h"

#include "glm/ext.hpp"

#include "../../graphics/mesh/meshes.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/statemanager.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/texture/textureutil.h"

#include "../node/cameranode.h"
#include "../scenegraph.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

ControlRenderPipeline::ControlRenderPipeline(SceneGraph *scene, glm::ivec4 extent) :
    _scene(scene), _extent(move(extent)) {
}

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
    _geometryDepth->unbind();

    _geometry.init();
    _geometry.bind();
    Textures::instance().bindDefaults();
    _geometry.attachColor(*_geometryColor);
    _geometry.attachDepth(*_geometryDepth);
    _geometry.checkCompleteness();
    _geometry.unbind();
}

void ControlRenderPipeline::render(const glm::ivec2 &offset) {
    // Render to framebuffer

    StateManager::instance().withViewport(glm::ivec4(0, 0, _extent[2], _extent[3]), [this]() {
        _geometry.bind();

        ShaderUniforms uniforms(Shaders::instance().defaultUniforms());
        uniforms.combined.general.projection = _scene->activeCamera()->projection();
        uniforms.combined.general.view = _scene->activeCamera()->view();
        uniforms.combined.general.cameraPosition = _scene->activeCamera()->absoluteTransform()[3];
        _scene->setUniformsPrototype(move(uniforms));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        StateManager::instance().withDepthTest([this]() { _scene->draw(); });

        _geometry.unbind();
    });


    // Render control

    StateManager::instance().setActiveTextureUnit(TextureUnits::diffuse);
    _geometryColor->bind();

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);

    glm::mat4 projection(glm::ortho(
        0.0f,
        static_cast<float>(viewport[2]),
        static_cast<float>(viewport[3]),
        0.0f));

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent[0] + offset.x, _extent[1] + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent[2], _extent[3], 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = move(projection);
    uniforms.combined.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
    Meshes::instance().getQuad()->draw();
}

} // namespace scene

} // namespace reone
