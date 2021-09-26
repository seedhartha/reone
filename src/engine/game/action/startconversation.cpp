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

#include "startconversation.h"

#include "../game.h"

using namespace std;

namespace reone {

namespace game {

static constexpr float kMaxConversationDistance = 4.0f;

void StartConversationAction::execute(Object &actor, float dt) {
    shared_ptr<Object> actorPtr(_game.objectFactory().getObjectById(actor.id()));
    auto creatureActor = static_pointer_cast<Creature>(actorPtr);
    auto object = static_pointer_cast<SpatialObject>(_object);

    bool reached =
        !creatureActor ||
        _ignoreStartRange ||
        creatureActor->navigateTo(object->position(), true, kMaxConversationDistance, dt);

    if (reached) {
        bool isActorLeader = _game.party().getLeader() == actorPtr;
        _game.module()->area()->startDialog(isActorLeader ? object : static_pointer_cast<SpatialObject>(actorPtr), _dialogResRef);
        complete();
    }
}

} // namespace game

} // namespace reone
