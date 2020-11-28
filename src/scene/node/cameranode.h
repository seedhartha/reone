/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../render/aabb.h"

namespace reone {

namespace scene {

const int kFrustumPlaneCount = 6;

class CameraSceneNode : public SceneNode {
public:
    CameraSceneNode(SceneGraph *sceneGraph, const glm::mat4 &projection);

    bool isInFrustum(const glm::vec3 &point) const;
    bool isInFrustum(const render::AABB &aabb) const;

    const glm::mat4 &projection() const;
    const glm::mat4 &view() const;

    void setProjection(const glm::mat4 &projection);

private:
    glm::mat4 _projection { 1.0f };
    glm::mat4 _view { 1.0f };
    glm::vec4 _frustum[kFrustumPlaneCount];

    void updateAbsoluteTransform() override;

    void updateView();
    void updateFrustum();
};

} // namespace scene

} // namespace reone
