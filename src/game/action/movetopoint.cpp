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

#include "movetopoint.h"

#include "../game.h"
#include "../object/factory.h"
#include "../services.h"

using namespace std;

namespace reone {

namespace game {

void MoveToPointAction::execute(shared_ptr<Action> self, Object &actor, float dt) {
    auto creatureActor = _game.objectFactory().getObjectById<Creature>(actor.id());
    bool reached = creatureActor->navigateTo(_point, true, 1.0f, dt);
    if (reached) {
        complete();
    }
}

} // namespace game

} // namespace reone
