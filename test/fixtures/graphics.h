/*
 * Copyright (c) 2020-2022 The reone project contributors
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

class MockFonts : public IFonts {
};

class MockGraphicsContext : public IGraphicsContext {
};

class MockLipAnimations : public ILipAnimations {
};

class MockMeshes : public IMeshes {
};

class MockModels : public IModels {
public:
    std::shared_ptr<Model> get(const std::string &resRef) override {
        return nullptr;
    }
};

class MockPipeline : public IPipeline {
};

class MockShaders : public IShaders {
};

class MockTextures : public ITextures {
public:
    std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage) override {
        return nullptr;
    }
};

class MockUniforms : public IUniforms {
};

class MockWalkmeshes : public IWalkmeshes {
};

class MockWindow : public IWindow {
};

inline std::unique_ptr<GraphicsServices> mockGraphicsServices() {
    // TODO: free automatically
    auto fonts = new MockFonts();
    auto context = new MockGraphicsContext();
    auto lipAnimations = new MockLipAnimations();
    auto meshes = new MockMeshes();
    auto models = new MockModels();
    auto pipeline = new MockPipeline();
    auto shaders = new MockShaders();
    auto textures = new MockTextures();
    auto uniforms = new MockUniforms();
    auto walkmeshes = new MockWalkmeshes();
    auto window = new MockWindow();

    return std::make_unique<GraphicsServices>(
        *fonts,
        *context,
        *lipAnimations,
        *meshes,
        *models,
        *pipeline,
        *shaders,
        *textures,
        *uniforms,
        *walkmeshes,
        *window);
}

} // namespace graphics

} // namespace reone
