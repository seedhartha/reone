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

#include "../../script/enginetype.h"

namespace reone {

namespace game {

class Location : public script::EngineType {
public:
    Location(glm::vec3 position, float facing) :
        _position(std::move(position)),
        _facing(facing) {
    }

    const glm::vec3 &position() const { return _position; }
    float facing() const { return _facing; }

private:
    glm::vec3 _position;
    float _facing;
};

} // namespace game

} // namespace reone
