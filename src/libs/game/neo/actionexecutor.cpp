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
#include "reone/system/checkutil.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

static constexpr float kPlayerMoveRate = 4.0f;

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
    glm::vec3 delta = action.location.position - subject.position();
    glm::vec2 delta2D = delta;
    auto distance2D = glm::length(delta2D);
    if (distance2D < 1.0f) {
        subject.setMoveType(Creature::MoveType::None);
        return true;
    }
    glm::vec3 dir = glm::normalize(delta);
    glm::vec2 dir2D = delta2D / distance2D;
    glm::vec2 oldPos2D = subject.position();
    glm::vec2 newPos2D = oldPos2D + kPlayerMoveRate * dir2D * dt;
    subject.setFacingPoint(action.location.position);

    std::vector<ObjectWalkmesh> walkmeshes;
    auto &area = _module->get().area();
    for (const auto &room : area.rooms()) {
        auto walkmesh = _resourceSvc.walkmeshes.get(room.model, ResType::Wok);
        if (walkmesh) {
            walkmeshes.emplace_back(area.id(), *walkmesh);
        }
    }
    for (const auto &door : area.doors()) {
        auto walkmeshResRef = door.get().modelName().value();
        DoorWalkmeshType walkmeshType;
        if (door.get().isClosed()) {
            walkmeshResRef += "0";
            walkmeshType = DoorWalkmeshType::Closed;
        } else {
            walkmeshResRef += "1";
            walkmeshType = DoorWalkmeshType::Open1;
        }
        auto walkmesh = _resourceSvc.walkmeshes.get(walkmeshResRef, ResType::Dwk);
        if (walkmesh) {
            walkmeshes.emplace_back(door.get().id(), *walkmesh, walkmeshType);
        }
    }
    for (const auto &placeable : area.placeables()) {
        auto walkmeshResRef = placeable.get().modelName().value();
        auto walkmesh = _resourceSvc.walkmeshes.get(walkmeshResRef, ResType::Pwk);
        if (walkmesh) {
            walkmeshes.emplace_back(placeable.get().id(), *walkmesh);
        }
    }

    // TODO: extract method
    // First pass: Z coord and floor obstructions
    std::optional<float> zCoord;
    for (const auto &ow : walkmeshes) {
        const auto &object = _module->get().objectById(ow.objectId)->get();
        const auto &walkmesh = ow.walkmesh;
        glm::mat4 toLocal {1.0f};
        if (object.type() == ObjectType::Area) {
            if (!walkmesh.contains(oldPos2D) &&
                !walkmesh.contains(newPos2D)) {
                continue;
            }
        } else {
            const auto &spatial = static_cast<const SpatialObject &>(object);
            float distance = glm::distance(subject.position(), spatial.position());
            if (distance > 4.0f) {
                continue;
            }
            toLocal = glm::inverse(glm::rotate(
                glm::translate(spatial.position()),
                spatial.facing(),
                glm::vec3 {0.0f, 0.0f, 1.0f}));
        }
        glm::vec3 originLocal = toLocal * glm::vec4 {newPos2D, 1000.0f, 1.0f};
        glm::vec3 dirLocal = toLocal * glm::vec4 {0.0f, 0.0f, -1.0f, 0.0f};
        float distance;
        auto face = walkmesh.raycast(
            _walkcheckSurfaceMaterials,
            originLocal,
            dirLocal,
            std::numeric_limits<float>::max(),
            distance);
        if (face) {
            bool walkable = _walkSurfaceMaterials.count(face->material);
            if (!walkable) {
                subject.setMoveType(Creature::MoveType::None);
                return false;
            }
            zCoord = 1000.0f - distance;
        }
    }
    if (!zCoord) {
        return false;
    }
    // Second pass: wall obstructions
    for (const auto &ow : walkmeshes) {
        const auto &object = _module->get().objectById(ow.objectId)->get();
        const auto &walkmesh = ow.walkmesh;
        glm::mat4 toLocal {1.0f};
        if (object.type() == ObjectType::Area) {
            if (!walkmesh.contains(oldPos2D) &&
                !walkmesh.contains(newPos2D)) {
                continue;
            }
        } else {
            const auto &spatial = static_cast<const SpatialObject &>(object);
            float distance = glm::distance(subject.position(), spatial.position());
            if (distance > 4.0f) {
                continue;
            }
            toLocal = glm::inverse(glm::rotate(
                glm::translate(spatial.position()),
                spatial.facing(),
                glm::vec3 {0.0f, 0.0f, 1.0f}));
        }
        glm::vec3 originLocal = toLocal * glm::vec4 {subject.position(), 1.0f};
        glm::vec3 dirLocal = toLocal * glm::vec4 {dir, 0.0f};
        float distance;
        auto face = walkmesh.raycast(
            _walkcheckSurfaceMaterials,
            originLocal + glm::vec3 {0.0f, 0.0f, 0.5f},
            dirLocal,
            kPlayerMoveRate * dt,
            distance);
        if (face && _walkSurfaceMaterials.count(face->material) == 0) {
            subject.setMoveType(Creature::MoveType::None);
            return false;
        }
    }
    subject.setMoveType(Creature::MoveType::Run);
    subject.setPosition(glm::vec3 {newPos2D, *zCoord});
    return false;
}

} // namespace neo

} // namespace game

} // namespace reone
