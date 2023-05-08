/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/common/exception/validation.h"
#include "reone/graphics/models.h"
#include "reone/graphics/services.h"
#include "reone/graphics/walkmeshes.h"
#include "reone/resource/2das.h"
#include "reone/resource/gff.h"
#include "reone/resource/gffs.h"
#include "reone/resource/services.h"
#include "reone/resource/strings.h"
#include "../../scene/graph.h"
#include "../../scene/node/model.h"

#include "../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Door::loadFromGit(const Gff &git) {
    // From GIT

    auto templateResRef = git.getString("TemplateResRef");
    auto tag = git.getString("Tag");
    auto x = git.getFloat("X");
    auto y = git.getFloat("Y");
    auto z = git.getFloat("Z");
    auto bearing = git.getFloat("Bearing");

    // From UTD

    auto utd = _resourceSvc.gffs.get(templateResRef, ResourceType::Utd);
    if (!utd) {
        throw ValidationException("UTD not found: " + templateResRef);
    }
    auto genericType = utd->getInt("GenericType");
    auto locName = _resourceSvc.strings.get(utd->getInt("LocName"));

    // From doortypes 2DA

    auto genericDoorsTable = _resourceSvc.twoDas.get("genericdoors");
    if (!genericDoorsTable) {
        throw ValidationException("genericdoors 2DA not found");
    }
    auto modelName = genericDoorsTable->getString(genericType, "modelname");

    // Make scene nodes

    shared_ptr<ModelSceneNode> sceneNode;
    auto model = _graphicsSvc.models.get(modelName);
    if (model) {
        sceneNode = _sceneGraph->newModel(*model, ModelUsage::Door);
        sceneNode->init();
        sceneNode->setUser(*this);
        sceneNode->setCullable(true);
        sceneNode->setPickable(true);
    }

    shared_ptr<WalkmeshSceneNode> walkmeshClosedSceneNode;
    auto walkmeshClosed = _graphicsSvc.walkmeshes.get(modelName + "0", ResourceType::Dwk);
    if (walkmeshClosed) {
        walkmeshClosedSceneNode = _sceneGraph->newWalkmesh(*walkmeshClosed);
        walkmeshClosedSceneNode->setUser(*this);
    }

    shared_ptr<WalkmeshSceneNode> walkmeshOpen1SceneNode;
    auto walkmeshOpen1 = _graphicsSvc.walkmeshes.get(modelName + "1", ResourceType::Dwk);
    if (walkmeshOpen1) {
        walkmeshOpen1SceneNode = _sceneGraph->newWalkmesh(*walkmeshOpen1);
        walkmeshOpen1SceneNode->setUser(*this);
        walkmeshOpen1SceneNode->setEnabled(false);
    }

    shared_ptr<WalkmeshSceneNode> walkmeshOpen2SceneNode;
    auto walkmeshOpen2 = _graphicsSvc.walkmeshes.get(modelName + "2", ResourceType::Dwk);
    if (walkmeshOpen2) {
        walkmeshOpen2SceneNode = _sceneGraph->newWalkmesh(*walkmeshOpen2);
        walkmeshOpen2SceneNode->setEnabled(false);
    }

    //

    _tag = move(tag);
    _name = move(locName);
    _position = glm::vec3(x, y, z);
    _facing = bearing;
    _sceneNode = sceneNode.get();
    _walkmeshClosed = walkmeshClosedSceneNode.get();
    _walkmeshOpen1 = walkmeshOpen1SceneNode.get();
    _walkmeshOpen2 = walkmeshOpen2SceneNode.get();

    flushTransform();
}

void Door::handleClick(Object &clicker) {
    if (_state == State::Closed) {
        _state = State::Opening;
    }
}

void Door::update(float delta) {
    if (_sceneNode) {
        auto &modelSceneNode = static_cast<ModelSceneNode &>(*_sceneNode);
        if (_state == State::Closed) {
            if (modelSceneNode.activeAnimationName() != "closed") {
                modelSceneNode.playAnimation("closed");
            }
        } else if (_state == State::Opening) {
            auto animName = modelSceneNode.activeAnimationName();
            if (animName == "opening1" && modelSceneNode.isAnimationFinished()) {
                _state = State::Open;
                _walkmeshClosed->setEnabled(false);
                _walkmeshOpen1->setEnabled(true);
            } else if (animName != "opening1") {
                modelSceneNode.playAnimation("opening1");
            }
        } else if (_state == State::Open) {
            if (modelSceneNode.activeAnimationName() != "open1") {
                modelSceneNode.playAnimation("open1");
            }
        }
    }
}

void Door::flushTransform() {
    Object::flushTransform();

    auto transform = glm::translate(_position);
    transform *= glm::rotate(_facing, glm::vec3(0.0f, 0.0f, 1.0f));
    transform *= glm::rotate(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    if (_walkmeshClosed) {
        _walkmeshClosed->setLocalTransform(transform);
    }
    if (_walkmeshOpen1) {
        _walkmeshOpen1->setLocalTransform(transform);
    }
    if (_walkmeshOpen2) {
        _walkmeshOpen2->setLocalTransform(transform);
    }
}

} // namespace game

} // namespace reone