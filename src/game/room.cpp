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

#include "room.h"

#include "object/spatial.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

Room::Room(
    const string &name,
    const glm::vec3 &position,
    const std::shared_ptr<ModelSceneNode> &model,
    const std::shared_ptr<Walkmesh> &walkmesh
) :
    _name(name), _position(position), _model(model), _walkmesh(walkmesh) {
}

void Room::addTenant(SpatialObject *object) {
    _tenants.insert(object);
}

void Room::removeTenant(SpatialObject *object) {
    _tenants.erase(object);
}

void Room::update(float dt) {
}

bool Room::isVisible() const {
    return _visible;
}

const string &Room::name() const {
    return _name;
}

const glm::vec3 &Room::position() const {
    return _position;
}

shared_ptr<ModelSceneNode> Room::model() const {
    return _model;
}

const Walkmesh *Room::walkmesh() const {
    return _walkmesh.get();
}

void Room::setVisible(bool visible) {
    for (auto &tenant : _tenants) {
        tenant->setVisible(visible);
    }
    if (_visible == visible) return;

    _visible = visible;

    if (_model) {
        _model->setVisible(visible);
    }
}

} // namespace game

} // namespace reone
