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

#pragma once

#include "../framebuffer.h"
#include "../options.h"
#include "../renderbuffer.h"
#include "../texture.h"
#include "../textures.h"
#include "../types.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class IScene;
class Meshes;
class Shaders;
class Textures;

class WorldPipeline : boost::noncopyable {
public:
    WorldPipeline(
        GraphicsOptions options,
        GraphicsContext &graphicsContext,
        Meshes &meshes,
        Shaders &shaders,
        Textures &textures) :
        _options(std::move(options)),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures) {
    }

    void init();

    void draw();

    std::shared_ptr<Texture> screenshot() { return _cbScreenshot; }

    void setScene(IScene &scene) { _scene = &scene; }
    void setTakeScreenshot(bool take) { _takeScreenshot = take; }

private:
    GraphicsOptions _options;

    IScene *_scene {nullptr};
    bool _takeScreenshot {false}; /**< render next frame into texture */

    glm::mat4 _shadowLightSpace[kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 _shadowCascadeFarPlanes[2] {glm::vec4(0.0f)};

    // Services

    GraphicsContext &_graphicsContext;
    Meshes &_meshes;
    Shaders &_shaders;
    Textures &_textures;

    // END Services

    // Framebuffers

    std::shared_ptr<Framebuffer> _fbGeometryMS;
    std::shared_ptr<Framebuffer> _fbGeometry;
    std::shared_ptr<Framebuffer> _fbVerticalBlur;
    std::shared_ptr<Framebuffer> _fbHorizontalBlur;
    std::shared_ptr<Framebuffer> _fbPointLightShadows;
    std::shared_ptr<Framebuffer> _fbDirectionalLightShadows;
    std::shared_ptr<Framebuffer> _fbScreenshot;

    // END Framebuffers

    // Color Buffers

    std::shared_ptr<Texture> _cbGeometry1MS;
    std::shared_ptr<Texture> _cbGeometry2MS;
    std::shared_ptr<Texture> _cbGeometry1;
    std::shared_ptr<Texture> _cbGeometry2;
    std::shared_ptr<Texture> _cbVerticalBlur;
    std::shared_ptr<Texture> _cbHorizontalBlur;
    std::shared_ptr<Texture> _cbScreenshot;

    // END Color Buffers

    // Depth Buffers

    std::shared_ptr<Renderbuffer> _dbCommonMS;
    std::shared_ptr<Renderbuffer> _dbCommon;
    std::shared_ptr<Texture> _dbDirectionalLightShadows;
    std::shared_ptr<Texture> _dbPointLightShadows;

    // END Depth Buffers

    void computeLightSpaceMatrices();
    void drawShadows();
    void drawGeometry();
    void drawHorizontalBlur();
    void drawVerticalBlur();
    void presentWorld();
};

} // namespace graphics

} // namespace reone
