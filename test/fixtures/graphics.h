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

#include "reone/graphics/context.h"
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

#include "reone/graphics/services.h"

namespace reone {

namespace graphics {

class MockFonts : public IFonts, boost::noncopyable {
};

class MockGraphicsContext : public IGraphicsContext, boost::noncopyable {
};

class MockLipAnimations : public ILipAnimations, boost::noncopyable {
};

class MockMeshes : public IMeshes, boost::noncopyable {
};

class MockModels : public IModels, boost::noncopyable {
public:
    std::shared_ptr<Model> get(const std::string &resRef) override {
        return nullptr;
    }
};

class MockPipeline : public IPipeline, boost::noncopyable {
};

class MockShaders : public IShaders, boost::noncopyable {
};

class MockTextures : public ITextures, boost::noncopyable {
public:
    std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage) override {
        return nullptr;
    }
};

class MockUniforms : public IUniforms, boost::noncopyable {
};

class MockWalkmeshes : public IWalkmeshes, boost::noncopyable {
};

class MockWindow : public IWindow, boost::noncopyable {
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
