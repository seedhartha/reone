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

#include "types.h"

namespace reone {

namespace game {

class Area;
class Room;
class SpatialObject;

struct RaycastFlags {
    static constexpr int aabb = 1; /**< use AABB of objects, not walkmeshes */
    static constexpr int rooms = 2; /**< test room walkmeshes */
    static constexpr int walkable = 4; /**< test walkable faces of the walkmesh */
    static constexpr int selectable = 8; /**< test only selectable objects */
    static constexpr int alive = 0x10; /**< test only objects that are alive  */
};

struct RaycastProperties {
    int flags { 0 };
    glm::vec3 origin { 0.0f };
    glm::vec3 direction { 0.0f };
    std::set<ObjectType> objectTypes;
    const SpatialObject *except { nullptr };
    float distance { kDefaultRaycastDistance };
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
     * Casts a ray and tests if it intersects an obstacle.
     *
     * @return true if the ray intersects an obstacle, false otherwise
     */
    bool raycast(const RaycastProperties &props, RaycastResult &result) const;

private:
    Area *_area;

    bool rayTestRooms(const RaycastProperties &props, RaycastResult &result) const;
    bool rayTestObjects(const RaycastProperties &props, RaycastResult &result) const;
};

} // namespace game

} // namespace reone
