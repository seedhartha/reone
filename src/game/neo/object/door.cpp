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
#include "../../../scene/graph.h"
#include "../../../scene/node/model.h"

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

    auto utd = _resourceSvc.gffs.get(templateResRef, ResourceType::Utd);
    if (!utd) {
        throw ValidationException("UTD not found: " + templateResRef);
    }
    auto genericType = utd->getInt("GenericType");

    // From doortypes 2DA

    auto genericDoorsTable = _resourceSvc.twoDas.get("genericdoors");
    if (!genericDoorsTable) {
        throw ValidationException("genericdoors 2DA not found");
    }
    auto modelName = genericDoorsTable->getString(genericType, "modelname");

    // Make scene node

    shared_ptr<ModelSceneNode> sceneNode;

    auto model = _graphicsSvc.models.get(modelName);
    if (model) {
        sceneNode = _sceneGraph.newModel(move(model), ModelUsage::Door, nullptr);
    }

    // Make door

    auto door = Door::Builder()
                    .id(_idSeq.nextObjectId())
                    .tag(move(tag))
                    .sceneNode(move(sceneNode))
                    .sceneGraph(&_sceneGraph)
                    .build();

    door->setPosition(glm::vec3(x, y, z));
    door->setFacing(bearing);

    return move(door);
}

} // namespace neo

} // namespace game

} // namespace reone