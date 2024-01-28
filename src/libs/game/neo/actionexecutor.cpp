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

#include "reone/game/neo/actionexecutor.h"

#include "reone/game/neo/action.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/module.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/objectfactory.h"
#include "reone/graphics/walkmesh.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/walkmeshes.h"
#include "reone/scene/collision.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/system/checkutil.h"

using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

static constexpr float kPlayerMoveRate = 4.0f;
static constexpr float kMinMoveDistance = 0.1f;

bool ActionExecutor::executeAction(Object &subject, const Action &action, float dt) {
    checkThat(static_cast<bool>(_module), "Module must be present");
    if (subject.type() == ObjectType::Creature) {
        auto &creature = static_cast<Creature &>(subject);
        if (action.type == ActionType::MoveToPoint) {
            return executeMoveToPoint(creature, action, dt);
        }
    }
    return false;
}

bool ActionExecutor::executeMoveToPoint(Creature &subject, const Action &action, float dt) {
    auto oldPos = subject.position();
    auto delta = action.location.position - oldPos;
    auto distance = glm::length(delta);
    if (distance < kMinMoveDistance) {
        subject.setMoveType(Creature::MoveType::None);
        return true;
    }
    auto dir = glm::normalize(delta);
    auto newPos = oldPos + kPlayerMoveRate * dir * dt;
    subject.setFacingPoint(action.location.position);

    bool canMove = true;
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    _sceneLock.lock();
    Collision collision;
    if (scene.testWalk(oldPos, newPos, nullptr, collision)) {
        canMove = false;
    }
    if (!scene.testElevation(newPos, collision)) {
        canMove = false;
    }
    _sceneLock.unlock();

    if (canMove) {
        subject.setMoveType(Creature::MoveType::Run);
        subject.setPosition(collision.intersection);
    } else {
        subject.setMoveType(Creature::MoveType::None);
    }
    return false;
}

} // namespace neo

} // namespace game

} // namespace reone
