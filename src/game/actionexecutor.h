/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#pragma once

#include "glm/vec3.hpp"

#include "action/commandaction.h"
#include "action/follow.h"
#include "action/locationaction.h"
#include "action/movetoobject.h"
#include "action/movetopoint.h"
#include "action/startconversation.h"
#include "action/attack.h"
#include "object/creature.h"

namespace reone {

namespace game {

class Game;

class ActionExecutor {
public:
    ActionExecutor(Game *game);

    void executeActions(Object &object, float dt);

private:
    Game *_game { nullptr };

    ActionExecutor(const ActionExecutor &) = delete;
    ActionExecutor &operator=(const ActionExecutor &) = delete;

    bool navigateCreature(Creature &creature, const glm::vec3 &dest, bool run, float distance, float dt);
    void advanceCreatureOnPath(Creature &creature, bool run, float dt);
    void selectNextPathPoint(Creature::Path &path);
    void updateCreaturePath(Creature &creature, const glm::vec3 &dest);

    // Actions

    void executeMoveToPoint(Creature &actor, MoveToPointAction &action, float dt);
    void executeMoveToObject(Creature &actor, MoveToObjectAction &action, float dt);
    void executeFollow(Creature &actor, FollowAction &action, float dt);
    void executeDoCommand(Object &actor, CommandAction &command, float dt);
    void executeStartConversation(Object &actor, StartConversationAction &action, float dt);
    void executeAttack(Creature &actor, AttackAction &action, float dt);
    void executeOpenDoor(Object &actor, ObjectAction &action, float dt);
    void executeCloseDoor(Object &actor, ObjectAction &action, float dt);
    void executeOpenContainer(Creature &actor, ObjectAction &action, float dt);
    void executeOpenLock(Creature &actor, ObjectAction &action, float dt);
    void executeJumpToObject(Object &actor, ObjectAction &action, float dt);
    void executeJumpToLocation(Object &actor, LocationAction &action, float dt);

    // END Actions
};

} // namespace game

} // namespace reone
