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
#include "../../graphics/renderbuffer.h"

namespace reone {

namespace graphics {

class Texture;

}

namespace di {

class GraphicsServices;

}

namespace scene {

class SceneGraph;

class ControlRenderPipeline : boost::noncopyable {
public:
    ControlRenderPipeline(glm::ivec4 extent, di::GraphicsServices &graphics, SceneGraph &scene);

    void init();
    void render(const glm::ivec2 &offset);

private:
    glm::vec4 _extent;
    SceneGraph &_sceneGraph;
    di::GraphicsServices &_graphics;

    graphics::Framebuffer _geometry;
    std::unique_ptr<graphics::Texture> _geometryColor;
    std::unique_ptr<graphics::Renderbuffer> _geometryDepth;
};

} // namespace scene

} // namespace reone
