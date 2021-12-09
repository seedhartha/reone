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

class SceneGraphs;

class ControlRenderPipeline : boost::noncopyable {
public:
    ControlRenderPipeline(
        SceneGraphs &sceneGraphs,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders) :
        _sceneGraphs(sceneGraphs),
        _context(context),
        _meshes(meshes),
        _shaders(shaders) {
    }

    void init();
    void prepareFor(const glm::ivec4 &extent);

    void render(const std::string &sceneName, const glm::ivec4 &extent, const glm::ivec2 &offset);

private:
    struct AttachmentsId {
        glm::ivec4 extent;

        bool operator==(const AttachmentsId &other) const {
            return extent == other.extent;
        }
    };

    struct AttachmentsIdHasher {
        size_t operator()(const AttachmentsId &id) const {
            std::hash<int> intHash;
            return intHash(id.extent[0]) ^
                   intHash(id.extent[1]) ^
                   intHash(id.extent[2]) ^
                   intHash(id.extent[3]);
        }
    };

    struct Attachments {
        std::shared_ptr<graphics::Texture> colorBuffer;
        std::shared_ptr<graphics::Renderbuffer> depthBuffer;
    };

    std::shared_ptr<graphics::Framebuffer> _geometry;
    std::unordered_map<AttachmentsId, Attachments, AttachmentsIdHasher> _attachments;

    // Services

    SceneGraphs &_sceneGraphs;

    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;

    // END Services
};

} // namespace scene

} // namespace reone
