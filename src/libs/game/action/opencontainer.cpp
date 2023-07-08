/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/action/opencontainer.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/placeable.h"

namespace reone {

namespace game {

void OpenContainerAction::execute(std::shared_ptr<Action> self, Object &actor, float dt) {
    auto creatureActor = _game.getObjectById<Creature>(actor.id());
    auto placeable = std::static_pointer_cast<Placeable>(_object);
    bool reached = creatureActor->navigateTo(placeable->position(), true, kDefaultMaxObjectDistance, dt);
    if (reached) {
        _game.openContainer(placeable);
        complete();
    }
}

} // namespace game

} // namespace reone
