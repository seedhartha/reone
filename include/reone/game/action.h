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

#include "types.h"

namespace reone {

namespace game {

struct ServicesView;

class Creature;
class Game;
class Object;

class Action : boost::noncopyable {
public:
    virtual ~Action() = default;

    virtual void execute(std::shared_ptr<Action> self, Object &actor, float dt);

    void complete() { _completed = true; }

    ActionType type() const { return _type; }

    bool isUserAction() const { return _userAction; }
    bool isCompleted() const { return _completed; }

    void setUserAction(bool val) { _userAction = val; }

protected:
    const float kDefaultMaxObjectDistance = 2.0f;
    const float kDistanceWalk = 4.0f;

    Game &_game;
    ServicesView &_services;
    ActionType _type;

    bool _userAction {false};
    bool _completed {false};

    Action(
        Game &game,
        ServicesView &services,
        ActionType type) :
        _game(game),
        _services(services),
        _type(type) {
    }
};

} // namespace game

} // namespace reone
