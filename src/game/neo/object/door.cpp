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

#include "door.h"

#include "../../../common/exception/validation.h"
#include "../../../graphics/models.h"
#include "../../../graphics/services.h"
#include "../../../resource/2das.h"
#include "../../../resource/gffs.h"
#include "../../../resource/gffstruct.h"
#include "../../../resource/services.h"
#include "../../../scene/graphs.h"
#include "../../../scene/node/model.h"
#include "../../../scene/services.h"

#include "../../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Door> Door::Loader::load(const GffStruct &gitEntry) {
    // From GIT entry

    auto templateResRef = gitEntry.getString("TemplateResRef");
    auto tag = gitEntry.getString("Tag");
    auto x = gitEntry.getFloat("X");
    auto y = gitEntry.getFloat("Y");
    auto z = gitEntry.getFloat("Z");
    auto bearing = gitEntry.getFloat("Bearing");

    // From UTD

    auto utd = _services.resource.gffs.get(templateResRef, ResourceType::Utd);
    if (!utd) {
        throw ValidationException("UTD not found: " + templateResRef);
    }
    auto genericType = utd->getInt("GenericType");

    // From doortypes 2DA

    auto genericDoorsTable = _services.resource.twoDas.get("genericdoors");
    if (!genericDoorsTable) {
        throw ValidationException("genericdoors 2DA not found");
    }
    auto modelName = genericDoorsTable->getString(genericType, "modelname");

    // Make scene node

    shared_ptr<ModelSceneNode> sceneNode;

    auto model = _services.graphics.models.get(modelName);
    if (model) {
        auto &scene = _services.scene.graphs.get(kSceneMain);
        sceneNode = scene.newModel(move(model), ModelUsage::Door, nullptr);

        auto transform = glm::translate(glm::vec3(x, y, z));
        transform *= glm::rotate(bearing, glm::vec3(0.0f, 0.0f, 1.0f));
        sceneNode->setLocalTransform(move(transform));
    }

    // Make door

    return Door::Builder()
        .id(_idSeq.nextObjectId())
        .tag(move(tag))
        .sceneNode(move(sceneNode))
        .build();
}

} // namespace neo

} // namespace game

} // namespace reone