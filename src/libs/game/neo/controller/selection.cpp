/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/neo/controller/selection.h"

#include "reone/game/neo/object/door.h"
#include "reone/graphics/options.h"
#include "reone/input/event.h"
#include "reone/scene/graph.h"
#include "reone/system/logutil.h"

namespace reone {

namespace game {

namespace neo {

bool SelectionController::handle(const input::Event &event) {
    switch (event.type) {
    case input::EventType::MouseMotion:
        _hoveredObject = findObjectAt(event.motion.x, event.motion.y);
        return true;
    case input::EventType::MouseButtonDown:
        if (event.button.button == input::MouseButton::Left) {
            _selectedObject = _hoveredObject;
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

std::optional<std::reference_wrapper<SpatialObject>> SelectionController::findObjectAt(int x, int y) const {
    if (!_camera) {
        return std::nullopt;
    }
    const auto &camera = *_camera->get().camera();
    glm::vec3 cursorNDC {2.0f * x / static_cast<float>(_graphicsOpt.width) - 1.0f,
                         1.0f - 2.0f * y / static_cast<float>(_graphicsOpt.height),
                         -1.0f};
    auto cursorView = camera.projectionInv() * glm::vec4 {cursorNDC, 1.0f};
    cursorView /= cursorView.w;
    glm::vec3 cursorWorld = camera.viewInv() * cursorView;
    auto cursorDir = glm::normalize(cursorWorld - camera.position());
    auto picked = _scene.pickModelRay(camera.position(), cursorDir);
    if (!picked) {
        return std::nullopt;
    }
    auto ref = picked->get().externalRef();
    if (!ref) {
        return std::nullopt;
    }
    auto &object = *reinterpret_cast<SpatialObject *>(ref);
    if (object.type() == ObjectType::Door) {
        auto &door = static_cast<Door &>(object);
        if (door.doorState() != DoorState::Closed) {
            return std::nullopt;
        }
    }
    return object;
}

} // namespace neo

} // namespace game

} // namespace reone
