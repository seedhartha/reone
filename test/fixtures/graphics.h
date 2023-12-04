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
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/pipeline.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textureregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/system/exception/notimplemented.h"

namespace reone {

namespace graphics {

class MockGraphicsContext : public IGraphicsContext, boost::noncopyable {
public:
    MOCK_METHOD(void, clearColor, (glm::vec4 color), (override));
    MOCK_METHOD(void, clearDepth, (), (override));
    MOCK_METHOD(void, clearColorDepth, (glm::vec4 color), (override));

    MOCK_METHOD(void, bind, (Texture & texture, int unit), (override));

    MOCK_METHOD(void, useProgram, (ShaderProgram &), (override));
    MOCK_METHOD(void, resetProgram, (), (override));

    MOCK_METHOD(void, bind, (UniformBuffer &, int), (override));
    MOCK_METHOD(UniformBuffer &, uniformBufferAt, (int), (override));

    MOCK_METHOD(void, withDepthTest, (DepthTestMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withFaceCulling, (CullFaceMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withBlending, (BlendMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withPolygonMode, (PolygonMode mode, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withViewport, (glm::ivec4 viewport, const std::function<void()> &block), (override));
    MOCK_METHOD(void, withScissorTest, (const glm::ivec4 &bounds, const std::function<void()> &block), (override));
};

class MockMeshRegistry : public IMeshRegistry, boost::noncopyable {
public:
    MOCK_METHOD(Mesh &, get, (const std::string &), (override));
};

class MockPipeline : public IPipeline, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Texture>, draw, (IScene & scene, const glm::ivec2 &dim), (override));
};

class MockShaderRegistry : public IShaderRegistry, boost::noncopyable {
public:
    MOCK_METHOD(ShaderProgram &, get, (const std::string &), (override));
};

class MockTextureRegistry : public ITextureRegistry, boost::noncopyable {
public:
    MOCK_METHOD(Texture &, get, (const std::string &), (override));
};

class MockUniforms : public IUniforms, boost::noncopyable {
public:
    MOCK_METHOD(void, setGlobals, (const std::function<void(GlobalsUniforms &)> &block), (override));
    MOCK_METHOD(void, setLocals, (const std::function<void(LocalsUniforms &)> &block), (override));
    MOCK_METHOD(void, setText, (const std::function<void(TextUniforms &)> &block), (override));
    MOCK_METHOD(void, setLighting, (const std::function<void(LightingUniforms &)> &block), (override));
    MOCK_METHOD(void, setSkeletal, (const std::function<void(SkeletalUniforms &)> &block), (override));
    MOCK_METHOD(void, setParticles, (const std::function<void(ParticlesUniforms &)> &block), (override));
    MOCK_METHOD(void, setGrass, (const std::function<void(GrassUniforms &)> &block), (override));
    MOCK_METHOD(void, setWalkmesh, (const std::function<void(WalkmeshUniforms &)> &block), (override));
    MOCK_METHOD(void, setPoints, (const std::function<void(PointsUniforms &)> &block), (override));
    MOCK_METHOD(void, setScreenSpace, (const std::function<void(ScreenSpaceUniforms &)> &block), (override));
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
        _context = std::make_unique<MockGraphicsContext>();
        _meshRegistry = std::make_unique<MockMeshRegistry>();
        _pipeline = std::make_unique<MockPipeline>();
        _shaderRegistry = std::make_unique<MockShaderRegistry>();
        _textureRegistry = std::make_unique<MockTextureRegistry>();
        _uniforms = std::make_unique<MockUniforms>();
        _window = std::make_unique<MockWindow>();

        _services = std::make_unique<GraphicsServices>(
            *_context,
            *_meshRegistry,
            *_pipeline,
            *_shaderRegistry,
            *_textureRegistry,
            *_uniforms,
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

    MockShaderRegistry &shaderRegistry() {
        return *_shaderRegistry;
    }

    GraphicsServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockGraphicsContext> _context;
    std::unique_ptr<MockMeshRegistry> _meshRegistry;
    std::unique_ptr<MockPipeline> _pipeline;
    std::unique_ptr<MockShaderRegistry> _shaderRegistry;
    std::unique_ptr<MockTextureRegistry> _textureRegistry;
    std::unique_ptr<MockUniforms> _uniforms;
    std::unique_ptr<MockWindow> _window;

    std::unique_ptr<GraphicsServices> _services;
};

} // namespace graphics

} // namespace reone
