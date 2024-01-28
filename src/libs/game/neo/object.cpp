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

#include "reone/game/neo/object.h"

#include "reone/game/neo/actionexecutor.h"
#include "reone/game/neo/eventcollector.h"

namespace reone {

namespace game {

namespace neo {

void Object::update(IActionExecutor &actionExecutor, float dt) {
    if (!_actions.empty()) {
        const auto &action = _actions.front();
        bool completed = actionExecutor.executeAction(*this, action, dt);
        if (completed) {
            _actions.pop();
        }
    }
}

void Object::setState(ObjectState state) {
    if (_state == state) {
        return;
    }
    _state = state;

    Event event;
    event.type = EventType::ObjectStateChanged;
    event.object.objectId = _id;
    event.object.state = _state;
    publishEvent(std::move(event));
}

void Object::resetAnimation(std::string name) {
    AnimationStack animations;
    animations.push(std::move(name));
    std::swap(_animations, animations);

    Event event;
    event.type = EventType::ObjectAnimationReset;
    event.animation.objectId = _id;
    event.animation.name = _animations.top().c_str();
    publishEvent(std::move(event));
}

void Object::playFireForgetAnimation(const std::string &name) {
    AnimationStack animations;
    animations.push(std::move(name));
    std::swap(_animations, animations);

    Event event;
    event.type = EventType::ObjectFireForgetAnimationFired;
    event.animation.objectId = _id;
    event.animation.name = _animations.top().c_str();
    publishEvent(std::move(event));
}

void Object::collectEvents(IEventCollector &collector) {
    for (auto &event : _events) {
        collector.collectEvent(std::move(event));
    }
    _events.clear();
}

void SpatialObject::setPosition(glm::vec3 position) {
    if (_position == position) {
        return;
    }
    _position = std::move(position);

    Event event;
    event.type = EventType::ObjectLocationChanged;
    event.object.objectId = _id;
    publishEvent(std::move(event));
}

void SpatialObject::setFacing(float facing) {
    if (_facing == facing) {
        return;
    }
    _facing = facing;

    Event event;
    event.type = EventType::ObjectLocationChanged;
    event.object.objectId = _id;
    publishEvent(std::move(event));
}

void SpatialObject::setFacingPoint(const glm::vec3 &target) {
    glm::vec2 dir = glm::normalize(target - _position);
    setFacing(-glm::atan(dir.x, dir.y));
}

} // namespace neo

} // namespace game

} // namespace reone
