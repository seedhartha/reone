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

#include "reone/game/action/startconversation.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/factory.h"
#include "reone/game/party.h"

namespace reone {

namespace game {

static constexpr float kMaxConversationDistance = 4.0f;

void StartConversationAction::execute(std::shared_ptr<Action> self, Object &actor, float dt) {
    auto actorPtr = _game.getObjectById(actor.id());
    auto creatureActor = std::static_pointer_cast<Creature>(actorPtr);

    bool reached =
        !creatureActor ||
        _ignoreStartRange ||
        creatureActor->navigateTo(_objectToConverse->position(), true, kMaxConversationDistance, dt);

    if (reached) {
        bool isActorLeader = _game.party().getLeader() == actorPtr;
        _game.module()->area()->startDialog(isActorLeader ? _objectToConverse : actorPtr, _dialogResRef);
        complete();
    }
}

} // namespace game

} // namespace reone
