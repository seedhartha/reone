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

#include "object.h"

#include <stdexcept>

#include "../../common/collectionutil.h"
#include "../../common/guardutil.h"

using namespace std;

namespace reone {

namespace game {

Object::Object(uint32_t id, ObjectType type, Game *game) : _id(id), _type(type), _game(game) {
    ensureNotNull(game, "game");
}

void Object::update(float dt) {
    updateActions(dt);
}

bool Object::getLocalBoolean(int index) const {
    return getFromLookupOrElse(_localBooleans, index, false);
}

int Object::getLocalNumber(int index) const {
    return getFromLookupOrElse(_localNumbers, index, 0);
}

void Object::setLocalBoolean(int index, bool value) {
    _localBooleans[index] = value;
}

void Object::setLocalNumber(int index, int value) {
    _localNumbers[index] = value;
}

} // namespace game

} // namespace reone
