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

#pragma once

#include <gmock/gmock.h>

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/fonts.h"
#include "reone/graphics/lipanimations.h"
#include "reone/graphics/meshes.h"
#include "reone/graphics/models.h"
#include "reone/graphics/pipeline.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/textures.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/walkmeshes.h"
#include "reone/graphics/window.h"
#include "reone/system/exception/notimplemented.h"

namespace reone {

namespace graphics {

class MockFonts : public IFonts, boost::noncopyable {
public:
    MOCK_METHOD(void, clear,(), (override));
    MOCK_METHOD(std::shared_ptr<Font>, get, (const std::string &key), (override));
};

class MockGraphicsContext : public IGraphicsContext, boost::noncopyable {
public:
    MOCK_METHOD(void, clearColor, (glm::vec4 color), (override));
    MOCK_METHOD(void, clearDepth, (), (override));
    MOCK_METHOD(void, clearColorDepth, (glm::vec4 color), (override));

    MOCK_METHOD(void, withDepthTest, (DepthTestMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withFaceCulling, (CullFaceMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withBlending, (BlendMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withPolygonMode, (PolygonMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withViewport, (glm::ivec4 viewport, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withScissorTest, (const glm::ivec4 &bounds, const std::function<void()> &block), (override));
};

class MockLipAnimations : public ILipAnimations, boost::noncopyable {
public:
    MOCK_METHOD(void, clear,(), (override));
    MOCK_METHOD(std::shared_ptr<LipAnimation>, get, (const std::string &key), (override));
};

class MockMeshes : public IMeshes, boost::noncopyable {
public:
    MOCK_METHOD(Mesh &, quad, (), (const override));
    MOCK_METHOD(Mesh &, quadNDC, (), (const override));
    MOCK_METHOD(Mesh &, billboard, (), (const override));
    MOCK_METHOD(Mesh &, grass, (), (const override));

    MOCK_METHOD(Mesh &, box, (), (const override));
    MOCK_METHOD(Mesh &, cubemap, (), (const override));
};

class MockModels : public IModels, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Model>, get, (const std::string &resRef), (override));
};

class MockPipeline : public IPipeline, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Texture>, draw, (IScene & scene, const glm::ivec2 &dim), (override));
};

class MockShaders : public IShaders, boost::noncopyable {
public:
    MOCK_METHOD(void, use, (ShaderProgramId programId), (override));
};

class MockTextures : public ITextures, boost::noncopyable {
public:
    MOCK_METHOD(void, clear,(), (override));

    MOCK_METHOD(void, bind, (Texture & texture, int unit), (override));
    MOCK_METHOD(void, bindBuiltIn, (), (override));

    MOCK_METHOD(std::shared_ptr<Texture>, get, (const std::string &resRef, TextureUsage usage), (override));

    // Built-in

    MOCK_METHOD(std::shared_ptr<Texture>, default2DRGB, (), (const override));
    MOCK_METHOD(std::shared_ptr<Texture>, defaultArrayDepth, (), (const override));
    MOCK_METHOD(std::shared_ptr<Texture>, defaultCubemapRGB, (), (const override));
    MOCK_METHOD(std::shared_ptr<Texture>, defaultCubemapDepth, (), (const override));

    MOCK_METHOD(std::shared_ptr<Texture>, noiseRG, (), (const override));
    MOCK_METHOD(std::shared_ptr<Texture>, ssaoRGB, (), (const override));
    MOCK_METHOD(std::shared_ptr<Texture>, ssrRGBA, (), (const override));

    // END Built-in
};

class MockUniforms : public IUniforms, boost::noncopyable {
public:
    MOCK_METHOD(void, setGeneral, (const std::function<void(GeneralUniforms &)> &block), (override));
    MOCK_METHOD(void, setText, (const std::function<void(TextUniforms &)> &block), (override));
    MOCK_METHOD(void, setLighting, (const std::function<void(LightingUniforms &)> &block), (override));
    MOCK_METHOD(void, setSkeletal, (const std::function<void(SkeletalUniforms &)> &block), (override));
    MOCK_METHOD(void, setParticles, (const std::function<void(ParticlesUniforms &)> &block), (override));
    MOCK_METHOD(void, setGrass, (const std::function<void(GrassUniforms &)> &block), (override));
    MOCK_METHOD(void, setSSAO, (const std::function<void(SSAOUniforms &)> &block), (override));
    MOCK_METHOD(void, setWalkmesh, (const std::function<void(WalkmeshUniforms &)> &block), (override));
    MOCK_METHOD(void, setPoints, (const std::function<void(PointsUniforms &)> &block), (override));
};

class MockWalkmeshes : public IWalkmeshes, boost::noncopyable {
public:
    MOCK_METHOD(void, clear,(), (override));

    MOCK_METHOD(std::shared_ptr<Walkmesh>, get, (const std::string &resRef, resource::ResourceType type), (override));
};

class MockWindow : public IWindow, boost::noncopyable {
public:
    MOCK_METHOD(void, processEvents, (bool &quit), (override));
    MOCK_METHOD(void, swapBuffers, (), (const override));
    MOCK_METHOD(bool, isInFocus, (), (const override));
    MOCK_METHOD(glm::mat4, getOrthoProjection, (float near, float far), (const override));
    MOCK_METHOD(void, setEventHandler, (IEventHandler * eventHandler), (override));
    MOCK_METHOD(void, setRelativeMouseMode, (bool enabled), (override));
    MOCK_METHOD(uint32_t, mouseState, (int *x, int *y), (override));
    MOCK_METHOD(void, showCursor, (bool show), (override));
};

class TestGraphicsModule : boost::noncopyable {
public:
    void init() {
        _fonts = std::make_unique<MockFonts>();
        _context = std::make_unique<MockGraphicsContext>();
        _lipAnimations = std::make_unique<MockLipAnimations>();
        _meshes = std::make_unique<MockMeshes>();
        _models = std::make_unique<MockModels>();
        _pipeline = std::make_unique<MockPipeline>();
        _shaders = std::make_unique<MockShaders>();
        _textures = std::make_unique<MockTextures>();
        _uniforms = std::make_unique<MockUniforms>();
        _walkmeshes = std::make_unique<MockWalkmeshes>();
        _window = std::make_unique<MockWindow>();

        _services = std::make_unique<GraphicsServices>(
            *_fonts,
            *_context,
            *_lipAnimations,
            *_meshes,
            *_models,
            *_pipeline,
            *_shaders,
            *_textures,
            *_uniforms,
            *_walkmeshes,
            *_window);
    }

    MockGraphicsContext &context() {
        return *_context;
    }

    MockPipeline &pipeline() {
        return *_pipeline;
    }

    MockWindow &window() {
        return *_window;
    }

    GraphicsServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockFonts> _fonts;
    std::unique_ptr<MockGraphicsContext> _context;
    std::unique_ptr<MockLipAnimations> _lipAnimations;
    std::unique_ptr<MockMeshes> _meshes;
    std::unique_ptr<MockModels> _models;
    std::unique_ptr<MockPipeline> _pipeline;
    std::unique_ptr<MockShaders> _shaders;
    std::unique_ptr<MockTextures> _textures;
    std::unique_ptr<MockUniforms> _uniforms;
    std::unique_ptr<MockWalkmeshes> _walkmeshes;
    std::unique_ptr<MockWindow> _window;

    std::unique_ptr<GraphicsServices> _services;
};

} // namespace graphics

} // namespace reone
