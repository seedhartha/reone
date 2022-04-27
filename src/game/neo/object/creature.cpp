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

unique_ptr<Creature> Creature::Loader::load(const resource::GffStruct &gitEntry) {
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
        throw ValidationException("Appearance 2DA not found");
    }
    auto modelType = appearanceTable->getString(appearanceType, "modeltype");
    auto race = appearanceTable->getString(appearanceType, "race");

    // Make scene node

    auto &scene = _services.scene.graphs.get(kSceneMain);
    auto model = _services.graphics.models.get(race);
    auto sceneNode = scene.newModel(move(model), ModelUsage::Creature, nullptr);

    if (modelType == "B") {
        auto normalHead = appearanceTable->getInt(appearanceType, "normalhead");
        auto backupHead = appearanceTable->getInt(appearanceType, "backuphead");
        auto headsTable = _services.resource.twoDas.get("heads");
        if (!headsTable) {
            throw ValidationException("Heads 2DA not found");
        }
        auto head = headsTable->getString(normalHead, "head");
        auto headModel = _services.graphics.models.get(head);
        if (headModel) {
            auto headSceneNode = scene.newModel(move(headModel), ModelUsage::Creature, nullptr);
            sceneNode->attach(kHeadHookNodeName, move(headSceneNode));
        }
    }

    auto transform = glm::translate(glm::vec3(xPosition, yPosition, zPosition));
    transform *= glm::rotate(-glm::atan(xOrientation, yOrientation), glm::vec3(0.0f, 0.0f, 1.0f));
    sceneNode->setLocalTransform(move(transform));

    // Make creature

    return Creature::Builder()
        .id(_idSeq.nextObjectId())
        .tag(move(tag))
        .sceneNode(move(sceneNode))
        .build();
}

} // namespace neo

} // namespace game

} // namespace reone