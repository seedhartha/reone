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

#include "context.h"
#include "mesh.h"
#include "meshes.h"
#include "renderbuffer.h"
#include "shaders.h"
#include "texture.h"
#include "textureutil.h"

using namespace std;

namespace reone {

namespace graphics {

static constexpr int kNumPrefilterMipMaps = 5;

static const glm::mat4 g_captureProjection(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));

static const glm::mat4 g_captureViews[] {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

PBRIBL::PBRIBL(Context &context, Meshes &meshes, Shaders &shaders) :
    _context(context),
    _meshes(meshes),
    _shaders(shaders) {
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
    static glm::ivec4 irradianceViewport(0, 0, 32, 32);

    // Set viewport

    glm::ivec4 oldViewport(_context.viewport());
    _context.setViewport(irradianceViewport);

    // Bind irradiance framebuffer

    auto irradianceColor = make_shared<Texture>(envmap->name() + "_irradiance_color", getTextureProperties(TextureUsage::IrradianceMap));
    irradianceColor->init();
    irradianceColor->bind();
    irradianceColor->clearPixels(32, 32, PixelFormat::RGB);

    auto irradianceDepth = make_shared<Renderbuffer>();
    irradianceDepth->init();
    irradianceDepth->bind();
    irradianceDepth->configure(32, 32, PixelFormat::Depth);

    _irradianceFB.bind();
    _irradianceFB.attachDepth(*irradianceDepth);

    // Bind environment map

    _context.setActiveTextureUnit(TextureUnits::environmentMap);
    envmap->bind();

    // Draw cube sides

    for (int i = 0; i < kNumCubeFaces; ++i) {
        _irradianceFB.attachCubeMapFaceAsColor(*irradianceColor, static_cast<CubeMapFace>(i));
        _irradianceFB.checkCompleteness();

        ShaderUniforms uniforms;
        uniforms.combined.general.projection = g_captureProjection;
        uniforms.combined.general.view = g_captureViews[i];

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _shaders.activate(ShaderProgram::SimpleIrradiance, uniforms);
        _meshes.cubemap().draw();
    }

    // Restore context

    _context.unbindFramebuffer();
    _context.setViewport(move(oldViewport));

    return move(irradianceColor);
}

shared_ptr<Texture> PBRIBL::computePrefilterMap(const Texture *envmap) {
    // Bind prefiltered map framebuffer

    auto prefilterColor = make_shared<Texture>(envmap->name() + "_prefilter_color", getTextureProperties(TextureUsage::PrefilterMap));
    prefilterColor->init();
    prefilterColor->bind();
    prefilterColor->clearPixels(128, 128, PixelFormat::RGB);

    auto prefilterDepth = make_shared<Renderbuffer>();
    prefilterDepth->init();
    prefilterDepth->bind();

    _prefilterFB.bind();
    _prefilterFB.attachDepth(*prefilterDepth);

    // Bind environment map

    _context.setActiveTextureUnit(TextureUnits::environmentMap);
    envmap->bind();

    // Draw cube sides

    glm::ivec4 oldViewport(_context.viewport());

    for (int mip = 0; mip < kNumPrefilterMipMaps; ++mip) {
        int mipWidth = static_cast<int>(128 * glm::pow(0.5f, mip));
        int mipHeight = static_cast<int>(128 * glm::pow(0.5f, mip));
        prefilterDepth->configure(mipWidth, mipHeight, PixelFormat::Depth);

        glm::ivec4 prefilterViewport(0, 0, mipWidth, mipHeight);
        _context.setViewport(move(prefilterViewport));

        float roughness = mip / static_cast<float>(kNumPrefilterMipMaps - 1);

        for (int face = 0; face < kNumCubeFaces; ++face) {
            _prefilterFB.attachCubeMapFaceAsColor(*prefilterColor, static_cast<CubeMapFace>(face), 0, mip);
            _prefilterFB.checkCompleteness();

            ShaderUniforms uniforms(_shaders.defaultUniforms());
            uniforms.combined.general.projection = g_captureProjection;
            uniforms.combined.general.view = g_captureViews[face];
            uniforms.combined.general.roughness = roughness;
            uniforms.combined.general.envmapResolution = static_cast<float>(envmap->width());
            _shaders.activate(ShaderProgram::SimplePrefilter, uniforms);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _meshes.cubemap().draw();
        }
    }

    // Restore context

    _context.unbindFramebuffer();
    _context.setViewport(move(oldViewport));

    return move(prefilterColor);
}

shared_ptr<Texture> PBRIBL::computeBRDFLookup(const Texture *envmap) {
    static glm::ivec4 brdfViewport(0, 0, 512, 512);

    // Set viewport

    glm::ivec4 oldViewport(_context.viewport());
    _context.setViewport(brdfViewport);

    // Bind BRDF lookup framebuffer

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

    // Draw a quad

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shaders.activate(ShaderProgram::SimpleBRDF, _shaders.defaultUniforms());
    _meshes.quadNDC().draw();

    // Restore context

    _context.unbindFramebuffer();
    _context.setViewport(move(oldViewport));

    return move(brdfLookupColor);
}

} // namespace graphics

} // namespace reone
