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

using namespace reone::script;

namespace reone {

namespace game {

Action::Action(ActionType type) : type(type) {
}

Action::Action(ActionType type, const shared_ptr<Object> &object, float distance) : type(type), object(object), distance(distance) {
}

Action::Action(ActionType type, const ExecutionContext &ctx) : type(type), context(ctx) {
}

void ActionQueue::clear() {
    while (!_actions.empty()) {
        _actions.pop();
    }
}

void ActionQueue::push(Action action) {
    _actions.push(move(action));
}

void ActionQueue::pop() {
    _actions.pop();
}

const Action *ActionQueue::currentAction() const {
   return _actions.empty() ? nullptr : &_actions.back();
}

} // namespace game

} // namespace reone
