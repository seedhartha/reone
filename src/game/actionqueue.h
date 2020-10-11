/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "action/action.h"

namespace reone {

namespace game {

class ActionQueue {
public:
    void clear();
    void push(std::unique_ptr<Action> action);
    void update();

    Action *currentAction();

private:
    std::queue<std::unique_ptr<Action>> _actions;
};

} // namespace game

} // namespace reone
