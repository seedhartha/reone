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

#include "spatial.h"

#include "../../common/log.h"

#include "../room.h"

#include "item.h"
#include "objectfactory.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

SpatialObject::SpatialObject(
    uint32_t id,
    ObjectType type,
    Game *game,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph
) :
    Object(id, type, game),
    _objectFactory(objectFactory),
    _sceneGraph(sceneGraph) {

    ensureNotNull(objectFactory, "objectFactory");
    ensureNotNull(sceneGraph, "sceneGraph");
}

shared_ptr<Item> SpatialObject::addItem(const string &resRef, int stackSize, bool dropable) {
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
        result->loadFromBlueprint(resRef);
        result->setStackSize(stackSize);
        result->setDropable(dropable);

        _items.push_back(result);
    }

    return move(result);
}

void SpatialObject::addItem(const shared_ptr<Item> &item) {
    auto maybeItem = find_if(_items.begin(), _items.end(), [&item](auto &entry) { return entry->blueprintResRef() == item->blueprintResRef(); });
    if (maybeItem != _items.end()) {
        (*maybeItem)->setStackSize((*maybeItem)->stackSize() + 1);
    } else {
        _items.push_back(item);
    }
}

bool SpatialObject::removeItem(const shared_ptr<Item> &item, bool &last) {
    auto maybeItem = find(_items.begin(), _items.end(), item);
    if (maybeItem == _items.end()) return false;

    last = false;

    int stackSize = (*maybeItem)->stackSize();
    if (stackSize > 1) {
        (*maybeItem)->setStackSize(stackSize - 1);
    } else {
        last = true;
        _items.erase(maybeItem);
    }

    return true;
}

float SpatialObject::getDistanceTo(const glm::vec2 &point) const {
    return glm::distance(glm::vec2(_position), point);
}

float SpatialObject::getDistanceTo2(const glm::vec2 &point) const {
    return glm::distance2(glm::vec2(_position), point);
}

float SpatialObject::getDistanceTo(const glm::vec3 &point) const {
    return glm::distance(_position, point);
}

float SpatialObject::getDistanceTo2(const glm::vec3 &point) const {
    return glm::distance2(_position, point);
}

float SpatialObject::getDistanceTo(const SpatialObject &other) const {
    return glm::distance(_position, other._position);
}

float SpatialObject::getDistanceTo2(const SpatialObject &other) const {
    return glm::distance2(_position, other._position);
}

bool SpatialObject::contains(const glm::vec3 &point) const {
    if (!_sceneNode) return false;

    const AABB &aabb = _sceneNode->aabb();

    return (aabb * _transform).contains(point);
}

void SpatialObject::face(const SpatialObject &other) {
    if (_id != other._id) {
        face(other._position);
    }
}

void SpatialObject::face(const glm::vec3 &point) {
    if (point == _position) return;

    glm::vec2 dir(glm::normalize(point - _position));
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(dir.x, dir.y)));
    updateTransform();
}

void SpatialObject::faceAwayFrom(const SpatialObject &other) {
    if (_id == other._id || _position == other.position()) return;

    glm::vec2 dir(glm::normalize(_position - other.position()));
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(dir.x, dir.y)));
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

void SpatialObject::applyEffect(const shared_ptr<Effect> &effect, DurationType durationType, float duration) {
    if (durationType == DurationType::Instant) {
        applyInstantEffect(*effect);
    } else {
        AppliedEffect appliedEffect;
        appliedEffect.effect = effect;
        appliedEffect.durationType = durationType;
        appliedEffect.duration = duration;
        _effects.push_back(move(appliedEffect));
    }
}

void SpatialObject::applyInstantEffect(Effect &effect) {
    effect.applyTo(*this);
}

void SpatialObject::update(float dt) {
    Object::update(dt);
    updateEffects(dt);
}

void SpatialObject::updateEffects(float dt) {
    for (auto it = _effects.begin(); it != _effects.end(); ) {
        AppliedEffect &effect = *it;
        bool temporary = effect.durationType == DurationType::Temporary;
        if (temporary) {
            effect.duration = glm::max(0.0f, effect.duration - dt);
        }
        if (temporary && effect.duration == 0.0f) {
            applyInstantEffect(*effect.effect);
            it = _effects.erase(it);
        } else {
            ++it;
        }
    }
}

void SpatialObject::playAnimation(AnimationType animation, AnimationProperties properties, PlayAnimationAction *actionToComplete) {
}

bool SpatialObject::isSelectable() const {
    return false;
}

shared_ptr<Walkmesh> SpatialObject::getWalkmesh() const {
    return nullptr;
}

glm::vec3 SpatialObject::getSelectablePosition() const {
    auto model = static_pointer_cast<ModelSceneNode>(_sceneNode);
    return model ? model->getWorldCenterOfAABB() : _position;
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
    if (_sceneNode && !_stunt) {
        _sceneNode->setLocalTransform(_transform);
    }
}

void SpatialObject::setFacing(float facing) {
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, facing));
    updateTransform();
}

void SpatialObject::setVisible(bool visible) {
    if (_visible == visible) return;

    _visible = visible;

    if (_sceneNode) {
        _sceneNode->setVisible(visible);
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

shared_ptr<Item> SpatialObject::getItemByTag(const string &tag) {
    for (auto &item : _items) {
        if (item->tag() == tag) return item;
    }
    return nullptr;
}

void SpatialObject::clearAllEffects() {
    _effects.clear();
}

void SpatialObject::die() {
}

void SpatialObject::startStuntMode() {
    if (_sceneNode) {
        _sceneNode->setLocalTransform(glm::mat4(1.0f));
        _sceneNode->setCullable(false);
    }
    _stunt = true;
}

void SpatialObject::stopStuntMode() {
    if (!_stunt) return;

    if (_sceneNode) {
        _sceneNode->setLocalTransform(_transform);
        _sceneNode->setCullable(true);
    }
    _stunt = false;
}

shared_ptr<Effect> SpatialObject::getFirstEffect() {
    _effectIndex = 1;

    if (_effects.empty()) return nullptr;

    return _effects.front().effect;
}

shared_ptr<Effect> SpatialObject::getNextEffect() {
    if (_effects.size() <= _effectIndex) return nullptr;

    return _effects[_effectIndex++].effect;
}

} // namespace game

} // namespace reone
