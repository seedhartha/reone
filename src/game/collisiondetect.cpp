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

#include "collisiondetect.h"

#include <algorithm>
#include <stdexcept>
#include <map>

#include "object/area.h"
#include "object/door.h"

#include "room.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

CollisionDetector::CollisionDetector(Area *area) : _area(area) {
    if (!area) {
        throw invalid_argument("area must not be null");
    }
}

bool CollisionDetector::raycast(const RaycastProperties &props, RaycastResult &result) const {
    // Only test rooms when rooms flag is set
    if (props.flags & RaycastFlags::rooms) {
        if (rayTestRooms(props, result)) return true;
    }
    return rayTestObjects(props, result);
}

bool CollisionDetector::rayTestRooms(const RaycastProperties &props, RaycastResult &result) const {
    float distance;

    for (auto &room : _area->rooms()) {
        shared_ptr<ModelSceneNode> model(room.second->model());
        if (!model) continue;

        // Optimization: ensure that the ray origin is inside a room in 2D space
        AABB aabb(model->aabb() * model->absoluteTransform());
        if (!aabb.contains(glm::vec2(props.origin))) continue;

        shared_ptr<Walkmesh> walkmesh(room.second->walkmesh());
        if (!walkmesh) continue;

        int material = 0;
        if (walkmesh->raycast(props.origin, props.direction, props.flags & RaycastFlags::walkable, distance, material) &&
            distance <= props.distance) {

            result.room = room.second.get();
            result.intersection = props.origin + distance * props.direction;
            result.distance = distance;
            result.material = material;
            return true;
        }
    }

    return false;
}

bool CollisionDetector::rayTestObjects(const RaycastProperties &props, RaycastResult &result) const {
    float distance;
    vector<pair<shared_ptr<SpatialObject>, float>> collisions;

    for (auto &object : _area->objects()) {
        if (object.get() == props.except) continue; // Object is not be to be tested
        if (props.objectTypes.count(object->type()) == 0) continue; // Objects of this type are not to be tested
        if ((props.flags & RaycastFlags::selectable) && !object->isSelectable()) continue; // Non-selectable objects are not to be tested
        if ((props.flags & RaycastFlags::alive) && object->isDead()) continue; // Dead objects are not to be tested
        if (object->getDistanceTo2(glm::vec2(props.origin)) > props.distance * props.distance) continue; // Optimization: object must not be too far away in 2D space

        // Do testing in object space
        glm::mat4 invObjTransform(glm::inverse(object->transform()));
        glm::vec3 objSpaceOrigin(invObjTransform * glm::vec4(props.origin, 1.0f));
        glm::vec3 objSpaceDir(glm::normalize(invObjTransform * glm::vec4(props.direction, 0.0f)));

        if (props.flags & RaycastFlags::aabb) {
            // Test using AABB
            shared_ptr<ModelSceneNode> model(object->getModelSceneNode());
            if (model) {
                // Prevent division by zero
                if (objSpaceDir.x == 0.0f) objSpaceDir.x = 1e-7f;
                if (objSpaceDir.y == 0.0f) objSpaceDir.y = 1e-7f;
                if (objSpaceDir.z == 0.0f) objSpaceDir.z = 1e-7f;
                glm::vec3 invDir(1.0f / objSpaceDir);

                if (model->aabb().raycast(objSpaceOrigin, invDir, distance) &&
                    distance >= 0.0f && distance <= props.distance) {

                    collisions.push_back(make_pair(object, distance));
                }
            }
        } else {
            // Test using a walkmesh
            shared_ptr<Walkmesh> walkmesh(object->getWalkmesh());
            int material = 0;

            if (walkmesh && walkmesh->raycast(objSpaceOrigin, objSpaceDir, props.flags & RaycastFlags::walkable, distance, material) &&
                distance <= props.distance) {

                collisions.push_back(make_pair(object, distance));
            }
        }
    }

    if (collisions.empty()) return false;

    sort(collisions.begin(), collisions.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    result.object = collisions[0].first;
    result.intersection = props.origin + collisions[0].second * props.direction;
    result.distance = collisions[0].second;

    return true;
}

} // namespace game

} // namespace reone
