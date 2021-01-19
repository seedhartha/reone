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

#include "glm/ext.hpp"

#include "GL/glew.h"

#include "../../render/mesh/quad.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

ControlRenderPipeline::ControlRenderPipeline(SceneGraph *scene, const glm::ivec4 &extent) :
    _scene(scene),
    _extent(extent),
    _geometry(_extent[2], _extent[3]) {
}

void ControlRenderPipeline::init() {
    _geometry.init();
}

void ControlRenderPipeline::render(const glm::ivec2 &offset) const {
    // Render to framebuffer

    withViewport(glm::ivec4(0, 0, _extent[2], _extent[3]), [this]() {
        _geometry.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        withDepthTest([this]() { _scene->render(); });

        _geometry.unbind();
    });


    // Render control

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent[0] + offset.x, _extent[1] + offset.y, 100.0f));
    transform = glm::scale(transform, glm::vec3(_extent[2], _extent[3], 1.0f));

    GlobalUniforms globals;
    globals.projection = glm::ortho(
        0.0f,
        static_cast<float>(viewport[2]),
        static_cast<float>(viewport[3]),
        0.0f,
        -100.0f, 100.0f);

    Shaders::instance().setGlobalUniforms(globals);

    LocalUniforms locals;
    locals.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    setActiveTextureUnit(0);
    _geometry.bindColorBuffer(0);

    Quad::getDefault().renderTriangles();

    _geometry.unbindColorBuffer(0);
}

} // namespace scene

} // namespace reone
