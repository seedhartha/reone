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

#include "objectaction.h"

#include <stdexcept>

using namespace std;

namespace reone {

namespace game {

ObjectAction::ObjectAction(ActionType type, Object *object) : Action(type), _object(object) {
    if (!object) {
        throw invalid_argument("Object must not be null");
    }
}

Object *ObjectAction::object() const {
    return _object;
}

} // namespace game

} // namespace reone
