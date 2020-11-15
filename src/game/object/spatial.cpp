/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "spatial.h"

#include "glm/gtx/euler_angles.hpp"

#include "../room.h"

#include "item.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

SpatialObject::SpatialObject(uint32_t id, ObjectType type, SceneGraph *sceneGraph) :
    Object(id, type), _sceneGraph(sceneGraph) {
}

void SpatialObject::addItem(const shared_ptr<Item> &item) {
    _items.push_back(item);
}

float SpatialObject::distanceTo(const glm::vec2 &point) const {
    return glm::distance(glm::vec2(_position), point);
}

float SpatialObject::distanceTo(const glm::vec3 &point) const {
    return glm::distance(_position, point);
}

bool SpatialObject::contains(const glm::vec3 &point) const {
    if (!_model) return false;

    const AABB &aabb = _model->model()->aabb();

    return (aabb * _transform).contains(point);
}

void SpatialObject::face(const SpatialObject &other) {
    glm::vec2 dir(glm::normalize(other._position - _position));
    _heading = -glm::atan(dir.x, dir.y);
    updateTransform();
}

void SpatialObject::moveItemsTo(SpatialObject &other) {
    for (auto &item : _items) {
        other._items.push_back(item);
    }
    _items.clear();
}

void SpatialObject::update(float dt) {
    Object::update(dt);
    if (_model) {
        _model->update(dt);
    }
}

void SpatialObject::playAnimation(const string &name, int flags, float speed) {
    if (_model) {
        _model->playAnimation(name, flags, speed);
    }
}

bool SpatialObject::isSelectable() const {
    return _selectable;
}

Room *SpatialObject::room() const {
    return _room;
}

const glm::vec3 &SpatialObject::position() const {
    return _position;
}

float SpatialObject::heading() const {
    return _heading;
}

const glm::mat4 &SpatialObject::transform() const {
    return _transform;
}

bool SpatialObject::visible() const {
    return _visible;
}

shared_ptr<ModelSceneNode> SpatialObject::model() const {
    return _model;
}

shared_ptr<Walkmesh> SpatialObject::walkmesh() const {
    return _walkmesh;
}

const vector<shared_ptr<Item>> &SpatialObject::items() const {
    return _items;
}

glm::vec3 SpatialObject::selectablePosition() const {
    return _model->getCenterOfAABB();
}

float SpatialObject::drawDistance() const {
    return _drawDistance;
}

float SpatialObject::fadeDistance() const {
    return _fadeDistance;
}

void SpatialObject::setRoom(Room *room) {
    if (_room) {
        _room->removeTenant(this);
    }
    _room = room;

    if (_room) {
        _room->addTenant(this);
    }
}

void SpatialObject::setPosition(const glm::vec3 &position) {
    _position = position;
    updateTransform();
}

void SpatialObject::updateTransform() {
    _transform = glm::translate(glm::mat4(1.0f), _position);
    _transform *= glm::mat4_cast(_orientation);

    if (_heading != 0.0f) {
        _transform *= glm::eulerAngleZ(_heading);
    }
    if (_model) {
        _model->setLocalTransform(_transform);
    }
}

void SpatialObject::setHeading(float heading) {
    _heading = heading;
    updateTransform();
}

void SpatialObject::setVisible(bool visible) {
    if (_visible == visible) return;

    _visible = visible;

    if (_model) {
        _model->setVisible(visible);
    }
}

} // namespace game

} // namespace reone
