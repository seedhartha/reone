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

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../render/aabb.h"

namespace reone {

namespace scene {

class CameraSceneNode;
class OctreeNode;
class SceneNode;

struct OctreeObject {
    SceneNode *sceneNode { nullptr };
    render::AABB aabb;
    std::unordered_set<OctreeNode *> nodes;
};

class OctreeNode {
public:
    OctreeNode(OctreeNode *parent, render::AABB aabb);

    void add(OctreeObject *object);

private:
    enum class Quadrant {
        BottomLeft,
        BottomRight,
        TopLeft,
        TopRight
    };

    OctreeNode *_parent { nullptr };

    render::AABB _aabb;
    render::AABB _bottomLeftAabb;
    render::AABB _bottomRightAabb;
    render::AABB _topLeftAabb;
    render::AABB _topRightAabb;

    std::unique_ptr<OctreeNode> _bottomLeft;
    std::unique_ptr<OctreeNode> _bottomRight;
    std::unique_ptr<OctreeNode> _topLeft;
    std::unique_ptr<OctreeNode> _topRight;

    std::unordered_set<OctreeObject *> _objects;

    void computeQuadrantsAABB();

    bool isLeaf() const;
    bool isObjectIn(OctreeObject *object, Quadrant quadrant) const;

    friend class Octree;
};

/**
 * This is actually a quadtree. Used to optimize rendering and ray casting.
 */
class Octree {
public:
    void clear();
    void registerObject(OctreeObject object);
    void build();

    std::vector<SceneNode *> getNodesInFrustum(const CameraSceneNode *camera) const;

private:
    std::unordered_map<SceneNode *, OctreeObject> _objects;
    std::unique_ptr<OctreeNode> _root;
};

} // namespace scene

} // namespace reone
