/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "object.h"

#include "reone/graphics/services.h"
#include "reone/graphics/window.h"
#include "../scene/graph.h"

#include "options.h"

namespace reone {

namespace game {

void Object::update(float delta) {
    if (_actions.empty()) {
        return;
    }
    auto action = _actions.front();
    action->execute(*this, delta);
    if (action->isCompleted()) {
        _actions.pop();
    }
}

void Object::face(Object &other) {
    face(glm::vec2(other._position));
}

void Object::face(const glm::vec2 &point) {
    auto dir = glm::normalize(point - glm::vec2(_position));
    setFacing(-glm::atan(dir.x, dir.y));
}

void Object::show() {
    if (!_sceneNode) {
        return;
    }
    _sceneNode->setEnabled(true);
}

void Object::hide() {
    if (!_sceneNode) {
        return;
    }
    _sceneNode->setEnabled(false);
}

glm::vec3 Object::targetWorldCoords() const {
    if (!_sceneNode) {
        return _position;
    }
    return _sceneNode->getWorldCenterOfAABB();
}

glm::ivec3 Object::targetScreenCoords() const {
    auto camera = _sceneGraph->activeCamera();
    auto viewport = glm::ivec4(0, 0, _graphicsOpt.width, _graphicsOpt.height);
    auto screenCoords = glm::project(
        targetWorldCoords(),
        camera->camera()->view(),
        camera->camera()->projection(),
        viewport);

    return glm::ivec3(screenCoords.x, _graphicsOpt.height - screenCoords.y, screenCoords.z);
}

} // namespace game

} // namespace reone
