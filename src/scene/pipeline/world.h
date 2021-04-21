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

#include <boost/noncopyable.hpp>

#include "glm/mat4x4.hpp"

#include "../../render/framebuffer.h"
#include "../../render/renderbuffer.h"
#include "../../render/texture.h"
#include "../../render/types.h"

#include "../scenegraph.h"

namespace reone {

namespace scene {

class WorldRenderPipeline : boost::noncopyable {
public:
    WorldRenderPipeline(SceneGraph *scene, const render::GraphicsOptions &opts);

    void init();
    void render();

private:
    SceneGraph *_scene { nullptr };
    render::GraphicsOptions _opts;
    glm::mat4 _lightSpaceMatrices[render::kNumCubeFaces];

    // Framebuffers

    render::Framebuffer _geometry;
    render::Framebuffer _verticalBlur;
    render::Framebuffer _horizontalBlur;
    render::Framebuffer _shadows;

    // END Framebuffers

    // Framebuffer targets

    std::unique_ptr<render::Renderbuffer> _depthRenderbuffer;
    std::unique_ptr<render::Texture> _geometryColor1;
    std::unique_ptr<render::Texture> _geometryColor2;
    std::unique_ptr<render::Texture> _verticalBlurColor;
    std::unique_ptr<render::Texture> _horizontalBlurColor;
    std::unique_ptr<render::Texture> _shadowsDepth;
    std::unique_ptr<render::Texture> _cubeShadowsDepth;

    // END Framebuffers targets

    void computeLightSpaceMatrices();
    void drawShadows();
    void drawGeometry();
    void applyHorizontalBlur();
    void applyVerticalBlur();
    void drawResult();
};

} // namespace scene

} // namespace reone
