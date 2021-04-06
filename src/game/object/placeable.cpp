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

#include <boost/algorithm/string.hpp>

#include "../../render/model/models.h"
#include "../../render/walkmesh/walkmeshes.h"
#include "../../resource/resources.h"
#include "../../scene/node/modelscenenode.h"
#include "../../script/types.h"

#include "../script/runner.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

Placeable::Placeable(
    uint32_t id,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph,
    ScriptRunner *scriptRunner
) :
    SpatialObject(id, ObjectType::Placeable, objectFactory, sceneGraph, scriptRunner) {

    _drawDistance = 64.0f;
}

void Placeable::loadFromGIT(const GffStruct &gffs) {
    string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);

    loadTransformFromGIT(gffs);
}

void Placeable::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> utp(Resources::instance().getGFF(resRef, ResourceType::Utp));
    loadUTP(*utp);

    shared_ptr<TwoDA> placeables(Resources::instance().get2DA("placeables"));
    string modelName(boost::to_lower_copy(placeables->getString(_appearance, "modelname")));

    auto model = make_shared<ModelSceneNode>(ModelSceneNode::Classification::Placeable, Models::instance().get(modelName), _sceneGraph);
    _sceneNode = move(model);

    _walkmesh = Walkmeshes::instance().get(modelName, ResourceType::Pwk);
}

void Placeable::loadTransformFromGIT(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _facing = gffs.getFloat("Bearing");

    updateTransform();
}

bool Placeable::isSelectable() const {
    return _usable;
}

shared_ptr<Walkmesh> Placeable::getWalkmesh() const {
    return _walkmesh;
}

void Placeable::runOnUsed(shared_ptr<SpatialObject> usedBy) {
    if (!_onUsed.empty()) {
        _scriptRunner->run(_onUsed, _id, usedBy ? usedBy->id() : kObjectInvalid);
    }
}

void Placeable::runOnInvDisturbed(shared_ptr<SpatialObject> triggerrer) {
    if (!_onInvDisturbed.empty()) {
        _scriptRunner->run(_onInvDisturbed, _id, triggerrer ? triggerrer->id() : kObjectInvalid);
    }
}

} // namespace game

} // namespace reone
