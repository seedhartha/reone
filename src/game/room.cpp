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

#include "room.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace game {

Room::Room(
    const string &name,
    const glm::vec3 &position,
    const shared_ptr<render::ModelSceneNode> &model,
    const shared_ptr<render::Walkmesh> &walkmesh
) :
    _name(name), _position(position), _model(model), _walkmesh(walkmesh) {
}

const glm::vec3 &Room::position() const {
    return _position;
}

shared_ptr<ModelSceneNode> Room::model() const {
    return _model;
}

shared_ptr<Walkmesh> Room::walkmesh() const {
    return _walkmesh;
}

} // namespace game

} // namespace reone
