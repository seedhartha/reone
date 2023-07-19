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

#pragma once

namespace reone {

namespace game {

enum class ObjectEventType {
    Invalid = -1,

    ObjectCreated = 0,
    ObjectDestroyed = 1,

    ObjectTransformChanged = 2
};

struct ObjectEvent {
    ObjectEventType type {ObjectEventType::Invalid};
    uint32_t objectId {0};
    std::string modelResRef;
    std::string texResRef;
    glm::mat4 transform {1.0f};
};

} // namespace game

} // namespace reone
