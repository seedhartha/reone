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

#include "reone/game/action/useskill.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/creature.h"
#include "reone/game/script/runner.h"
#include "reone/system/logutil.h"

namespace reone {

namespace game {

void UseSkillAction::execute(std::shared_ptr<Action> self, Object &actor, float dt) {
    if (_skill == SkillType::Security) {
        if (!_target || _target->type() != ObjectType::Door) {
            warn("ActionExecutor: unsupported OpenLock object: " + std::to_string(_target->id()));
            complete();
            return;
        }

        auto door = std::static_pointer_cast<Door>(_target);
        auto creatureActor = _game.getObjectById<Creature>(actor.id());

        bool reached = creatureActor->navigateTo(door->position(), true, kDefaultMaxObjectDistance, dt);
        if (reached) {
            creatureActor->face(*door);
            creatureActor->playAnimation(AnimationType::LoopingUnlockDoor);

            if (!door->isKeyRequired()) {
                door->setLocked(false);
                door->open(creatureActor);

                std::string onOpen(door->getOnOpen());
                if (!onOpen.empty()) {
                    _game.scriptRunner().run(onOpen, door->id(), actor.id());
                }
            }

            complete();
        }
    } else {
        complete();
    }
}

} // namespace game

} // namespace reone
