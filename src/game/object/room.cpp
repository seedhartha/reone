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

#include "room.h"

#include "reone/graphics/models.h"
#include "reone/graphics/options.h"
#include "reone/graphics/services.h"
#include "reone/graphics/walkmeshes.h"
#include "../../scene/graph.h"
#include "../../scene/node/model.h"
#include "../../scene/services.h"

#include "../gameinterface.h"

using namespace std;

using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Room::loadFromLyt(const Layout::Room &lyt, GrassProperties grassProperties) {
    _name = lyt.name;

    // Model
    auto model = _graphicsSvc.models.get(lyt.name);
    if (model) {
        auto sceneNode = _sceneGraph->newModel(*model, ModelUsage::Room);
        sceneNode->init();
        sceneNode->setUser(*this);
        _sceneNode = sceneNode.get();

        // Grass
        if (_graphicsOpt.grass && grassProperties.texture) {
            auto aabbNode = model->getAABBNode();
            if (aabbNode && grassProperties.texture) {
                auto grass = _sceneGraph->newGrass(grassProperties, *aabbNode);
                grass->init();
                _grass = grass.get();
            }
        }
    }

    // Walkmesh
    auto walkmesh = _graphicsSvc.walkmeshes.get(lyt.name, ResourceType::Wok);
    if (walkmesh) {
        _walkmesh = _sceneGraph->newWalkmesh(*walkmesh).get();
        _walkmesh->setUser(*this);
    }

    _position = lyt.position;
    flushTransform();
}

void Room::update(float delta) {
    if (_walkmesh) {
        auto &walkmesh = _walkmesh->walkmesh();
        auto objectsInside = _game.objectsSatisfying([&walkmesh](auto &object) {
            return walkmesh.contains(glm::vec2(object.position()));
        });
        for (auto &object : objectsInside) {
            object->setRoom(this);
        }
        _objectsInside.swap(objectsInside);
    }
}

void Room::show() {
    if (!_sceneNode) {
        return;
    }
    _sceneNode->setEnabled(true);
    if (_grass) {
        _grass->setEnabled(true);
    }
}

void Room::hide() {
    if (!_sceneNode) {
        return;
    }
    _sceneNode->setEnabled(false);
    if (_grass) {
        _grass->setEnabled(false);
    }
}

void Room::flushTransform() {
    if (!_sceneNode) {
        return;
    }
    auto transform = glm::translate(_position);
    transform *= glm::rotate(_facing, glm::vec3(0.0f, 0.0f, 1.0f));
    transform *= glm::rotate(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    _sceneNode->setLocalTransform(transform);

    if (_grass) {
        auto model = static_cast<ModelSceneNode *>(_sceneNode);
        _grass->setLocalTransform(transform * model->model().getAABBNode()->absoluteTransform());
    }
}

} // namespace game

} // namespace reone
