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

#pragma once

#include <memory>
#include <queue>

#include "../common/timer.h"

#include "action/action.h"

namespace reone {

namespace game {

class ActionQueue {
public:
    void clear();

    void add(std::unique_ptr<Action> action);
    void delay(std::unique_ptr<Action> action, float seconds);

    void update(float dt);

    using iterator = std::deque<std::shared_ptr<Action>>::iterator;
    iterator begin();
    iterator end();

    bool empty() const;
    int size() const;

    std::shared_ptr<Action> currentAction();

private:
    struct DelayedAction {
        std::unique_ptr<Action> action;
        Timer timer;
    };

    std::deque<std::shared_ptr<Action>> _actions;
    std::vector<DelayedAction> _delayed;

    void removeCompletedActions();
    void updateDelayedActions(float dt);
};

} // namespace game

} // namespace reone
