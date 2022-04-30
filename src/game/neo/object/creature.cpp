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

#include "../../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

static constexpr float kCreatureMoveSpeed = 3.96f;

static const string kHeadHookNodeName = "headhook";

bool Creature::moveForward(float delta) {
    auto dir = glm::vec2(-glm::sin(_facing), glm::cos(_facing));
    auto newPosition = glm::vec2(_position) + delta * kCreatureMoveSpeed * dir;

    Collision collision;
    if (!_sceneGraph.testElevation(glm::vec2(newPosition), collision)) {
        return false;
    }

    _position = glm::vec3(newPosition, collision.intersection.z);
    flushTransform();
    return true;
}

unique_ptr<Creature> Creature::Loader::load(const Gff &gitEntry) {
    // From GIT entry

    auto xPosition = gitEntry.getFloat("XPosition");
    auto yPosition = gitEntry.getFloat("YPosition");
    auto zPosition = gitEntry.getFloat("ZPosition");
    auto xOrientation = gitEntry.getFloat("XOrientation");
    auto yOrientation = gitEntry.getFloat("YOrientation");
    auto templateResRef = gitEntry.getString("TemplateResRef");

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
        sceneNode = _sceneGraph.newModel(move(model), ModelUsage::Creature, nullptr);

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
                auto headSceneNode = _sceneGraph.newModel(move(headModel), ModelUsage::Creature, nullptr);
                sceneNode->attach(kHeadHookNodeName, move(headSceneNode));
            }
        }

        sceneNode->setCullable(true);
        sceneNode->setPickable(true);
    }

    // Make creature

    auto creature = Creature::Builder()
                        .id(_idSeq.nextObjectId())
                        .tag(move(tag))
                        .sceneNode(move(sceneNode))
                        .sceneGraph(&_sceneGraph)
                        .build();

    creature->setPosition(glm::vec3(xPosition, yPosition, zPosition));
    creature->setFacing(-glm::atan(xOrientation, yOrientation));

    return move(creature);
}

} // namespace neo

} // namespace game

} // namespace reone