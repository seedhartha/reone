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

#include "placeable.h"

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

unique_ptr<Placeable> Placeable::Loader::load(const GffStruct &gitEntry) {
    // From GIT entry

    auto templateResRef = gitEntry.getString("TemplateResRef");
    auto x = gitEntry.getFloat("X");
    auto y = gitEntry.getFloat("Y");
    auto z = gitEntry.getFloat("Z");
    auto bearing = gitEntry.getFloat("Bearing");

    // From UTP

    auto utp = _services.resource.gffs.get(templateResRef, ResourceType::Utp);
    if (!utp) {
        throw ValidationException("UTP not found: " + templateResRef);
    }
    auto tag = utp->getString("Tag");
    auto appearance = utp->getInt("Appearance");

    // From placeables 2DA

    auto placeablesTable = _services.resource.twoDas.get("placeables");
    if (!placeablesTable) {
        throw ValidationException("placeables 2DA not found");
    }
    auto modelName = placeablesTable->getString(appearance, "modelname");

    // Make scene node

    shared_ptr<ModelSceneNode> sceneNode;

    auto model = _services.graphics.models.get(modelName);
    if (model) {
        auto &scene = _services.scene.graphs.get(kSceneMain);
        sceneNode = scene.newModel(move(model), ModelUsage::Placeable, nullptr);
    }

    // Make placeable

    auto placeable = Placeable::Builder()
        .id(_idSeq.nextObjectId())
        .tag(move(tag))
        .sceneNode(move(sceneNode))
        .build();

    placeable->setPosition(glm::vec3(x, y, z));
    placeable->setFacing(bearing);

    return move(placeable);
}

} // namespace neo

} // namespace game

} // namespace reone