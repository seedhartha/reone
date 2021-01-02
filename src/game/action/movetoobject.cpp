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

#include "movetoobject.h"

using namespace std;

namespace reone {

namespace game {

MoveToObjectAction::MoveToObjectAction(const shared_ptr<Object> &object, bool run, float distance) :
    ObjectAction(ActionType::MoveToObject, object),
    _run(run),
    _distance(distance) {
}

bool MoveToObjectAction::getRun() const {
    return _run;
}

float MoveToObjectAction::distance() const {
    return _distance;
}

} // namespace game

} // namespace reone
