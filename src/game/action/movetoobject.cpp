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

void MoveToObjectAction::execute(Object &executor, float delta) {
    if (executor.type() != ObjectType::Creature) {
        throw ValidationException("Executor must be of creature type");
    }
    auto &executorCreature = static_cast<Creature &>(executor);

    auto dist = executorCreature.square2dDistanceTo(_moveTo);
    if (dist < _range * _range) {
        executorCreature.setState(Creature::State::Pause);
        complete();
        return;
    }

    executorCreature.face(_moveTo);
    executorCreature.setState(_run ? Creature::State::Run : Creature::State::Walk);
    executorCreature.moveForward(delta);
}

} // namespace game

} // namespace reone
