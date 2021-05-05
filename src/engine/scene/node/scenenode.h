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

class SceneGraph;

class SceneNode : boost::noncopyable {
public:
    void addChild(const std::shared_ptr<SceneNode> &node);
    void removeChild(SceneNode &node);

    virtual void update(float dt);

    virtual void draw();

    bool isVisible() const { return _visible; }
    virtual bool isTransparent() const { return _transparent; }

    /**
     * @return true if this scene node has a bounding box, false if it is a point
     */
    bool isVolumetric() const { return _volumetric; }

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

    void setParent(const SceneNode *parent);
    virtual void setLocalTransform(const glm::mat4 &transform);
    void setPosition(glm::vec3 position);
    virtual void setVisible(bool visible);
    void setTransparent(bool transparent);

protected:
    SceneNodeType _type;
    SceneGraph *_sceneGraph;

    const SceneNode *_parent { nullptr };
    std::vector<std::shared_ptr<SceneNode>> _children;

    glm::mat4 _localTransform { 1.0f };
    glm::mat4 _absoluteTransform { 1.0f };
    glm::mat4 _absoluteTransformInv { 1.0f };
    graphics::AABB _aabb;

    bool _visible { true };
    bool _transparent { false };
    bool _volumetric { false };

    SceneNode(SceneNodeType type, SceneGraph *sceneGraph);

    virtual void updateAbsoluteTransform();
};

} // namespace scene

} // namespace reone
