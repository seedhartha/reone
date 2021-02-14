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

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include "../../render/framebuffer.h"
#include "../../render/renderbuffer.h"
#include "../../render/texture.h"

#include "../scenegraph.h"

namespace reone {

namespace scene {

class ControlRenderPipeline : boost::noncopyable {
public:
    ControlRenderPipeline(SceneGraph *scene, const glm::ivec4 &extent);

    void init();
    void render(const glm::ivec2 &offset);

private:
    SceneGraph *_scene;
    glm::vec4 _extent;

    render::Framebuffer _geometry;
    std::unique_ptr<render::Texture> _geometryColor;
    std::unique_ptr<render::Renderbuffer> _geometryDepth;
};

} // namespace scene

} // namespace reone
