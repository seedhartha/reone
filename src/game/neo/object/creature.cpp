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
#include "../../scene/graphs.h"
#include "../../scene/node/model.h"
#include "../../scene/services.h"

#include "../../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

static const string kHeadHookNodeName = "headhook";

bool Creature::moveForward(float delta) {
    auto dir = glm::vec2(-glm::sin(_facing), glm::cos(_facing));
    _position += delta * 5.0f * glm::vec3(dir, 0.0f);
    flushTransform();
    return true;
}

unique_ptr<Creature> Creature::Loader::load(const GffStruct &gitEntry) {
    // From GIT entry

    auto xPosition = gitEntry.getFloat("XPosition");
    auto yPosition = gitEntry.getFloat("YPosition");
    auto zPosition = gitEntry.getFloat("ZPosition");
    auto xOrientation = gitEntry.getFloat("XOrientation");
    auto yOrientation = gitEntry.getFloat("YOrientation");
    auto templateResRef = gitEntry.getString("TemplateResRef");

    // From UTC

    auto utc = _services.resource.gffs.get(templateResRef, ResourceType::Utc);
    if (!utc) {
        throw ValidationException("UTC not found: " + templateResRef);
    }
    auto tag = utc->getString("Tag");
    auto appearanceType = utc->getInt("Appearance_Type");

    // From appearance 2DA

    auto appearanceTable = _services.resource.twoDas.get("appearance");
    if (!appearanceTable) {
        throw ValidationException("appearance 2DA not found");
    }
    auto modelType = appearanceTable->getString(appearanceType, "modeltype");
    auto race = appearanceTable->getString(appearanceType, "race");

    // Make scene node

    shared_ptr<ModelSceneNode> sceneNode;

    auto model = _services.graphics.models.get(race);
    if (model) {
        auto &scene = _services.scene.graphs.get(kSceneMain);
        sceneNode = scene.newModel(move(model), ModelUsage::Creature, nullptr);

        if (modelType == "B") {
            auto normalHead = appearanceTable->getInt(appearanceType, "normalhead");
            auto backupHead = appearanceTable->getInt(appearanceType, "backuphead");
            auto headsTable = _services.resource.twoDas.get("heads");
            if (!headsTable) {
                throw ValidationException("heads 2DA not found");
            }
            auto head = headsTable->getString(normalHead, "head");
            auto headModel = _services.graphics.models.get(head);
            if (headModel) {
                auto headSceneNode = scene.newModel(move(headModel), ModelUsage::Creature, nullptr);
                sceneNode->attach(kHeadHookNodeName, move(headSceneNode));
            }
        }
    }

    // Make creature

    auto creature = Creature::Builder()
        .id(_idSeq.nextObjectId())
        .tag(move(tag))
        .sceneNode(move(sceneNode))
        .build();

    creature->setPosition(glm::vec3(xPosition, yPosition, zPosition));
    creature->setFacing(-glm::atan(xOrientation, yOrientation));

    return move(creature);
}

} // namespace neo

} // namespace game

} // namespace reone