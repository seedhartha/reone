/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/pbrtextures.h"

#include "reone/graphics/context.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/system/logutil.h"

static constexpr int kBRDFTextureSize = 512;
static constexpr int kIrradianceTextureSize = 32;
static constexpr int kPrefilteredTextureSize = 128;
static constexpr int kNumPrefilteredMipMaps = 5;
static constexpr int kMaxEnvMapDerivedLayers = 16;

static const glm::mat4 kCubeMapProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
static const glm::mat4 kCubeMapViews[] {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) //
};

namespace reone {

namespace graphics {

void PBRTextures::refresh() {
    if (!_brdfLUT) {
        initBRDFLUT();
    }
    if (!_irradianceMapArray) {
        initIrradianceMapArray();
    }
    if (!_prefilteredEnvMapArray) {
        initPrefilteredEnvMapArray();
    }
    if (!_envMapDerivedRequests.empty()) {
        auto &request = *_envMapDerivedRequests.begin();
        refreshEnvMapDerived(request);
        _envMapDerivedRequests.erase(request);
    }
}

void PBRTextures::initBRDFLUT() {
    _brdfLUT = std::make_unique<Texture>(
        "pbr_color_buffer",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::ColorBuffer));
    _brdfLUT->clear(kBRDFTextureSize, kBRDFTextureSize, PixelFormat::RG16F);
    _brdfLUT->init();

    _brdfDepthBuffer = std::make_unique<Renderbuffer>();
    _brdfDepthBuffer->configure(kBRDFTextureSize, kBRDFTextureSize, PixelFormat::Depth24);
    _brdfDepthBuffer->init();

    _brdfFramebuffer = std::make_unique<Framebuffer>();
    _brdfFramebuffer->attachColorDepth(_brdfLUT, _brdfDepthBuffer);
    _brdfFramebuffer->init();

    _context.withViewport(glm::ivec4 {0, 0, kBRDFTextureSize, kBRDFTextureSize}, [this]() {
        _context.bindDrawFramebuffer(*_brdfFramebuffer, {0});
        auto &shader = _shaderRegistry.get(ShaderProgramId::brdfLUT);
        _context.useProgram(shader);
        _uniforms.setGlobals([](auto &globals) {
            globals.reset();
        });
        _uniforms.setLocals([](auto &locals) {
            locals.reset();
        });
        _context.clearColorDepth();
        _meshRegistry.get(MeshName::quadNDC).draw();
        _context.resetDrawFramebuffer();
    });
}

void PBRTextures::initIrradianceMapArray() {
    _irradianceMapArray = std::make_shared<Texture>(
        "pbr_irradiance_map_array",
        TextureType::CubeMapArray,
        getTextureProperties(TextureUsage::ColorBuffer));
    _irradianceMapArray->clear(
        kIrradianceTextureSize,
        kIrradianceTextureSize,
        PixelFormat::RGB8,
        kMaxEnvMapDerivedLayers * kNumCubeFaces);
    _irradianceMapArray->init();

    _irradianceDepthBuffer = std::make_unique<Renderbuffer>();
    _irradianceDepthBuffer->configure(kIrradianceTextureSize, kIrradianceTextureSize, PixelFormat::Depth24);
    _irradianceDepthBuffer->init();

    _irradianceFramebuffer = std::make_unique<Framebuffer>();
    _irradianceFramebuffer->attachDepth(_irradianceDepthBuffer);
    _irradianceFramebuffer->init();
}

void PBRTextures::initPrefilteredEnvMapArray() {
    _prefilteredEnvMapArray = std::make_shared<Texture>(
        "pbr_prefiltered_env_map_array",
        TextureType::CubeMapArray,
        getTextureProperties(TextureUsage::ColorBuffer));
    _prefilteredEnvMapArray->clear(
        kPrefilteredTextureSize,
        kPrefilteredTextureSize,
        PixelFormat::RGB8,
        kMaxEnvMapDerivedLayers * kNumCubeFaces);
    _prefilteredEnvMapArray->init();

    for (int mip = 0; mip < kNumPrefilteredMipMaps; ++mip) {
        int w = static_cast<int>(kPrefilteredTextureSize * std::pow(0.5, mip));
        int h = static_cast<int>(kPrefilteredTextureSize * std::pow(0.5, mip));
        auto depthBuffer = std::make_unique<Renderbuffer>();
        depthBuffer->configure(w, h, PixelFormat::Depth24);
        depthBuffer->init();
        _prefilterDepthBuffers.push_back(std::move(depthBuffer));
    }

    _prefilterFramebuffer = std::make_unique<Framebuffer>();
    _prefilterFramebuffer->init();
}

void PBRTextures::refreshEnvMapDerived(const EnvMapDerivedRequest &request) {
    refreshIrradianceMap(request, _envMapDerivedLayer);
    refreshPrefilteredEnvMap(request, _envMapDerivedLayer);
    _envMapToDerivedLayer.insert({request.texture.name(), _envMapDerivedLayer});
    if (++_envMapDerivedLayer == kMaxEnvMapDerivedLayers) {
        _envMapDerivedLayer = 0;
    }
}

void PBRTextures::refreshIrradianceMap(const EnvMapDerivedRequest &request, int layer) {
    _context.bindDrawFramebuffer(*_irradianceFramebuffer, {0});
    auto &shader = _shaderRegistry.get(ShaderProgramId::pbrIrradiance);
    _context.useProgram(shader);
    _context.bindTexture(request.texture, TextureUnits::envMapCube);
    _uniforms.setLocals([](auto &locals) {
        locals.reset();
    });
    _context.withViewport(glm::ivec4 {0, 0, kIrradianceTextureSize, kIrradianceTextureSize}, [this, &layer]() {
        for (int i = 0; i < kNumCubeFaces; ++i) {
            _irradianceFramebuffer->attachTextureLayer(*_irradianceMapArray, kNumCubeFaces * layer + i, 0, Framebuffer::Attachment::Color);
            _uniforms.setGlobals([&i](auto &globals) {
                globals.reset();
                globals.projection = kCubeMapProjection;
                globals.view = kCubeMapViews[i];
            });
            _context.clearColorDepth();
            _meshRegistry.get(MeshName::cubemap).draw();
        }
    });
    _context.resetDrawFramebuffer();
}

void PBRTextures::refreshPrefilteredEnvMap(const EnvMapDerivedRequest &request, int layer) {
    _context.bindDrawFramebuffer(*_prefilterFramebuffer, {0});
    auto &shader = _shaderRegistry.get(ShaderProgramId::pbrPrefilter);
    _context.useProgram(shader);
    _context.bindTexture(request.texture, TextureUnits::envMapCube);
    _uniforms.setLocals([](auto &locals) {
        locals.reset();
    });
    for (int mip = 0; mip < kNumPrefilteredMipMaps; ++mip) {
        int w = static_cast<int>(kPrefilteredTextureSize * std::pow(0.5, mip));
        int h = static_cast<int>(kPrefilteredTextureSize * std::pow(0.5, mip));
        _context.withViewport(glm::ivec4 {0, 0, w, h}, [this, &request, &layer, &shader, &mip]() {
            for (int i = 0; i < kNumCubeFaces; ++i) {
                _irradianceFramebuffer->attachTextureLayer(*_prefilteredEnvMapArray, kNumCubeFaces * layer + i, mip, Framebuffer::Attachment::Color);
                _irradianceFramebuffer->attachRenderbuffer(*_prefilterDepthBuffers[mip], Framebuffer::Attachment::Depth);
                _uniforms.setGlobals([&i](auto &globals) {
                    globals.reset();
                    globals.projection = kCubeMapProjection;
                    globals.view = kCubeMapViews[i];
                });
                float roughness = mip / static_cast<float>(kNumPrefilteredMipMaps - 1);
                shader.setUniform("uRoughness", mip);
                _context.clearColorDepth();
                _meshRegistry.get(MeshName::cubemap).draw();
            }
        });
    }
    _context.resetDrawFramebuffer();

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _prefilteredEnvMapArray->nameGL());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP_ARRAY);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
}

} // namespace graphics

} // namespace reone
