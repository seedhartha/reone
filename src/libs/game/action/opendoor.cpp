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

#include "reone/game/action/opendoor.h"

#include "reone/game/game.h"
#include "reone/game/object/door.h"
#include "reone/game/object/factory.h"
#include "reone/game/script/runner.h"
#include "reone/game/services.h"

using namespace std;

namespace reone {

namespace game {

void OpenDoorAction::execute(shared_ptr<Action> self, Object &actor, float dt) {
    shared_ptr<Object> actorPtr(_game.objectFactory().getObjectById(actor.id()));
    auto creatureActor = dynamic_pointer_cast<Creature>(actorPtr);
    auto door = dynamic_pointer_cast<Door>(_object);

    bool reached = !creatureActor || creatureActor->navigateTo(door->position(), true, kDefaultMaxObjectDistance, dt);
    if (reached) {
        bool isObjectSelf = actorPtr == door;
        if (!isObjectSelf && door->isLocked()) {
            string onFailToOpen(door->getOnFailToOpen());
            if (!onFailToOpen.empty()) {
                _game.scriptRunner().run(onFailToOpen, door->id(), actor.id());
            }
        } else {
            door->open(actorPtr);
            if (!isObjectSelf) {
                string onOpen(door->getOnOpen());
                if (!onOpen.empty()) {
                    _game.scriptRunner().run(onOpen, door->id(), actor.id(), -1);
                }
            }
        }
        complete();
    }
}

} // namespace game

} // namespace reone
