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

#include "../../graphics/aabb.h"

namespace reone {

namespace scene {

const int kNumFrustumPlanes = 6;

class CameraSceneNode : public SceneNode {
public:
    CameraSceneNode(SceneGraph *sceneGraph, glm::mat4 projection, float aspect, float nearPlane, float farPlane);

    bool isInFrustum(const glm::vec3 &point) const;
    bool isInFrustum(const graphics::AABB &aabb) const;

    const glm::mat4 &projection() const { return _projection; }
    const glm::mat4 &view() const { return _view; }
    float aspect() const { return _aspect; }
    float nearPlane() const { return _nearPlane; }
    float farPlane() const { return _farPlane; }

    void setProjection(const glm::mat4 &projection);

private:
    glm::mat4 _projection { 1.0f };
    glm::mat4 _view { 1.0f };
    glm::vec4 _frustumPlanes[kNumFrustumPlanes];
    float _aspect { 1.0f };
    float _nearPlane { 0.0f };
    float _farPlane { 0.0f };

    void updateAbsoluteTransform() override;

    void updateView();
    void updateFrustum();
};

} // namespace scene

} // namespace reone
