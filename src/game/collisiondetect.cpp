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

#include "collisiondetect.h"

#include <algorithm>
#include <stdexcept>
#include <map>

#include "object/area.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

CollisionDetector::CollisionDetector(Area *area) : _area(area) {
    if (!area) {
        throw invalid_argument("Area must not be null");
    }
}

bool CollisionDetector::raycast(const RaycastProperties &props, RaycastResult &result) const {
    if (props.flags & kRaycastObjects) {
        if (rayTestObjects(props, result)) {
            return true;
        }
    }
    if (props.flags & kRaycastRooms) {
        if (rayTestRooms(props, result)) {
            return true;
        }
    }

    return false;
}

bool CollisionDetector::rayTestObjects(const RaycastProperties &props, RaycastResult &result) const {
    glm::mat4 invTransform(0.0f);
    glm::vec3 origin(0.0f);
    glm::vec3 dir(0.0f);
    float distance = 0.0f;
    vector<pair<SpatialObject *, float>> collisions;

    for (auto &object : _area->objects()) {
        if (!object->visible() ||
            object.get() == props.except ||
            (object->type() == ObjectType::Door && static_cast<Door &>(*object).isOpen())) {

            continue;
        }
        invTransform = glm::inverse(object->transform());
        origin = invTransform * glm::vec4(props.origin, 1.0f);
        dir = invTransform * glm::vec4(props.direction, 0.0f);

        if (props.flags & kRaycastAABB) {
            shared_ptr<ModelSceneNode> model(object->model());
            if (!model) continue;

            if (model->aabb().intersectLine(origin, dir, distance)) {
                collisions.push_back(make_pair(object.get(), distance));
            }
            continue;
        }
        shared_ptr<Walkmesh> walkmesh(object->walkmesh());
        if (!walkmesh) continue;

        if (walkmesh->raycast(origin, dir, false, distance)) {
            collisions.push_back(make_pair(object.get(), distance));
            continue;
        }
    }
    if (collisions.empty()) return false;

    sort(collisions.begin(), collisions.end(), [](const pair<SpatialObject *, float> &left, const pair<SpatialObject *, float> &right) {
        return left.second < right.second;
    });
    result.object = collisions[0].first;
    result.intersection = props.origin + collisions[0].second * props.direction;
    result.distance = collisions[0].second;

    return true;
}

bool CollisionDetector::rayTestRooms(const RaycastProperties &props, RaycastResult &result) const {
    float distance = 0.0f;

    for (auto &pair : _area->rooms()) {
        Room &room = *pair.second;

        const Walkmesh *walkmesh = room.walkmesh();
        if (!walkmesh) continue;

        if (walkmesh->raycast(props.origin, props.direction, props.flags & kRaycastWalkable, distance)) {
            result.room = &room;
            result.intersection = props.origin + distance * props.direction;
            result.distance = distance;
            return true;
        }
    }

    return false;
}

} // namespace game

} // namespace reone
