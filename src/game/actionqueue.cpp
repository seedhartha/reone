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

#include "actionqueue.h"

using namespace std;

namespace reone {

namespace game {

void ActionQueue::clear() {
    while (!_actions.empty()) {
        _actions.pop();
    }
}

void ActionQueue::push(unique_ptr<Action> action) {
    _actions.push(move(action));
}

void ActionQueue::update() {
    while (true) {
        const Action *action = currentAction();
        if (!action || !action->isCompleted()) return;

        _actions.pop();
    }
}

Action *ActionQueue::currentAction() {
   return _actions.empty() ? nullptr : _actions.front().get();
}

} // namespace game

} // namespace reone
