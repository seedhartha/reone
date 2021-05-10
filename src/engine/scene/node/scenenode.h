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

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

#include "glm/mat4x4.hpp"

#include "../../graphics/aabb.h"

#include "../types.h"

namespace reone {

namespace scene {

constexpr float kDefaultDrawDistance = 1024.0f;

class SceneGraph;

class SceneNode : boost::noncopyable {
public:
    void addChild(const std::shared_ptr<SceneNode> &node);
    void removeChild(SceneNode &node);

    virtual void update(float dt);

    virtual void draw();

    bool isVisible() const { return _visible; }
    virtual bool isTransparent() const { return _transparent; }
    bool isVolumetric() const { return _volumetric; }
    bool isCullable() const { return _cullable; }
    bool isCulled() const { return _culled; }

    virtual glm::vec3 getOrigin() const;

    /**
     * @return distance from the origin of this node to the point
     */
    float getDistanceTo(const glm::vec3 &point) const;

    /**
     * @return squared distance from the origin of this node to the point
     */
    float getDistanceTo2(const glm::vec3 &point) const;

    /**
     * @return shortest distance from the origin of this node to the other node
     */
    float getDistanceTo(const SceneNode &other) const;

    /**
     * @return shortest distance (squared) from the origin of this node to the other node
     */
    float getDistanceTo2(const SceneNode &other) const;

    SceneNodeType type() const { return _type; }
    const SceneNode *parent() const { return _parent; }
    const glm::mat4 &localTransform() const { return _localTransform; }
    const glm::mat4 &absoluteTransform() const { return _absoluteTransform; }
    const glm::mat4 &absoluteTransformInverse() const { return _absoluteTransformInv; }
    const graphics::AABB &aabb() const { return _aabb; }
    const std::vector<std::shared_ptr<SceneNode>> &children() const { return _children; }
    float drawDistance() const { return _drawDistance; }

    void setParent(const SceneNode *parent);
    virtual void setLocalTransform(const glm::mat4 &transform);
    void setPosition(glm::vec3 position);
    virtual void setVisible(bool visible);
    void setTransparent(bool transparent);
    void setDrawDistance(float distance) { _drawDistance = distance; }
    void setCullable(bool cullable) { _cullable = cullable; }
    void setCulled(bool culled) { _culled = culled; }

protected:
    SceneNodeType _type;
    SceneGraph *_sceneGraph;

    graphics::AABB _aabb;
    float _drawDistance { kDefaultDrawDistance };

    bool _visible { true };
    bool _transparent { false };
    bool _volumetric { false }; /**< does this model have a bounding box, or is it a point? */
    bool _cullable { false }; /**< can this model be frustum- or distance-culled? */
    bool _culled { false }; /**< has this model been frustum- or distance-culled? */

    // Relations

    const SceneNode *_parent { nullptr };
    std::vector<std::shared_ptr<SceneNode>> _children;

    // END Relations

    // Transformation matrices

    glm::mat4 _localTransform { 1.0f };
    glm::mat4 _absoluteTransform { 1.0f };
    glm::mat4 _absoluteTransformInv { 1.0f };

    // END Transformation matrices

    SceneNode(SceneNodeType type, SceneGraph *sceneGraph);

    virtual void updateAbsoluteTransform();
};

} // namespace scene

} // namespace reone
