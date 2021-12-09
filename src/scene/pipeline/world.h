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

#include "../../graphics/framebuffer.h"
#include "../../graphics/options.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/texture.h"
#include "../../graphics/types.h"

namespace reone {

namespace graphics {

class Context;
class Meshes;
class Shaders;

} // namespace graphics

namespace scene {

class SceneGraph;

class WorldRenderPipeline : boost::noncopyable {
public:
    WorldRenderPipeline(
        graphics::GraphicsOptions options,
        SceneGraph &sceneGraph,
        graphics::Context &contxt,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders);

    void init();
    void render();

    std::shared_ptr<graphics::Texture> screenshot() { return _screenshotColor; }

    void setTakeScreenshot(bool take) { _takeScreenshot = take; }

private:
    graphics::GraphicsOptions _options;

    glm::mat4 _lightSpaceMatrices[graphics::kNumCubeFaces];
    bool _takeScreenshot {false}; /**< render next frame into texture */

    // Services

    SceneGraph &_sceneGraph;

    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;

    // END Services

    // Framebuffers

    std::shared_ptr<graphics::Framebuffer> _geometry;
    std::shared_ptr<graphics::Framebuffer> _verticalBlur;
    std::shared_ptr<graphics::Framebuffer> _horizontalBlur;
    std::shared_ptr<graphics::Framebuffer> _shadows;
    std::shared_ptr<graphics::Framebuffer> _screenshot;

    // END Framebuffers

    // Framebuffer targets

    std::shared_ptr<graphics::Renderbuffer> _depthRenderbuffer;
    std::shared_ptr<graphics::Texture> _geometryColor1;
    std::shared_ptr<graphics::Texture> _geometryColor2;
    std::shared_ptr<graphics::Texture> _verticalBlurColor;
    std::shared_ptr<graphics::Texture> _horizontalBlurColor;
    std::shared_ptr<graphics::Texture> _shadowsDepth;
    std::shared_ptr<graphics::Texture> _cubeShadowsDepth;
    std::shared_ptr<graphics::Texture> _screenshotColor;

    // END Framebuffers targets

    void computeLightSpaceMatrices();
    void drawShadows();
    void drawGeometry();
    void applyHorizontalBlur();
    void applyVerticalBlur();
    void drawResult();
    void saveScreenshot();
};

} // namespace scene

} // namespace reone
