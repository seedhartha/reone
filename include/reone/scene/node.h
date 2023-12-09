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

#include "reone/graphics/aabb.h"

#include "types.h"

namespace reone {

namespace graphics {

struct GraphicsServices;

} // namespace graphics

namespace audio {

struct AudioServices;

}

namespace resource {

struct ResourceServices;

}

namespace scene {

class IRenderPass;
class IUser;
class SceneGraph;

class SceneNode : boost::noncopyable {
public:
    void addChild(SceneNode &node);
    void removeChild(SceneNode &node);
    void removeAllChildren();

    virtual void update(float dt);

    virtual void renderLeafs(IRenderPass &pass, const std::vector<SceneNode *> &leafs) {
    }

    bool isEnabled() const { return _enabled; }
    bool isCullable() const { return _cullable; }
    bool isCulled() const { return _culled; }
    bool isPoint() const { return _point; }

    glm::vec3 getOrigin() const;
    glm::vec2 getOrigin2D() const;

    float getDistanceTo(const glm::vec3 &point) const;
    float getDistanceTo(const SceneNode &other) const;
    float getSquareDistanceTo(const glm::vec3 &point) const;
    float getSquareDistanceTo(const SceneNode &other) const;
    float getSquareDistanceTo2D(const glm::vec2 &point) const;

    glm::vec3 getWorldCenterOfAABB() const;

    SceneNodeType type() const { return _type; }
    SceneNode *parent() { return _parent; }
    const SceneNode *parent() const { return _parent; }
    const std::unordered_set<SceneNode *> &children() const { return _children; }
    const graphics::AABB &aabb() const { return _aabb; }
    IUser *user() { return _user; }
    const IUser *user() const { return _user; }

    void setUser(IUser &user) { _user = &user; }

    // Flags

    void setEnabled(bool enabled) { _enabled = enabled; }
    void setCullable(bool cullable) { _cullable = cullable; }
    void setCulled(bool culled) { _culled = culled; }

    // END Flags

    // Transformations

    const glm::mat4 &localTransform() const { return _localTransform; }
    const glm::mat4 &absoluteTransform() const { return _absTransform; }
    const glm::mat4 &absoluteTransformInverse() const { return _absTransformInv; }

    void setLocalTransform(glm::mat4 transform);

    // END Transformations

protected:
    SceneNodeType _type;
    SceneGraph &_sceneGraph;
    graphics::GraphicsServices &_graphicsSvc;
    audio::AudioServices &_audioSvc;
    resource::ResourceServices &_resourceSvc;

    SceneNode *_parent {nullptr};
    std::unordered_set<SceneNode *> _children;

    graphics::AABB _aabb;

    IUser *_user {nullptr};

    // Flags

    bool _enabled {true};
    bool _cullable {false}; /**< can this node be frustum- or distance-culled? */
    bool _culled {false};   /**< has this node been frustum- or distance-culled? */
    bool _point {true};     /**< is this node represented by a single point?  */

    // END Flags

    // Transformations

    glm::mat4 _localTransform {1.0f};
    glm::mat4 _absTransform {1.0f};
    glm::mat4 _absTransformInv {1.0f};

    // END Transformations

    SceneNode(
        SceneNodeType type,
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        _type(type),
        _sceneGraph(sceneGraph),
        _graphicsSvc(graphicsSvc),
        _audioSvc(audioSvc),
        _resourceSvc(resourceSvc) {
    }

    void computeAbsoluteTransforms();

    virtual void onAbsoluteTransformChanged() {}
};

} // namespace scene

} // namespace reone
