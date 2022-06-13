/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "gameobject.h"

using namespace std;

namespace reone {

namespace game {

unique_ptr<MockObject> mockObject(uint32_t id, ObjectType type) {
    return make_unique<MockObject>(id, type);
}

unique_ptr<MockCreature> mockCreature(uint32_t id) {
    return make_unique<MockCreature>(id);
}

} // namespace game

} // namespace reone
