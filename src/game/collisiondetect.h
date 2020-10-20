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

#include "glm/vec3.hpp"

namespace reone {

namespace game {

enum RaycastFlag {
    kRaycastRooms = 1,
    kRaycastObjects = 2,
    kRaycastWalkable = 4,
    kRaycastAABB = 8
};

class Area;
class Room;
class SpatialObject;

struct RaycastProperties {
    int flags { 0 };
    glm::vec3 origin { 0.0f };
    glm::vec3 direction { 0.0f };
    const SpatialObject *except { nullptr };
};

struct RaycastResult {
    Room *room { nullptr };
    SpatialObject *object { nullptr };
    glm::vec3 intersection { 0.0f };
    float distance { 0.0f };
};

class CollisionDetector {
public:
    CollisionDetector(Area *area);

    /**
     * @return `true` if ray intersects an obstacle, `false` otherwise.
     */
    bool raycast(const RaycastProperties &props, RaycastResult &result) const;

private:
    Area *_area { nullptr };

    CollisionDetector(const CollisionDetector &) = delete;
    CollisionDetector &operator=(const CollisionDetector &) = delete;

    bool rayTestObjects(const RaycastProperties &props, RaycastResult &result) const;
    bool rayTestRooms(const RaycastProperties &props, RaycastResult &result) const;
};

} // namespace game

} // namespace reone
