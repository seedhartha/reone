/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../common/log.h"

#include "../blueprint/blueprints.h"
#include "../room.h"

#include "item.h"
#include "objectfactory.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

SpatialObject::SpatialObject(uint32_t id, ObjectType type, ObjectFactory *objectFactory, SceneGraph *sceneGraph) :
    Object(id, type),
    _objectFactory(objectFactory),
    _sceneGraph(sceneGraph) {
}

shared_ptr<Item> SpatialObject::addItem(const string &resRef, int stackSize, bool dropable) {
    auto blueprint = Blueprints::instance().getItem(resRef);
    if (!blueprint) return nullptr;

    shared_ptr<Item> result;

    auto maybeItem = find_if(_items.begin(), _items.end(), [&resRef](auto &item) {
        return item->blueprintResRef() == resRef;
    });
    if (maybeItem != _items.end()) {
        result = *maybeItem;
        int prevStackSize = result->stackSize();
        result->setStackSize(prevStackSize + stackSize);

    } else {
        result = _objectFactory->newItem();
        result->load(blueprint);
        result->setStackSize(stackSize);
        result->setDropable(dropable);

        _items.push_back(result);
    }

    return move(result);
}

void SpatialObject::addItem(const shared_ptr<Item> &item) {
    _items.push_back(item);
}

void SpatialObject::removeItem(const shared_ptr<Item> &item) {
    _items.erase(remove(_items.begin(), _items.end(), item), _items.end());
}

float SpatialObject::distanceTo(const glm::vec2 &point) const {
    return glm::distance(glm::vec2(_position), point);
}

float SpatialObject::distanceTo(const glm::vec3 &point) const {
    return glm::distance(_position, point);
}

float SpatialObject::distanceTo(const SpatialObject &other) const {
    return glm::distance(_position, other._position);
}

bool SpatialObject::contains(const glm::vec3 &point) const {
    if (!_model) return false;

    const AABB &aabb = _model->model()->aabb();

    return (aabb * _transform).contains(point);
}

void SpatialObject::face(const SpatialObject &other) {
    if (_id == other._id) return;

    glm::vec2 dir(glm::normalize(other._position - _position));
    _facing = -glm::atan(dir.x, dir.y);
    updateTransform();
}

void SpatialObject::moveDropableItemsTo(SpatialObject &other) {
    for (auto it = _items.begin(); it != _items.end(); ) {
        if ((*it)->isDropable()) {
            other._items.push_back(*it);
            it = _items.erase(it);
        } else {
            ++it;
        }
    }
}

void SpatialObject::applyEffect(const shared_ptr<Effect> &effect) {
    auto damage = dynamic_pointer_cast<DamageEffect>(effect);
    if (damage) {
        debug(boost::format("SpatialObject: '%s' takes %d damage") % _tag % damage->amount(), 2);
        _currentHitPoints = glm::max(_minOneHP ? 1 : 0, _currentHitPoints - damage->amount());
    } else {
        _effects.push_back(effect);
    }
}

void SpatialObject::update(float dt) {
    Object::update(dt);
    if (_model) {
        _model->update(dt);
    }
}

bool SpatialObject::isSelectable() const {
    return false;
}

bool SpatialObject::isOpen() const {
    return _open;
}

ObjectFactory &SpatialObject::objectFactory() {
    return *_objectFactory;
}

SceneGraph &SpatialObject::sceneGraph() {
    return *_sceneGraph;
}

Room *SpatialObject::room() const {
    return _room;
}

const glm::vec3 &SpatialObject::position() const {
    return _position;
}

float SpatialObject::facing() const {
    return _facing;
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

glm::vec3 SpatialObject::getSelectablePosition() const {
    return _model->getCenterOfAABB();
}

float SpatialObject::drawDistance() const {
    return _drawDistance;
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

    if (_facing != 0.0f) {
        _transform *= glm::eulerAngleZ(_facing);
    }
    if (_model) {
        _model->setLocalTransform(_transform);
    }
}

void SpatialObject::setFacing(float facing) {
    _facing = facing;
    updateTransform();
}

void SpatialObject::setVisible(bool visible) {
    if (_visible == visible) return;

    _visible = visible;

    if (_model) {
        _model->setVisible(visible);
    }
}

shared_ptr<Item> SpatialObject::getFirstItem() {
    _itemIndex = 0;
    return getNextItem();
}

shared_ptr<Item> SpatialObject::getNextItem() {
    int itemCount = static_cast<int>(_items.size());
    if (itemCount > _itemIndex) {
        return _items[_itemIndex++];
    }
    return nullptr;
}

} // namespace game

} // namespace reone
