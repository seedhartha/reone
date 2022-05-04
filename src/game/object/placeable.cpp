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

#include "../../common/exception/validation.h"
#include "../../graphics/models.h"
#include "../../graphics/services.h"
#include "../../graphics/walkmeshes.h"
#include "../../resource/2das.h"
#include "../../resource/gffs.h"
#include "../../resource/gff.h"
#include "../../resource/services.h"
#include "../../scene/graph.h"
#include "../../scene/node/model.h"
#include "../../scene/node/walkmesh.h"

#include "../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Placeable::loadFromGit(const Gff &git) {
    // From GIT

    auto templateResRef = git.getString("TemplateResRef");
    auto x = git.getFloat("X");
    auto y = git.getFloat("Y");
    auto z = git.getFloat("Z");
    auto bearing = git.getFloat("Bearing");

    // From UTP

    auto utp = _resourceSvc.gffs.get(templateResRef, ResourceType::Utp);
    if (!utp) {
        throw ValidationException("UTP not found: " + templateResRef);
    }
    auto tag = utp->getString("Tag");
    auto appearance = utp->getInt("Appearance");

    // From placeables 2DA

    auto placeablesTable = _resourceSvc.twoDas.get("placeables");
    if (!placeablesTable) {
        throw ValidationException("placeables 2DA not found");
    }
    auto modelName = placeablesTable->getString(appearance, "modelname");

    // Make scene nodes

    shared_ptr<ModelSceneNode> sceneNode;
    auto model = _graphicsSvc.models.get(modelName);
    if (model) {
        sceneNode = _sceneGraph->newModel(*model, ModelUsage::Placeable);
        sceneNode->setUser(*this);
        sceneNode->setCullable(true);
        sceneNode->setPickable(true);
    }

    shared_ptr<WalkmeshSceneNode> walkmeshSceneNode;
    auto walkmesh = _graphicsSvc.walkmeshes.get(modelName, ResourceType::Pwk);
    if (walkmesh) {
        walkmeshSceneNode = _sceneGraph->newWalkmesh(*walkmesh);
        walkmeshSceneNode->setUser(*this);
    }

    //

    _tag = move(tag);
    _position = glm::vec3(x, y, z);
    _facing = bearing;
    _sceneNode = move(sceneNode);
    _walkmesh = move(walkmeshSceneNode);

    flushTransform();
}

void Placeable::flushTransform() {
    Object::flushTransform();

    auto transform = glm::translate(_position);
    transform *= glm::rotate(_facing, glm::vec3(0.0f, 0.0f, 1.0f));
    transform *= glm::rotate(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    if (_walkmesh) {
        _walkmesh->setLocalTransform(_sceneNode->localTransform());
    }
}

} // namespace game

} // namespace reone