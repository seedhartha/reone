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

#include "pbribl.h"

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

#include "meshes.h"
#include "renderbuffer.h"
#include "shaders.h"
#include "stateutil.h"
#include "texture.h"
#include "textureutil.h"

using namespace std;

namespace reone {

namespace render {

static constexpr int kNumPrefilterMipMaps = 5;

static const glm::mat4 g_captureProjection(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));

static const glm::mat4 g_captureViews[] {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

PBRIBL &PBRIBL::instance() {
    static PBRIBL instance;
    return instance;
}

void PBRIBL::init() {
    _irradianceFB.init();
    _prefilterFB.init();
    _brdfLookupFB.init();
}

PBRIBL::~PBRIBL() {
    deinit();
}

void PBRIBL::deinit() {
    _derivedByEnvmap.clear();
    _envmapQueue.clear();
}

void PBRIBL::refresh() {
    for (auto &envmap : _envmapQueue) {
        Derived derived;
        derived.irradianceMap = computeIrradianceMap(envmap);
        derived.prefilterMap = computePrefilterMap(envmap);
        derived.brdfLookup = computeBRDFLookup(envmap);
        _derivedByEnvmap.insert(make_pair(envmap, move(derived)));
    }
    _envmapQueue.clear();
}

bool PBRIBL::contains(const Texture *envmap) {
    return _derivedByEnvmap.count(envmap) > 0;
}

bool PBRIBL::getDerived(const Texture *envmap, Derived &derived) {
    auto maybeDerived = _derivedByEnvmap.find(envmap);
    if (maybeDerived != _derivedByEnvmap.end()) {
        derived = maybeDerived->second;
        return true;
    }

    // If derived textures are not found, schedule their computation
    _envmapQueue.insert(envmap);

    return false;
}

shared_ptr<Texture> PBRIBL::computeIrradianceMap(const Texture *envmap) {
    static glm::ivec4 viewport(0, 0, 32, 32);

    auto irradianceColor = make_shared<Texture>(envmap->name() + "_irradiance_color", getTextureProperties(TextureUsage::IrradianceMap));
    irradianceColor->init();
    irradianceColor->bind();
    irradianceColor->clearPixels(32, 32, PixelFormat::RGB);

    auto irradianceDepth = make_shared<Renderbuffer>();
    irradianceDepth->init();
    irradianceDepth->bind();
    irradianceDepth->configure(32, 32, PixelFormat::Depth);

    _irradianceFB.bind();

    withViewport(viewport, [&]() {
        setActiveTextureUnit(TextureUnits::envmap);
        envmap->bind();

        for (int i = 0; i < kNumCubeFaces; ++i) {
            _irradianceFB.attachCubeMapFaceAsColor(*irradianceColor, static_cast<CubeMapFace>(i));
            _irradianceFB.attachDepth(*irradianceDepth);
            _irradianceFB.checkCompleteness();

            // TODO: set view matrix as part of local uniforms
            GlobalUniforms globals;
            globals.projection = g_captureProjection;
            globals.view = g_captureViews[i];
            Shaders::instance().setGlobalUniforms(globals);
            Shaders::instance().activate(ShaderProgram::SimpleIrradiance, LocalUniforms());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Meshes::instance().getCubemap().render();
        }
    });

    _irradianceFB.unbind();

    return move(irradianceColor);
}

shared_ptr<Texture> PBRIBL::computePrefilterMap(const Texture *envmap) {
    auto prefilterColor = make_shared<Texture>(envmap->name() + "_prefilter_color", getTextureProperties(TextureUsage::PrefilterMap));
    prefilterColor->init();
    prefilterColor->bind();
    prefilterColor->clearPixels(128, 128, PixelFormat::RGB);

    auto prefilterDepth = make_shared<Renderbuffer>();
    prefilterDepth->init();
    prefilterDepth->bind();
    prefilterDepth->configure(128, 128, PixelFormat::Depth);

    _prefilterFB.bind();

    for (int mip = 0; mip < kNumPrefilterMipMaps; ++mip) {
        int mipWidth = static_cast<int>(128 * glm::pow(0.5, mip));
        int mipHeight = static_cast<int>(128 * glm::pow(0.5, mip));

        for (int face = 0; face < kNumCubeFaces; ++face) {
            glm::ivec4 viewport(0, 0, mipWidth, mipHeight);
            withViewport(viewport, [&]() {
                _prefilterFB.attachCubeMapFaceAsColor(*prefilterColor, static_cast<CubeMapFace>(face), 0, mip);
                _prefilterFB.attachDepth(*prefilterDepth);
                _prefilterFB.checkCompleteness();

                // TODO: set view matrix as part of local uniforms
                GlobalUniforms globals;
                globals.projection = g_captureProjection;
                globals.view = g_captureViews[face];
                Shaders::instance().setGlobalUniforms(globals);

                LocalUniforms locals;
                locals.general.roughness = mip / static_cast<float>(kNumPrefilterMipMaps - 1);
                Shaders::instance().activate(ShaderProgram::SimplePrefilter, locals);

                setActiveTextureUnit(TextureUnits::envmap);
                envmap->bind();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Meshes::instance().getCubemap().render();
            });
        }
    }

    _prefilterFB.unbind();

    return move(prefilterColor);
}

shared_ptr<Texture> PBRIBL::computeBRDFLookup(const Texture *envmap) {
    static glm::ivec4 viewport(0, 0, 512, 512);

    auto brdfLookupColor = make_shared<Texture>(envmap->name() + "_brdf_color", getTextureProperties(TextureUsage::BRDFLookup));
    brdfLookupColor->init();
    brdfLookupColor->bind();
    brdfLookupColor->clearPixels(512, 512, PixelFormat::RGB);

    auto brdfLookupDepth = make_shared<Renderbuffer>();
    brdfLookupDepth->init();
    brdfLookupDepth->bind();
    brdfLookupDepth->configure(512, 512, PixelFormat::Depth);

    _brdfLookupFB.bind();
    _brdfLookupFB.attachColor(*brdfLookupColor);
    _brdfLookupFB.attachDepth(*brdfLookupDepth);
    _brdfLookupFB.checkCompleteness();

    withViewport(viewport, [&]() {
        Shaders::instance().setGlobalUniforms(GlobalUniforms());
        Shaders::instance().activate(ShaderProgram::SimpleBRDF, LocalUniforms());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Meshes::instance().getQuadNDC().render();
    });

    _brdfLookupFB.unbind();

    return move(brdfLookupColor);
}

} // namespace render

} // namespace reone
