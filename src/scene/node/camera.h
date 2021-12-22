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

#include "../../graphics/aabb.h"

#include "../node.h"

namespace reone {

namespace scene {

class CameraSceneNode : public SceneNode {
public:
    CameraSceneNode(
        float fieldOfView,
        float aspect,
        float nearPlane,
        float farPlane,
        SceneGraph &sceneGraph) :
        SceneNode(SceneNodeType::Camera, sceneGraph),
        _fieldOfView(fieldOfView),
        _aspect(aspect),
        _nearPlane(nearPlane),
        _farPlane(farPlane) {

        setProjection(fieldOfView, aspect, nearPlane, farPlane);
    }

    // TODO: forbid querying FOV and aspect of orthogonal projection cameras
    CameraSceneNode(
        glm::mat4 projection,
        SceneGraph &sceneGraph) :
        SceneNode(SceneNodeType::Camera, sceneGraph),
        _fieldOfView(-1.0f),
        _aspect(-1.0f),
        _nearPlane(-1.0f),
        _farPlane(-1.0f),
        _projection(std::move(projection)) {
    }

    bool isInFrustum(const glm::vec3 &point) const;
    bool isInFrustum(const graphics::AABB &aabb) const;
    bool isInFrustum(const SceneNode &other) const;

    float fieldOfView() const { return _fieldOfView; }
    float nearPlane() const { return _nearPlane; }
    float farPlane() const { return _farPlane; }
    const glm::mat4 &projection() const { return _projection; }
    const glm::mat4 &view() const { return _view; }

    void setProjection(float fieldOfView, float aspect, float nearPlane, float farPlane);

private:
    float _fieldOfView;
    float _aspect;
    float _nearPlane;
    float _farPlane;

    glm::mat4 _projection {1.0f};
    glm::mat4 _view {1.0f};

    // Frustum planes

    glm::vec4 _frustumLeft {0.0f};
    glm::vec4 _frustumRight {0.0f};
    glm::vec4 _frustumBottom {0.0f};
    glm::vec4 _frustumTop {0.0f};
    glm::vec4 _frustumNear {0.0f};
    glm::vec4 _frustumFar {0.0f};

    // END Frustum planes

    void computeView();
    void computeFrustumPlanes();

    void onAbsoluteTransformChanged() override;
};

} // namespace scene

} // namespace reone
