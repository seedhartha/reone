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
#include <set>

#include <boost/noncopyable.hpp>

#include "glm/vec3.hpp"

#include "object/types.h"

namespace reone {

namespace game {

static constexpr float kDefaultRaycastDistance = 8.0f;

class Area;
class Room;
class SpatialObject;

struct RaycastFlags {
    static constexpr int rooms = 1;
    static constexpr int objects = 2;
    static constexpr int walkable = 4;
    static constexpr int aabb = 8;
    static constexpr int selectable = 0x10;
    static constexpr int alive = 0x20;

    static constexpr int roomsWalkable = rooms | walkable;
    static constexpr int roomsObjects = rooms | objects;
    static constexpr int roomsObjectsAABB = roomsObjects | aabb;
    static constexpr int objectsAABBSelectable = objects | aabb | selectable;
    static constexpr int objectsAABBAlive = objects | aabb | alive;
};

struct RaycastProperties {
    int flags { 0 };
    glm::vec3 origin { 0.0f };
    glm::vec3 direction { 0.0f };
    std::set<ObjectType> objectTypes;
    const SpatialObject *except { nullptr };
    float maxDistance { kDefaultRaycastDistance };
};

struct RaycastResult {
    Room *room { nullptr };
    std::shared_ptr<SpatialObject> object;
    glm::vec3 intersection { 0.0f };
    float distance { 0.0f };
};

class CollisionDetector : boost::noncopyable {
public:
    CollisionDetector(Area *area);

    /**
     * @return true if ray intersects an obstacle, false otherwise
     */
    bool raycast(const RaycastProperties &props, RaycastResult &result) const;

private:
    Area *_area;

    bool rayTestObjects(const RaycastProperties &props, RaycastResult &result) const;
    bool rayTestRooms(const RaycastProperties &props, RaycastResult &result) const;
};

} // namespace game

} // namespace reone
