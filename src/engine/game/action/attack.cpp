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

#include "attack.h"

#include "../combat.h"
#include "../game.h"
#include "../object/creature.h"
#include "../object/factory.h"

#include "context.h"

using namespace std;

namespace reone {

namespace game {

void AttackAction::execute(Object &actor, ActionContext &ctx, float dt) {
    // If target is dead, complete the action
    shared_ptr<SpatialObject> target(static_pointer_cast<SpatialObject>(_object));
    if (target->isDead()) {
        complete();
        return;
    }

    auto creatureActor = ctx.objectFactory.getObjectById<Creature>(actor.id());

    // Make the actor follow its target. When reached, register an attack
    if (creatureActor->navigateTo(target->position(), true, _range, dt)) {
        ctx.combat.addAttack(move(creatureActor), move(target), this);
    }
}

} // namespace game

} // namespace reone
