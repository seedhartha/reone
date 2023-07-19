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

#include "reone/game/object/scenebridge.h"

namespace reone {

namespace game {

void ObjectSceneBridge::applyAll() {
    while (!_events.empty()) {
        auto &event = _events.front();
        apply(event);
        _events.pop();
    }
}

void ObjectSceneBridge::apply(const ObjectEvent &event) {
    switch (event.type) {
    case ObjectEventType::ObjectCreated:
        applyObjectCreated(event);
        break;
    case ObjectEventType::ObjectDestroyed:
        applyObjectDestroyed(event);
        break;
    case ObjectEventType::ObjectTransformChanged:
        applyObjectTransformChanged(event);
        break;
    default:
        // do nothing
        break;
    }
}

void ObjectSceneBridge::applyObjectCreated(const ObjectEvent &event) {
}

void ObjectSceneBridge::applyObjectDestroyed(const ObjectEvent &event) {
}

void ObjectSceneBridge::applyObjectTransformChanged(const ObjectEvent &event) {
}

} // namespace game

} // namespace reone
