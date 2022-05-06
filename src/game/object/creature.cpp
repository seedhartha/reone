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

#include "creature.h"

#include "../../common/exception/validation.h"
#include "../../graphics/models.h"
#include "../../graphics/services.h"
#include "../../resource/2das.h"
#include "../../resource/gffs.h"
#include "../../resource/services.h"
#include "../../scene/collision.h"
#include "../../scene/graph.h"
#include "../../scene/node/model.h"

#include "../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kCreatureRunSpeed = 3.96f;

static const string kHeadHookNodeName = "headhook";

void Creature::loadFromGit(const Gff &git) {
    auto xPosition = git.getFloat("XPosition");
    auto yPosition = git.getFloat("YPosition");
    auto zPosition = git.getFloat("ZPosition");
    auto xOrientation = git.getFloat("XOrientation");
    auto yOrientation = git.getFloat("YOrientation");
    auto templateResRef = git.getString("TemplateResRef");

    _position = glm::vec3(xPosition, yPosition, zPosition);
    _facing = -glm::atan(xOrientation, yOrientation);

    loadFromUtc(templateResRef);
}

void Creature::loadFromUtc(const string &templateResRef) {
    // From UTC

    auto utc = _resourceSvc.gffs.get(templateResRef, ResourceType::Utc);
    if (!utc) {
        throw ValidationException("UTC not found: " + templateResRef);
    }
    auto tag = utc->getString("Tag");
    auto appearanceType = utc->getInt("Appearance_Type");

    // From appearance 2DA

    auto appearanceTable = _resourceSvc.twoDas.get("appearance");
    if (!appearanceTable) {
        throw ValidationException("appearance 2DA not found");
    }
    auto modelType = appearanceTable->getString(appearanceType, "modeltype");
    auto race = appearanceTable->getString(appearanceType, "race");

    // Make scene node

    shared_ptr<ModelSceneNode> sceneNode;
    auto model = _graphicsSvc.models.get(race);
    if (model) {
        sceneNode = _sceneGraph->newModel(*model, ModelUsage::Creature);
        if (modelType == "B") {
            auto normalHead = appearanceTable->getInt(appearanceType, "normalhead");
            auto backupHead = appearanceTable->getInt(appearanceType, "backuphead");
            auto headsTable = _resourceSvc.twoDas.get("heads");
            if (!headsTable) {
                throw ValidationException("heads 2DA not found");
            }
            auto head = headsTable->getString(normalHead, "head");
            auto headModel = _graphicsSvc.models.get(head);
            if (headModel) {
                auto headSceneNode = _sceneGraph->newModel(*headModel, ModelUsage::Creature);
                sceneNode->attach(kHeadHookNodeName, *headSceneNode);
            }
        }
        sceneNode->setUser(*this);
        sceneNode->setCullable(true);
        sceneNode->setPickable(true);
    }

    //

    _tag = move(tag);
    _sceneNode = sceneNode.get();

    flushTransform();
}

bool Creature::moveForward(float delta) {
    auto dir = glm::vec2(-glm::sin(_facing), glm::cos(_facing));
    auto newPosition = glm::vec2(_position) + delta * kCreatureRunSpeed * dir;

    Collision collision;
    if (!_sceneGraph->testElevation(glm::vec2(newPosition), collision)) {
        return false;
    }

    _position = glm::vec3(newPosition, collision.intersection.z);
    flushTransform();
    return true;
}

void Creature::update(float delta) {
    if (_sceneNode) {
        auto &modelSceneNode = static_cast<ModelSceneNode &>(*_sceneNode);
        if (_state == State::Pause && modelSceneNode.getActiveAnimationName() != "pause1") {
            modelSceneNode.playAnimation("pause1", AnimationProperties::fromFlags(AnimationFlags::loop | AnimationFlags::propagate));
        } else if (_state == State::Walk && modelSceneNode.getActiveAnimationName() != "walk") {
            modelSceneNode.playAnimation("walk", AnimationProperties::fromFlags(AnimationFlags::loop | AnimationFlags::propagate));
        } else if (_state == State::Run && modelSceneNode.getActiveAnimationName() != "run") {
            modelSceneNode.playAnimation("run", AnimationProperties::fromFlags(AnimationFlags::loop | AnimationFlags::propagate));
        }
    }
}

} // namespace game

} // namespace reone