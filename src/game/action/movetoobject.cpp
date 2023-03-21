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

#include "movetoobject.h"

#include "../../common/exception/validation.h"

#include "../object.h"
#include "../object/creature.h"

using namespace std;

namespace reone {

namespace game {

void MoveToObjectAction::execute(Object &actor, float delta) {
    if (actor.type() != ObjectType::Creature) {
        throw ValidationException("Actor must be of creature type");
    }
    auto &creatureActor = static_cast<Creature &>(actor);
    if (creatureActor.moveTo(_moveTo, _run, _range, delta)) {
        complete();
    }
}

} // namespace game

} // namespace reone
