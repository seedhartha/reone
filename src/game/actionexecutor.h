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

#pragma once

#include <boost/noncopyable.hpp>

#include "glm/vec3.hpp"

#include "action/attack.h"
#include "action/commandaction.h"
#include "action/follow.h"
#include "action/locationaction.h"
#include "action/movetolocation.h"
#include "action/movetoobject.h"
#include "action/movetopoint.h"
#include "action/playanimation.h"
#include "action/startconversation.h"
#include "object/creature.h"

namespace reone {

namespace game {

class Game;

class ActionExecutor : boost::noncopyable {
public:
    ActionExecutor(Game *game);

    void executeActions(const std::shared_ptr<Object> &object, float dt);

private:
    Game *_game;

    bool navigateCreature(const std::shared_ptr<Creature> &creature, const glm::vec3 &dest, bool run, float distance, float dt);
    void advanceCreatureOnPath(const std::shared_ptr<Creature> &creature, bool run, float dt);
    void selectNextPathPoint(Creature::Path &path);
    void updateCreaturePath(const std::shared_ptr<Creature> &creature, const glm::vec3 &dest);

    // Actions

    void executeMoveToPoint(const std::shared_ptr<Object> &actor, MoveToPointAction &action, float dt);
    void executeMoveToObject(const std::shared_ptr<Object> &actor, MoveToObjectAction &action, float dt);
    void executeFollow(const std::shared_ptr<Object> &actor, FollowAction &action, float dt);
    void executeDoCommand(const std::shared_ptr<Object> &actor, CommandAction &command, float dt);
    void executeStartConversation(const std::shared_ptr<Object> &actor, StartConversationAction &action, float dt);
    void executeAttack(const std::shared_ptr<Object> &actor, AttackAction &action, float dt);
    void executeOpenDoor(const std::shared_ptr<Object> &actor, ObjectAction &action, float dt);
    void executeCloseDoor(const std::shared_ptr<Object> &actor, ObjectAction &action, float dt);
    void executeOpenContainer(const std::shared_ptr<Object> &actor, ObjectAction &action, float dt);
    void executeOpenLock(const std::shared_ptr<Object> &actor, ObjectAction &action, float dt);
    void executeJumpToObject(const std::shared_ptr<Object> &actor, ObjectAction &action, float dt);
    void executeJumpToLocation(const std::shared_ptr<Object> &actor, LocationAction &action, float dt);
    void executePlayAnimation(const std::shared_ptr<Object> &actor, const std::shared_ptr<PlayAnimationAction> &action, float dt);
    void executeFollowLeader(const std::shared_ptr<Object> &actor, Action &action, float dt);
    void executeMoveToLocation(const std::shared_ptr<Object> &actor, MoveToLocationAction &action, float dt);

    // END Actions
};

} // namespace game

} // namespace reone
