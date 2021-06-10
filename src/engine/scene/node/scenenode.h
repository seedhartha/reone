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

#include "../nodeelement.h"
#include "../types.h"

namespace reone {

namespace scene {

class SceneGraph;

class SceneNode : boost::noncopyable {
public:
    void addChild(std::shared_ptr<SceneNode> node);
    void removeChild(SceneNode &node);

    virtual void update(float dt);
    virtual void draw();

    virtual void drawElements(const std::vector<std::shared_ptr<SceneNodeElement>> &elements, int count = -1) {}

    bool isVisible() const { return _visible; }
    bool isCullable() const { return _cullable; }
    bool isCulled() const { return _culled; }
    bool isVolumetric() const { return _volumetric; }

    glm::vec3 getOrigin() const;

    /**
     * @return distance from the origin of this node to the specified point
     */
    float getDistanceTo(const glm::vec3 &point) const;

    /**
     * @return squared distance from the origin of this node to the specified point
     */
    float getDistanceTo2(const glm::vec3 &point) const;

    /**
     * @return distance between origins of this and the specified node
     */
    float getDistanceTo(const SceneNode &other) const;

    /**
     * @return squared distance between origins of this and the specified node
     */
    float getDistanceTo2(const SceneNode &other) const;

    glm::vec3 getWorldCenterOfAABB() const;

    const std::string &name() const { return _name; }
    SceneNodeType type() const { return _type; }
    const SceneNode *parent() const { return _parent; }
    const std::vector<std::shared_ptr<SceneNode>> &children() const { return _children; }
    const graphics::AABB &aabb() const { return _aabb; }

    void setVisible(bool visible) { _visible = visible; }
    void setCullable(bool cullable) { _cullable = cullable; }
    void setCulled(bool culled) { _culled = culled; }

    // Transformations

    const glm::mat4 &localTransform() const { return _localTransform; }
    const glm::mat4 &absoluteTransform() const { return _absTransform; }
    const glm::mat4 &absoluteTransformInverse() const { return _absTransformInv; }

    void setLocalTransform(glm::mat4 transform);

    // END Transformations

protected:
    std::string _name;
    SceneNodeType _type;
    SceneGraph *_sceneGraph;

    graphics::AABB _aabb;
    const SceneNode *_parent { nullptr };
    std::vector<std::shared_ptr<SceneNode>> _children;

    // Transformations

    glm::mat4 _localTransform { 1.0f };
    glm::mat4 _absTransform { 1.0f };
    glm::mat4 _absTransformInv { 1.0f };

    // END Transformations

    // Flags

    bool _visible { true };
    bool _cullable { false }; /**< can this scene node be frustum- or distance-culled? */
    bool _culled { false }; /**< has this scene node been frustum- or distance-culled? */
    bool _volumetric { false }; /**< does this scene node have a bounding box? */

    // END Flags

    SceneNode(std::string name, SceneNodeType type, SceneGraph *sceneGraph);

    void computeAbsoluteTransforms();

    virtual void onAbsoluteTransformChanged() { }
};

} // namespace scene

} // namespace reone
