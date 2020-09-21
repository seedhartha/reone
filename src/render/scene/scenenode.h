/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "glm/mat4x4.hpp"

namespace reone {

namespace render {

class SceneGraph;

class SceneNode {
public:
    void addChild(const std::shared_ptr<SceneNode> &node);
    virtual void fillSceneGraph();
    virtual void render() const;

    float distanceTo(const glm::vec3 &point) const;

    const SceneNode *parent() const;
    const glm::mat4 &localTransform() const;
    const glm::mat4 &absoluteTransform() const;
    const std::vector<std::shared_ptr<SceneNode>> &children() const;

    void setParent(const SceneNode *parent);
    virtual void setLocalTransform(const glm::mat4 &transform);

protected:
    const SceneNode *_parent { nullptr };
    glm::mat4 _localTransform { 1.0f };
    glm::mat4 _absoluteTransform { 1.0f };

    SceneNode() = default;

    virtual void updateAbsoluteTransform();

private:
    std::vector<std::shared_ptr<SceneNode>> _children;

    SceneNode(const SceneNode &) = delete;
    SceneNode &operator=(const SceneNode &) = delete;
};

} // namespace render

} // namespace reone
