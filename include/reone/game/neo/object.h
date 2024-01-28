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

#pragma once

#include "../types.h"

#include "action.h"
#include "effect.h"
#include "event.h"
#include "types.h"

namespace reone {

namespace game {

namespace neo {

class IActionExecutor;
class IEventCollector;

using ActionQueue = std::queue<Action>;
using EffectList = std::vector<Effect>;
using AnimationStack = std::stack<std::string>;

class Object : boost::noncopyable {
public:
    virtual void update(IActionExecutor &actionExecutor, float dt);

    ObjectId id() const {
        return _id;
    }

    const ObjectTag &tag() const {
        return _tag;
    }

    ObjectType type() const {
        return _type;
    }

    ObjectState state() const {
        return _state;
    }

    bool is(ObjectState state) const {
        return _state == state;
    }

    bool operator==(const Object &that) const {
        return this == &that;
    }

    bool operator!=(const Object &that) const {
        return this != &that;
    }

    // Actions

    void clearAllActions() {
        ActionQueue empty;
        std::swap(_actions, empty);
    }

    void add(Action action) {
        _actions.push(std::move(action));
    }

    std::optional<Action> currentAction() const {
        if (_actions.empty()) {
            return std::nullopt;
        }
        return _actions.front();
    }

    // END Actions

    // Effects

    void apply(Effect effect) {
        _effects.push_back(std::move(effect));
    }

    // END Effects

protected:
    ObjectId _id;
    ObjectTag _tag;
    ObjectType _type;
    IEventCollector &_eventCollector;

    ObjectState _state {ObjectState::Created};

    ActionQueue _actions;
    EffectList _effects;
    AnimationStack _animations;

    Object(ObjectId id,
           ObjectTag tag,
           ObjectType type,
           IEventCollector &eventCollector) :
        _id(id),
        _tag(std::move(tag)),
        _type(type),
        _eventCollector(eventCollector) {
        collectObjectCreatedEvent();
    }

    void setState(ObjectState state);

    void resetAnimation(std::string name);
    void playFireForgetAnimation(const std::string &name);

private:
    void collectObjectCreatedEvent();
};

class SpatialObject : public Object {
public:
    const glm::vec3 &position() const {
        return _position;
    }

    float facing() const {
        return _facing;
    }

    void setPosition(glm::vec3 position);
    void setFacing(float facing);
    void setFacingPoint(const glm::vec3 &target);

protected:
    glm::vec3 _position {0.0f};
    float _facing {0.0f};

    SpatialObject(ObjectId id,
                  ObjectTag tag,
                  ObjectType type,
                  IEventCollector &eventCollector) :
        Object(
            id,
            std::move(tag),
            type,
            eventCollector) {
    }
};

} // namespace neo

} // namespace game

} // namespace reone
