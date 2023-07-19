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

#include "reone/game/object.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/item.h"
#include "reone/game/room.h"
#include "reone/system/logutil.h"

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kKeepPathDuration = 1000.0f;
static constexpr float kDefaultMaxObjectDistance = 2.0f;
static constexpr float kMaxConversationDistance = 4.0f;
static constexpr float kDistanceWalk = 4.0f;

void Object::update(float dt) {
    updateActions(dt);
    updateEffects(dt);
    if (!_dead) {
        executeActions(dt);
    }
    if (_sceneNode && _sceneNode->type() == SceneNodeType::Model) {
        std::static_pointer_cast<ModelSceneNode>(_sceneNode)->setPickable(isSelectable());
    }
}

bool Object::getLocalBoolean(int index) const {
    auto it = _localBooleans.find(index);
    return it != _localBooleans.end() ? it->second : false;
}

int Object::getLocalNumber(int index) const {
    auto it = _localNumbers.find(index);
    return it != _localNumbers.end() ? it->second : 0;
}

void Object::setLocalBoolean(int index, bool value) {
    _localBooleans[index] = value;
}

void Object::setLocalNumber(int index, int value) {
    _localNumbers[index] = value;
}

void Object::clearAllActions() {
    while (!_actions.empty()) {
        _actions.pop_front();
    }
}

void Object::addAction(std::shared_ptr<Action> action) {
    _actions.push_back(std::move(action));
}

void Object::addActionOnTop(std::shared_ptr<Action> action) {
    _actions.push_front(std::move(action));
}

void Object::delayAction(std::shared_ptr<Action> action, float seconds) {
    DelayedAction delayed;
    delayed.action = std::move(action);
    delayed.timer = std::make_unique<Timer>(seconds);
    _delayed.push_back(std::move(delayed));
}

void Object::updateActions(float dt) {
    removeCompletedActions();
    updateDelayedActions(dt);
}

void Object::removeCompletedActions() {
    while (true) {
        std::shared_ptr<Action> action(getCurrentAction());
        if (!action || !action->isCompleted())
            return;

        _actions.pop_front();
    }
}

void Object::updateDelayedActions(float dt) {
    for (auto &delayed : _delayed) {
        delayed.timer->update(dt);
        if (delayed.timer->elapsed()) {
            _actions.push_back(std::move(delayed.action));
        }
    }
    auto delayedToRemove = std::remove_if(
        _delayed.begin(),
        _delayed.end(),
        [](const DelayedAction &delayed) { return delayed.timer->elapsed(); });

    _delayed.erase(delayedToRemove, _delayed.end());
}

void Object::executeActions(float dt) {
    if (_actions.empty()) {
        return;
    }
    std::shared_ptr<Action> action(_actions.front());
    action->execute(action, *this, dt);
}

bool Object::hasUserActionsPending() const {
    // TODO: must only work during combat
    for (auto &action : _actions) {
        if (action->isUserAction())
            return true;
    }
    return false;
}

std::shared_ptr<Action> Object::getCurrentAction() const {
    return _actions.empty() ? nullptr : _actions.front();
}

std::shared_ptr<Item> Object::addItem(const std::string &resRef, int stackSize, bool dropable) {
    std::shared_ptr<Item> result;

    auto maybeItem = std::find_if(_items.begin(), _items.end(), [&resRef](auto &item) {
        return item->blueprintResRef() == resRef;
    });
    if (maybeItem != _items.end()) {
        result = *maybeItem;
        int prevStackSize = result->stackSize();
        result->setStackSize(prevStackSize + stackSize);

    } else {
        result = _game.newItem();
        result->loadFromBlueprint(resRef);
        result->setStackSize(stackSize);
        result->setDropable(dropable);

        _items.push_back(result);
    }

    return result;
}

void Object::addItem(const std::shared_ptr<Item> &item) {
    auto maybeItem = std::find_if(_items.begin(), _items.end(), [&item](auto &entry) { return entry->blueprintResRef() == item->blueprintResRef(); });
    if (maybeItem != _items.end()) {
        (*maybeItem)->setStackSize((*maybeItem)->stackSize() + 1);
    } else {
        _items.push_back(item);
    }
}

bool Object::removeItem(const std::shared_ptr<Item> &item, bool &last) {
    auto maybeItem = find(_items.begin(), _items.end(), item);
    if (maybeItem == _items.end())
        return false;

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

float Object::getDistanceTo(const glm::vec2 &point) const {
    return glm::distance(glm::vec2(_position), point);
}

float Object::getSquareDistanceTo(const glm::vec2 &point) const {
    return glm::distance2(glm::vec2(_position), point);
}

float Object::getDistanceTo(const glm::vec3 &point) const {
    return glm::distance(_position, point);
}

float Object::getSquareDistanceTo(const glm::vec3 &point) const {
    return glm::distance2(_position, point);
}

float Object::getDistanceTo(const Object &other) const {
    return glm::distance(_position, other._position);
}

float Object::getSquareDistanceTo(const Object &other) const {
    return glm::distance2(_position, other._position);
}

bool Object::contains(const glm::vec3 &point) const {
    if (!_sceneNode)
        return false;

    const AABB &aabb = _sceneNode->aabb();

    return (aabb * _transform).contains(point);
}

void Object::face(const Object &other) {
    if (_id != other._id) {
        face(other._position);
    }
}

void Object::face(const glm::vec3 &point) {
    if (point == _position)
        return;

    glm::vec2 dir(glm::normalize(point - _position));
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(dir.x, dir.y)));
    updateTransform();
}

void Object::faceAwayFrom(const Object &other) {
    if (_id == other._id || _position == other.position())
        return;

    glm::vec2 dir(glm::normalize(_position - other.position()));
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(dir.x, dir.y)));
    updateTransform();
}

void Object::moveDropableItemsTo(Object &other) {
    for (auto it = _items.begin(); it != _items.end();) {
        if ((*it)->isDropable()) {
            other._items.push_back(*it);
            it = _items.erase(it);
        } else {
            ++it;
        }
    }
}

void Object::applyEffect(const std::shared_ptr<Effect> &effect, DurationType durationType, float duration) {
    if (durationType == DurationType::Instant) {
        applyInstantEffect(*effect);
    } else {
        AppliedEffect appliedEffect;
        appliedEffect.effect = effect;
        appliedEffect.durationType = durationType;
        appliedEffect.duration = duration;
        _effects.push_back(std::move(appliedEffect));
    }
}

void Object::applyInstantEffect(Effect &effect) {
    effect.applyTo(*this);
}

void Object::updateEffects(float dt) {
    for (auto it = _effects.begin(); it != _effects.end();) {
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

void Object::playAnimation(AnimationType animation, AnimationProperties properties) {
}

bool Object::isSelectable() const {
    return false;
}

glm::vec3 Object::getSelectablePosition() const {
    auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
    return model ? model->getWorldCenterOfAABB() : _position;
}

void Object::setRoom(Room *room) {
    if (_room) {
        _room->removeTenant(this);
    }
    _room = room;

    if (_room) {
        _room->addTenant(this);
    }
}

void Object::setPosition(const glm::vec3 &position) {
    _position = position;
    updateTransform();
}

void Object::updateTransform() {
    _transform = glm::translate(glm::mat4(1.0f), _position);
    _transform *= glm::mat4_cast(_orientation);

    if (_sceneNode && !_stunt) {
        _sceneNode->setLocalTransform(_transform);
    }
}

void Object::setFacing(float facing) {
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, facing));
    updateTransform();
}

void Object::setVisible(bool visible) {
    if (_visible == visible)
        return;

    _visible = visible;

    if (_sceneNode) {
        _sceneNode->setEnabled(visible);
    }
}

std::shared_ptr<Item> Object::getFirstItem() {
    _itemIndex = 0;
    return getNextItem();
}

std::shared_ptr<Item> Object::getNextItem() {
    int itemCount = static_cast<int>(_items.size());
    if (itemCount > _itemIndex) {
        return _items[_itemIndex++];
    }
    return nullptr;
}

std::shared_ptr<Item> Object::getItemByTag(const std::string &tag) {
    for (auto &item : _items) {
        if (item->tag() == tag)
            return item;
    }
    return nullptr;
}

void Object::clearAllEffects() {
    _effects.clear();
}

void Object::die() {
}

void Object::startStuntMode() {
    if (_sceneNode) {
        _sceneNode->setLocalTransform(glm::mat4(1.0f));
        _sceneNode->setCullable(false);
    }
    _stunt = true;
}

void Object::stopStuntMode() {
    if (!_stunt)
        return;

    if (_sceneNode) {
        _sceneNode->setLocalTransform(_transform);
        _sceneNode->setCullable(true);
    }
    _stunt = false;
}

std::shared_ptr<Effect> Object::getFirstEffect() {
    _effectIndex = 1;
    return !_effects.empty() ? _effects[0].effect : nullptr;
}

std::shared_ptr<Effect> Object::getNextEffect() {
    return (_effectIndex < _effects.size()) ? _effects[_effectIndex++].effect : nullptr;
}

bool Object::isInLineOfSight(const Object &other, float fov) const {
    if (other._position == _position) {
        return true;
    }
    auto normal = _orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    auto dir = glm::normalize(glm::vec3(glm::vec2(other._position - _position), 0.0f));
    float dot = glm::dot(normal, dir);
    return dot > 0.0f && glm::acos(dot) < fov;
}

} // namespace game

} // namespace reone
