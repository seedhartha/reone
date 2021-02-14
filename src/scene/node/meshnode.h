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

#include "../../render/mesh.h"

namespace reone {

namespace scene {

class MeshSceneNode : public SceneNode {
public:
    MeshSceneNode(SceneGraph *sceneGraph, const std::shared_ptr<render::Mesh> &mesh);

    void render() const override;

    void setColor(glm::vec3 color);
    void setAlpha(float alpha);

private:
    std::shared_ptr<render::Mesh> _mesh;

    glm::vec3 _color { 1.0f };
    float _alpha { 1.0f };
};

} // namespace scene

} // namespace reone
