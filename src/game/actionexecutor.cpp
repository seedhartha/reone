/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "actionexecutor.h"

#include <stdexcept>

#include "SDL2/SDL_timer.h"

#include "../core/log.h"
#include "../script/execution.h"

#include "object/area.h"
#include "object/creature.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

static const float kKeepPathDuration = 1000.0f;

ActionExecutor::ActionExecutor(Area *area) : _area(area) {
    if (!area) {
        throw invalid_argument("area must not be null");
    }
}

void ActionExecutor::executeActions(Creature &creature, float dt) {
    ActionQueue &actionQueue = creature.actionQueue();

    Action *action = actionQueue.currentAction();
    if (!action) return;

    ActionType type = action->type();
    switch (type) {
        case ActionType::MoveToPoint:
            executeMoveToPoint(creature, *dynamic_cast<MoveToPointAction *>(action), dt);
            break;
        case ActionType::MoveToObject:
            executeMoveToObject(creature, *dynamic_cast<MoveToObjectAction *>(action), dt);
            break;
        case ActionType::Follow:
            executeFollow(creature, *dynamic_cast<FollowAction *>(action), dt);
            break;
        case ActionType::DoCommand:
            executeDoCommand(creature, *dynamic_cast<CommandAction *>(action), dt);
            break;
        case ActionType::StartConversation:
            executeStartConversation(creature, *dynamic_cast<StartConversationAction *>(action), dt);
            break;
        default:
            warn("Area: action not implemented: " + to_string(static_cast<int>(type)));
            action->isCompleted();
            break;
    }
}

void ActionExecutor::executeMoveToPoint(Creature &creature, MoveToPointAction &action, float dt) {
    glm::vec3 dest(action.point());

    bool reached = navigateCreature(creature, dest, 1.0f, dt);
    if (reached) {
        action.complete();
    }
}

void ActionExecutor::executeMoveToObject(Creature &creature, MoveToObjectAction &action, float dt) {
    const SpatialObject *object = dynamic_cast<const SpatialObject *>(action.object());
    glm::vec3 dest(object->position());
    float distance = action.distance();

    bool reached = navigateCreature(creature, dest, distance, dt);
    if (reached) {
        action.complete();
    }
}

void ActionExecutor::executeFollow(Creature &creature, FollowAction &action, float dt) {
    const SpatialObject *object = dynamic_cast<const SpatialObject *>(action.object());
    glm::vec3 dest(object->position());
    float distance = action.distance();

    navigateCreature(creature, dest, distance, dt);
}

void ActionExecutor::executeDoCommand(Creature &creature, CommandAction &action, float dt) {
    ExecutionContext ctx(action.context());
    ctx.callerId = creature.id();

    ScriptExecution(ctx.savedState->program, move(ctx)).run();
    action.complete();
}

void ActionExecutor::executeStartConversation(Creature &creature, StartConversationAction &action, float dt) {
    _area->startDialog(creature, action.dialogResRef());
    action.complete();
}

bool ActionExecutor::navigateCreature(Creature &creature, const glm::vec3 &dest, float distance, float dt) {
    const glm::vec3 &origin = creature.position();
    float distToDest = glm::distance2(origin, dest);

    if (distToDest <= distance) {
        creature.setMovementType(MovementType::None);
        creature.clearPath();
        return true;
    }
    bool updatePath = true;
    shared_ptr<Creature::Path> path(creature.path());

    if (path) {
        uint32_t now = SDL_GetTicks();
        if (path->destination == dest || now - path->timeFound <= kKeepPathDuration) {
            advanceCreatureOnPath(creature, dt);
            updatePath = false;
        }
    }
    if (updatePath) {
        updateCreaturePath(creature, dest);
    }

    return false;
}

void ActionExecutor::advanceCreatureOnPath(Creature &creature, float dt) {
    const glm::vec3 &origin = creature.position();
    shared_ptr<Creature::Path> path(creature.path());
    size_t pointCount = path->points.size();
    glm::vec3 dest;
    float distToDest;

    if (path->pointIdx == pointCount) {
        dest = path->destination;
        distToDest = glm::distance2(origin, dest);

    } else {
        const glm::vec3 &nextPoint = path->points[path->pointIdx];
        float distToNextPoint = glm::distance2(origin, nextPoint);
        float distToPathDest = glm::distance2(origin, path->destination);

        if (distToPathDest < distToNextPoint) {
            dest = path->destination;
            distToDest = distToPathDest;
            path->pointIdx = static_cast<int>(pointCount);

        } else {
            dest = nextPoint;
            distToDest = distToNextPoint;
        }
    }

    if (distToDest <= 1.0f) {
        selectNextPathPoint(*path);

    } else if (_area->moveCreatureTowards(creature, dest, true, dt)) {
        creature.setMovementType(MovementType::Run);

    } else {
        creature.setMovementType(MovementType::None);
    }
}

void ActionExecutor::selectNextPathPoint(Creature::Path &path) {
    size_t pointCount = path.points.size();
    if (path.pointIdx < pointCount) {
        path.pointIdx++;
    }
}

void ActionExecutor::updateCreaturePath(Creature &creature, const glm::vec3 &dest) {
    const glm::vec3 &origin = creature.position();
    vector<glm::vec3> points(_area->pathfinder().findPath(origin, dest));
    uint32_t now = SDL_GetTicks();

    creature.setPath(dest, move(points), now);
}

} // namespace game

} // namespace reone
