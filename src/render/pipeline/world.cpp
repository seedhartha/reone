/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "world.h"

#include "glm/ext.hpp"

#include "GL/glew.h"

#include "../mesh/quad.h"
#include "../shaders.h"
#include "../util.h"

using namespace std;

namespace reone {

namespace render {

static const int kBlurPassCount = 1;

WorldRenderPipeline::WorldRenderPipeline(IRenderable *scene, const GraphicsOptions &opts) :
    _scene(scene),
    _opts(opts),
    _geometry(opts.width, opts.height, 2),
    _verticalBlur(opts.width, opts.height),
    _horizontalBlur(opts.width, opts.height) {
}

void WorldRenderPipeline::init() {
    _geometry.init();
    _verticalBlur.init();
    _horizontalBlur.init();
}

void WorldRenderPipeline::render() const {
    {
        // Render geometry
        _geometry.bind();

        static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, buffers);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        withDepthTest([this]() { _scene->render(); });

        _geometry.unbind();
    }
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    GlobalUniforms globals;
    globals.projection = glm::ortho(0.0f, w, h, 0.0f);

    Shaders::instance().setGlobalUniforms(globals);

    for (int i = 0; i < kBlurPassCount; ++i) {
        {
            // Apply horizontal blur
            _horizontalBlur.bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 transform(1.0f);
            transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

            LocalUniforms locals;
            locals.model = move(transform);
            locals.features.blurEnabled = true;
            locals.blur.resolution = glm::vec2(w, h);
            locals.blur.direction = glm::vec2(1.0f, 0.0f);

            Shaders::instance().activate(ShaderProgram::GUIBlur, locals);

            glActiveTexture(GL_TEXTURE0);
            if (i == 0) {
                _geometry.bindColorBuffer(1);
            } else {
                _verticalBlur.bindColorBuffer(0);
            }

            withDepthTest([]() {
                Quad::getDefault().renderTriangles();
            });

            _geometry.unbindColorBuffer();
            _horizontalBlur.unbind();
        }
        {
            // Apply vertical blur
            _verticalBlur.bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 transform(1.0f);
            transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

            LocalUniforms locals;
            locals.model = move(transform);
            locals.features.blurEnabled = true;
            locals.blur.resolution = glm::vec2(_opts.width, _opts.height);
            locals.blur.direction = glm::vec2(0.0f, 1.0f);

            Shaders::instance().activate(ShaderProgram::GUIBlur, locals);

            glActiveTexture(GL_TEXTURE0);
            _horizontalBlur.bindColorBuffer(0);

            withDepthTest([]() {
                Quad::getDefault().renderTriangles();
            });

            _horizontalBlur.unbindColorBuffer();
            _verticalBlur.unbind();
        }
    }
    {
        glm::mat4 transform(1.0f);
        transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

        LocalUniforms locals;
        locals.model = move(transform);
        locals.features.bloomEnabled = true;

        Shaders::instance().activate(ShaderProgram::GUIBloom, locals);

        glActiveTexture(GL_TEXTURE0);
        _geometry.bindColorBuffer(0);

        glActiveTexture(GL_TEXTURE0 + locals.textures.bloom);
        _verticalBlur.bindColorBuffer(0);

        Quad::getDefault().renderTriangles();

        glActiveTexture(GL_TEXTURE + locals.textures.bloom);
        _verticalBlur.unbindColorBuffer();

        glActiveTexture(GL_TEXTURE0);
        _geometry.unbindColorBuffer();
    }
}

} // namespace render

} // namespace reone
