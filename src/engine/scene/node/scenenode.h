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

#include "../types.h"

namespace reone {

namespace graphics {

class Context;
class Meshes;
class Shaders;

} // namespace graphics

namespace scene {

class IUser;
class SceneGraph;

class SceneNode : boost::noncopyable {
public:
    void addChild(std::shared_ptr<SceneNode> node);
    void removeChild(const std::shared_ptr<SceneNode> &node);
    void removeAllChildren();

    virtual void update(float dt);
    virtual void draw();
    virtual void drawElements(const std::vector<SceneNode *> &elements, int count = -1) {}

    bool isVisible() const { return _visible; }
    bool isCullable() const { return _cullable; }
    bool isCulled() const { return _culled; }
    bool isPoint() const { return _point; }

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

    SceneNodeType type() const { return _type; }
    SceneNode *parent() { return _parent; }
    const SceneNode *parent() const { return _parent; }
    const std::unordered_set<std::shared_ptr<SceneNode>> &children() const { return _children; }
    const graphics::AABB &aabb() const { return _aabb; }
    IUser *user() { return _user; }
    const IUser *user() const { return _user; }

    void setUser(IUser &user) { _user = &user; }
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
    SceneNodeType _type;

    SceneNode *_parent {nullptr};
    std::unordered_set<std::shared_ptr<SceneNode>> _children;

    graphics::AABB _aabb;

    IUser *_user {nullptr};

    // Services

    SceneGraph &_sceneGraph;

    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;

    // END Services

    // Transformations

    glm::mat4 _localTransform {1.0f};
    glm::mat4 _absTransform {1.0f};
    glm::mat4 _absTransformInv {1.0f};

    // END Transformations

    // Flags

    bool _visible {true};
    bool _cullable {false}; /**< can this scene node be frustum- or distance-culled? */
    bool _culled {false};   /**< has this scene node been frustum- or distance-culled? */
    bool _point {true};

    // END Flags

    SceneNode(
        SceneNodeType type,
        SceneGraph &sceneGraph,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders) :
        _type(type),
        _sceneGraph(sceneGraph),
        _context(context),
        _meshes(meshes),
        _shaders(shaders) {
    }

    void computeAbsoluteTransforms();

    virtual void onAbsoluteTransformChanged() {}
};

} // namespace scene

} // namespace reone
