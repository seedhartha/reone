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

class Context;
class Meshes;
class Shaders;
class Texture;

} // namespace graphics

namespace scene {

class SceneGraph;

class ControlRenderPipeline : boost::noncopyable {
public:
    ControlRenderPipeline(
        glm::ivec4 extent,
        SceneGraph &sceneGraph,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders) :
        _extent(std::move(extent)),
        _sceneGraph(sceneGraph),
        _context(context),
        _meshes(meshes),
        _shaders(shaders) {
    }

    void init();
    void render(const glm::ivec2 &offset);

private:
    glm::vec4 _extent;

    graphics::Framebuffer _geometry;
    std::unique_ptr<graphics::Texture> _geometryColor;
    std::unique_ptr<graphics::Renderbuffer> _geometryDepth;

    // Services

    SceneGraph &_sceneGraph;

    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;

    // END Services
};

} // namespace scene

} // namespace reone
