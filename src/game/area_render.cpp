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

#include "area.h"

#include "glm/gtx/norm.hpp"

#include "../render/mesh/aabb.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::render;

namespace reone {

namespace game {

void Area::initGL() {
    for (auto &room : _rooms) {
        shared_ptr<ModelInstance> model(room->model());
        if (model) model->initGL();
    }
    for (auto &pair : _objects) {
        for (auto &object : pair.second) {
            object->initGL();
        }
    }
}

static bool sortByDistanceToCamera(const RenderListItem &left, const RenderListItem &right, const glm::vec3 &cameraPosition) {
    return glm::distance2(left.center, cameraPosition) > glm::distance2(right.center, cameraPosition);
}

void Area::fillRenderLists(const glm::vec3 &cameraPosition) {
    auto &opaque = _renderLists[RenderListName::Opaque];
    auto &transparent = _renderLists[RenderListName::Transparent];

    opaque.clear();
    transparent.clear();

    for (auto &room : _rooms) {
        shared_ptr<ModelInstance> model(room->model());
        if (!model) continue;

        glm::mat4 transform(glm::translate(glm::mat4(1.0f), room->position()));
        model->fillRenderLists(transform, opaque, transparent);
    }
    for (auto &pair : _objects) {
        for (auto &object : pair.second) {
            shared_ptr<ModelInstance> model(object->model());
            if (!model) continue;

            glm::mat4 transform(object->transform());
            model->fillRenderLists(transform, opaque, transparent);
        }
    }

    sort(
        opaque.begin(),
        opaque.end(),
        bind(sortByDistanceToCamera, _1, _2, cameraPosition));

    sort(
        transparent.begin(),
        transparent.end(),
        bind(sortByDistanceToCamera, _1, _2, cameraPosition));
}

void Area::render() const {
    for (auto &item : _renderLists.find(RenderListName::Opaque)->second) {
        item.model->render(*item.node, item.transform, _debugMode == DebugMode::ModelNodes);
    }
    for (auto &item : _renderLists.find(RenderListName::Transparent)->second) {
        item.model->render(*item.node, item.transform, _debugMode == DebugMode::ModelNodes);
    }

    if (_debugMode == DebugMode::GameObjects) {
        AABBMesh &aabb = AABBMesh::instance();
        for (auto &list : _objects) {
            if (list.first != ObjectType::Creature &&
                list.first != ObjectType::Door &&
                list.first != ObjectType::Placeable) continue;

            for (auto &object : list.second) {
                shared_ptr<ModelInstance> model(object->model());
                if (!model) return;

                aabb.render(model->model()->aabb(), object->transform());
            }
        }
    }
}

} // namespace game

} // namespace reone
