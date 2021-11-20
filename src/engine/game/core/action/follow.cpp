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

#include "follow.h"

#include "../game.h"
#include "../object/factory.h"
#include "../services.h"

using namespace std;

namespace reone {

namespace game {

void FollowAction::execute(Object &actor, float dt) {
    auto creatureActor = _game.objectFactory().getObjectById<Creature>(actor.id());
    auto object = static_pointer_cast<SpatialObject>(_object);
    glm::vec3 dest(object->position());
    float distance2 = creatureActor->getDistanceTo2(glm::vec2(dest));
    bool run = distance2 > kDistanceWalk * kDistanceWalk;

    creatureActor->navigateTo(dest, run, _distance, dt);
}

} // namespace game

} // namespace reone
