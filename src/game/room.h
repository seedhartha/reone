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

#include <memory>

#include "glm/vec3.hpp"

#include "../render/modelinstance.h"
#include "../render/walkmesh.h"

namespace reone {

namespace game {

class Room {
public:
    Room(
        const std::string &name,
        const glm::vec3 &position,
        const std::shared_ptr<render::ModelInstance> &model,
        const std::shared_ptr<render::Walkmesh> &walkmesh);

    const glm::vec3 &position() const;
    std::shared_ptr<render::ModelInstance> model() const;
    std::shared_ptr<render::Walkmesh> walkmesh() const;

private:
    std::string _name;
    glm::vec3 _position { 0.0f };
    std::shared_ptr<render::ModelInstance> _model;
    std::shared_ptr<render::Walkmesh> _walkmesh;
};

} // namespace game

} // namespace reone
