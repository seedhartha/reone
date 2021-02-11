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

#include "irradiancemaps.h"

#include "glm/ext.hpp"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "mesh/cubemap.h"
#include "shaders.h"
#include "stateutil.h"

using namespace std;

namespace reone {

namespace render {

IrradianceMaps &IrradianceMaps::instance() {
    static IrradianceMaps instance;
    return instance;
}

IrradianceMaps::IrradianceMaps() : _framebuffer(32, 32, 0) {
}

void IrradianceMaps::init() {
    _framebuffer.init();
}

IrradianceMaps::~IrradianceMaps() {
    deinit();
}

void IrradianceMaps::deinit() {
    _irradianceByEnvmap.clear();
    _computeQueue.clear();
}

void IrradianceMaps::refresh() {
    for (auto &envmap : _computeQueue) {
        _irradianceByEnvmap.insert(make_pair(envmap, computeIrradianceMap(envmap)));
    }
    _computeQueue.clear();
}

shared_ptr<Texture> IrradianceMaps::get(const Texture *envmap) {
    auto maybeIrradiance = _irradianceByEnvmap.find(envmap);
    if (maybeIrradiance != _irradianceByEnvmap.end()) return maybeIrradiance->second;

    // If irradiance map is not found, schedule its computation
    _computeQueue.insert(envmap);

    return nullptr;
}

shared_ptr<Texture> IrradianceMaps::computeIrradianceMap(const Texture *envmap) {
    static glm::mat4 captureProjection(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));
    static glm::mat4 captureViews[] {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    static glm::vec4 captureViewport(0.0f, 0.0f, 32.0f, 32.0f);

    auto irradianceMap = make_shared<Texture>(envmap->name() + "_irradiance", TextureType::IrradianceMap, 32, 32);
    irradianceMap->init();
    irradianceMap->clearPixels(PixelFormat::RGBA);

    withViewport(captureViewport, [&]() {
        _framebuffer.bind();

        setActiveTextureUnit(TextureUnits::envmap);
        envmap->bind();

        for (int i = 0; i < kNumCubeFaces; ++i) {
            // TODO: set view matrix as part of local uniforms
            GlobalUniforms globals;
            globals.projection = captureProjection;
            globals.view = captureViews[i];
            Shaders::instance().setGlobalUniforms(globals);

            LocalUniforms locals;
            locals.general.envmapEnabled = true;
            Shaders::instance().activate(ShaderProgram::GUIIrradiance, locals);

            // TODO: abstract that away
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap->textureId(), 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            CubeMapMesh::instance().renderTriangles();
        }

        _framebuffer.unbind();
    });

    return _irradianceByEnvmap.insert(make_pair(envmap, move(irradianceMap))).first->second;
}

} // namespace render

} // namespace reone
