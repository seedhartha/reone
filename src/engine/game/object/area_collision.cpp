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

#include "area.h"

#include "../game.h"
#include "../room.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kElevationTestZ = 1024.0f;
static constexpr float kMaxCollisionDistance = 8.0f;
static constexpr float kMaxCollisionDistance2 = kMaxCollisionDistance * kMaxCollisionDistance;
static constexpr float kLineOfSightTestHeight = 1.7f; // TODO: make it appearance-based

bool Area::testElevationAt(const glm::vec2 &point, float &z, int &material, Room *&room) const {
    static glm::vec3 down(0.0f, 0.0f, -1.0f);

    // Test non-walkable faces of object walkmeshes
    for (auto &o : _objects) {
        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        shared_ptr<Walkmesh> walkmesh(o->getWalkmesh());
        if (!model || !walkmesh) continue;

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(point) > kMaxCollisionDistance2) continue;

        // Test non-walkable faces beneath the specified point (object space)
        glm::vec2 objSpacePos(model->absoluteTransformInverse() * glm::vec4(point, 0.0f, 1.0f));
        float distance;
        glm::vec3 normal;
        if (walkmesh->raycastNonWalkableFirst(glm::vec3(objSpacePos, kElevationTestZ), down, distance, normal)) return false;
    }

    // Test walkable faces of room walkmeshes
    for (auto &r : _rooms) {
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<Walkmesh> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) continue;

        // Point must be inside room AABB in 2D object space
        glm::vec2 roomSpacePos(model->absoluteTransformInverse() * glm::vec4(point, 0.0f, 1.0f));
        if (!model->aabb().contains(roomSpacePos)) continue;

        // Test walkable faces beneath the specified point (world space)
        float distance;
        int tempMaterial;
        if (walkmesh->raycastWalkableFirst(glm::vec3(point, kElevationTestZ), down, distance, tempMaterial)) {
            z = kElevationTestZ - distance;
            material = tempMaterial;
            room = r.second.get();
            return true;
        }
    }

    return false;
}

shared_ptr<SpatialObject> Area::getObjectAt(int x, int y) const {
    shared_ptr<CameraSceneNode> camera(_game->services().scene().graph().activeCamera());
    shared_ptr<Creature> partyLeader(_game->services().party().getLeader());
    if (!camera || !partyLeader) return nullptr;

    const GraphicsOptions &opts = _game->options().graphics;
    glm::vec4 viewport(0.0f, 0.0f, opts.width, opts.height);
    glm::vec3 start(glm::unProject(glm::vec3(x, opts.height - y, 0.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 end(glm::unProject(glm::vec3(x, opts.height - y, 1.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 dir(glm::normalize(end - start));

    // Calculate distances to all selectable objects, return the closest object
    vector<pair<shared_ptr<SpatialObject>, float>> distances;
    for (auto &o : _objects) {
        // Skip non-selectable objects and party leader
        if (!o->isSelectable() || o == partyLeader) continue;

        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        if (!model) continue;

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(start) > kMaxCollisionDistance2) continue;

        // Test object AABB (object space)
        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, distance)) {
            distances.push_back(make_pair(o, distance));
        }
    }
    if (distances.empty()) return nullptr;
    sort(distances.begin(), distances.end(), [](auto &left, auto &right) { return left.second < right.second; });

    return distances[0].first;
}

bool Area::getCameraObstacle(const glm::vec3 &start, const glm::vec3 &end, glm::vec3 &intersection) const {
    glm::vec3 endToStart(end - start);
    glm::vec3 dir(glm::normalize(endToStart));
    float minDistance = numeric_limits<float>::max();
    float maxDistance = glm::length(endToStart);

    // Test AABB of door objects
    for (auto &o : _objects) {
        if (o->type() != ObjectType::Door) continue;

        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        if (!model) continue;

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(start) > kMaxCollisionDistance2) continue;

        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, distance) && distance < minDistance && distance < maxDistance) {
            minDistance = distance;
        }
    }

    // Test non-walkable faces of room walkmeshes
    for (auto &r : _rooms) {
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<Walkmesh> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) continue;

        // Start of path must be inside room AABB
        glm::vec2 roomSpacePos(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        if (!model->aabb().contains(roomSpacePos)) continue;

        float distance;
        glm::vec3 normal;
        if (walkmesh->raycastNonWalkableClosest(start, dir, distance, normal) && distance < minDistance && distance < maxDistance) {
            minDistance = distance;
        }
    }

    if (minDistance != numeric_limits<float>::max()) {
        intersection = start + minDistance * dir;
        return true;
    }

    return false;
}

bool Area::getCreatureObstacle(const glm::vec3 &start, const glm::vec3 &end, glm::vec3 &normal) const {
    glm::vec3 endToStart(end - start);
    glm::vec3 dir(glm::normalize(endToStart));
    float minDistance = numeric_limits<float>::max();
    float maxDistance = glm::length(endToStart);

    // Test non-walkable faces of room walkmeshes
    for (auto &r : _rooms) {
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<Walkmesh> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) continue;

        // Start of path must be inside room AABB
        glm::vec2 roomSpacePos(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        if (!model->aabb().contains(roomSpacePos)) continue;

        float distance;
        glm::vec3 tempNormal;
        if (walkmesh->raycastNonWalkableClosest(start, dir, distance, tempNormal) && distance < minDistance && distance < maxDistance) {
            minDistance = distance;
            normal = tempNormal;
        }
    }

    return minDistance != numeric_limits<float>::max();
}

bool Area::isInLineOfSight(const Creature &subject, const SpatialObject &target) const {
    static glm::vec3 offsetZ { 0.0f, 0.0f, kLineOfSightTestHeight };

    glm::vec3 start(subject.position() + offsetZ);
    glm::vec3 end(target.position() + offsetZ);
    glm::vec3 startToEnd(end - start);
    glm::vec3 dir(glm::normalize(startToEnd));
    float maxDistance = glm::length(startToEnd);

    for (auto &o : _objects) {
        if (o->type() != ObjectType::Door) continue;

        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        if (!model) continue;

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(start) > kMaxCollisionDistance2) continue;

        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, distance) && distance < maxDistance) return false;
    }

    // Test non-walkable faces of room walkmeshes
    for (auto &r : _rooms) {
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<Walkmesh> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) continue;

        // Start or end of path must be inside room AABB
        glm::vec2 roomSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec2 roomSpaceEnd(model->absoluteTransformInverse() * glm::vec4(end, 1.0f));
        if (!model->aabb().contains(roomSpaceStart) && !model->aabb().contains(roomSpaceEnd)) continue;

        float distance;
        glm::vec3 normal;
        if (walkmesh->raycastNonWalkableClosest(start, dir, distance, normal) && distance < maxDistance) return false;
    }

    return true;
}

} // namespace game

} // namespace reone
