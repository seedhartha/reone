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

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../../common/streamutil.h"
#include "../../render/model/models.h"
#include "../../render/walkmesh/walkmeshes.h"
#include "../../resource/resources.h"
#include "../../scene/node/modelscenenode.h"

#include "../blueprint/blueprints.h"

#include "objectfactory.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

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

bool Placeable::isSelectable() const {
    return _usable;
}

void Placeable::load(const GffStruct &gffs) {
    loadBlueprint(gffs);

    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _facing = gffs.getFloat("Bearing");

    updateTransform();
}

void Placeable::loadBlueprint(const GffStruct &gffs) {
    string resRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    shared_ptr<PlaceableBlueprint> blueprint(Blueprints::instance().getPlaceable(resRef));
    load(blueprint);
}

void Placeable::load(const shared_ptr<PlaceableBlueprint> &blueprint) {
    if (!blueprint) {
        throw invalid_argument("blueprint must not be null");
    }
    blueprint->load(*this);

    shared_ptr<TwoDA> placeables(Resources::instance().get2DA("placeables"));
    string modelName(boost::to_lower_copy(placeables->getString(_appearance, "modelname")));
    auto model = make_shared<ModelSceneNode>(ModelSceneNode::Classification::Placeable, Models::instance().get(modelName), _sceneGraph);

    _sceneNode = move(model);
    _walkmesh = Walkmeshes::instance().get(modelName, ResourceType::Pwk);
}

shared_ptr<Walkmesh> Placeable::getWalkmesh() const {
    return _walkmesh;
}

} // namespace game

} // namespace reone
