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

#include "scenenode.h"

#include "../../graphics/walkmesh.h"

namespace reone {

namespace scene {

class WalkmeshSceneNode : public SceneNode {
public:
    WalkmeshSceneNode(
        std::shared_ptr<graphics::Walkmesh> walkmesh,
        SceneGraph &sceneGraph,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders) :
        SceneNode(
            SceneNodeType::Walkmesh,
            sceneGraph,
            context,
            meshes,
            shaders),
        _walkmesh(std::move(walkmesh)) {

        _point = false;
        computeAABB();
    }

    const graphics::Walkmesh &walkmesh() const { return *_walkmesh; }

private:
    std::shared_ptr<graphics::Walkmesh> _walkmesh;

    void computeAABB();
};

} // namespace scene

} // namespace reone
