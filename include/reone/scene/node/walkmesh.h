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

#include "../node.h"

#include "reone/graphics/mesh.h"
#include "reone/graphics/walkmesh.h"

namespace reone {

namespace graphics {

struct GraphicsServices;

}

namespace scene {

class WalkmeshSceneNode : public SceneNode {
public:
    WalkmeshSceneNode(
        graphics::Walkmesh &walkmesh,
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        SceneNode(
            SceneNodeType::Walkmesh,
            sceneGraph,
            graphicsSvc,
            audioSvc,
            resourceSvc),
        _walkmesh(walkmesh) {

        _point = false;

        init();
    }

    void init();
    void draw();

    const graphics::Walkmesh &walkmesh() const { return _walkmesh; }

private:
    graphics::Walkmesh &_walkmesh;

    std::shared_ptr<graphics::Mesh> _mesh;
};

} // namespace scene

} // namespace reone
