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

#include "area.h"

#include "SDL2/SDL.h"

#include "../core/jobs.h"
#include "../core/log.h"
#include "../script/execution.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

static const float kKeepPathDuration = 1000.0f;

void Area::updateCreature(Creature &creature, float dt) {
    if (!creature.hasActions()) return;

    const Creature::Action &action = creature.currentAction();
    switch (action.type) {
        case Creature::ActionType::MoveToPoint:
        case Creature::ActionType::Follow: {
            SpatialObject *spatial = dynamic_cast<SpatialObject *>(action.object.get());
            glm::vec3 dest = (action.type == Creature::ActionType::Follow || action.object) ? spatial->position() : action.point;
            bool reached = navigateCreature(creature, dest, action.distance, dt);
            if (reached && action.type == Creature::ActionType::MoveToPoint) {
                creature.popCurrentAction();
            }
            break;
        }
        case Creature::ActionType::DoCommand: {
            ExecutionContext ctx(action.context);
            ctx.callerId = creature.id();

            ScriptExecution(action.context.savedState->program, move(ctx)).run();
            break;
        }
        case Creature::ActionType::StartConversation:
            if (_onStartDialog) {
                _onStartDialog(creature, action.resRef);
            }
            creature.popCurrentAction();
            break;
        default:
            warn("Area: action not implemented: " + to_string(static_cast<int>(action.type)));
            creature.popCurrentAction();
            break;
    }
}

bool Area::navigateCreature(Creature &creature, const glm::vec2 &dest, float distance, float dt) {
    glm::vec2 origin(creature.position());
    float distToDest = glm::distance2(origin, dest);

    if (distToDest <= distance) {
        creature.setMovementType(MovementType::None);
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

void Area::advanceCreatureOnPath(Creature &creature, float dt) {
    glm::vec2 origin(creature.position());
    shared_ptr<Creature::Path> path(creature.path());
    size_t pointCount = path->points.size();
    glm::vec2 dest;
    float distToDest;

    if (path->pointIdx == pointCount) {
        dest = path->destination;
        distToDest = glm::distance2(origin, dest);

    } else {
        const glm::vec2 &nextPoint = path->points[path->pointIdx];
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

    } else if (moveCreatureTowards(creature, dest, dt)) {
        creature.setMovementType(MovementType::Run);

    } else {
        creature.setMovementType(MovementType::None);
    }
}

void Area::selectNextPathPoint(Creature::Path &path) {
    size_t pointCount = path.points.size();
    if (path.pointIdx < pointCount) {
        path.pointIdx++;
    }
}

void Area::updateCreaturePath(Creature &creature, const glm::vec2 &dest) {
    if (creature.isPathUpdating()) return;

    creature.setPathUpdating();

    TheJobExecutor.enqueue([=, &creature](const atomic_bool &) {
        glm::vec2 origin(creature.position());
        vector<glm::vec2> points(_navMesh->findPath(origin, dest));
        uint32_t now = SDL_GetTicks();

        creature.setPath(dest, move(points), now);
    });
}

} // namespace game

} // namespace reone
