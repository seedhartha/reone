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

SpatialObject::SpatialObject(
    uint32_t id,
    ObjectType type,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph,
    ScriptRunner *scriptRunner
) :
    Object(id, type),
    _objectFactory(objectFactory),
    _sceneGraph(sceneGraph),
    _scriptRunner(scriptRunner) {
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
    auto model = getModelSceneNode();
    if (!model) return false;

    const AABB &aabb = model->model()->aabb();

    return (aabb * _transform).contains(point);
}

void SpatialObject::face(const SpatialObject &other) {
    if (_id != other._id) {
        face(other._position);
    }
}

void SpatialObject::face(const glm::vec3 &point) {
    glm::vec2 dir(glm::normalize(point - _position));
    _facing = -glm::atan(dir.x, dir.y);
    updateTransform();
}

void SpatialObject::faceAwayFrom(const SpatialObject &other) {
    if (_id == other._id) return;

    glm::vec2 dir(glm::normalize(_position - other.position()));
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
    switch (effect.type()) {
        case EffectType::Damage: {
            auto &damageEffect = static_cast<DamageEffect &>(effect);
            debug(boost::format("Damage taken: %s %d") % _tag % damageEffect.amount(), 2);
            _currentHitPoints = glm::max(_minOneHP ? 1 : 0, _currentHitPoints - damageEffect.amount());
            break;
        }
        case EffectType::Death:
            die();
            break;
        default:
            debug("Unsupported effect type: " + to_string(static_cast<int>(effect.type())), 2);
            break;
    }
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

void SpatialObject::playAnimation(AnimationType animation, AnimationProperties properties, shared_ptr<PlayAnimationAction> actionToComplete) {
}

bool SpatialObject::isSelectable() const {
    return false;
}

shared_ptr<ModelSceneNode> SpatialObject::getModelSceneNode() const {
    if (!_sceneNode || _sceneNode->type() != SceneNodeType::Model) return nullptr;

    return static_pointer_cast<ModelSceneNode>(_sceneNode);
}

shared_ptr<Walkmesh> SpatialObject::getWalkmesh() const {
    return nullptr;
}

glm::vec3 SpatialObject::getSelectablePosition() const {
    auto model = getModelSceneNode();
    return model ? model->getCenterOfAABB() : _position;
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
    if (_sceneNode && !_stunt) {
        _sceneNode->setLocalTransform(_transform);
    }
}

void SpatialObject::setFacing(float facing) {
    _facing = facing;
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
    }
    _stunt = true;
}

void SpatialObject::stopStuntMode() {
    if (!_stunt) return;

    if (_sceneNode) {
        _sceneNode->setLocalTransform(_transform);
    }
    _stunt = false;
}

} // namespace game

} // namespace reone
