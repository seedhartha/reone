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

#include <boost/algorithm/string.hpp>

#include "../../common/streamutil.h"
#include "../../graphics/model/models.h"
#include "../../graphics/walkmesh/walkmeshes.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"
#include "../../scene/node/modelscenenode.h"
#include "../../scene/types.h"
#include "../../script/scripts.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

Door::Door(
    uint32_t id,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph
) :
    SpatialObject(id, ObjectType::Door, objectFactory, sceneGraph) {
    _drawDistance = FLT_MAX;
}

void Door::loadFromGIT(const GffStruct &gffs) {
    string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);

    _linkedToModule = boost::to_lower_copy(gffs.getString("LinkedToModule"));
    _linkedTo = boost::to_lower_copy(gffs.getString("LinkedTo"));
    _linkedToFlags = gffs.getInt("LinkedToFlags");
    _transitionDestin = Strings::instance().get(gffs.getInt("TransitionDestin"));

    loadTransformFromGIT(gffs);
}

void Door::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> utd(Resources::instance().getGFF(resRef, ResourceType::Utd));
    if (!utd) return;

    loadUTD(*utd);

    shared_ptr<TwoDA> doors(Resources::instance().get2DA("genericdoors"));
    string modelName(boost::to_lower_copy(doors->getString(_genericType, "modelname")));

    auto model = make_unique<ModelSceneNode>(ModelUsage::Door, Models::instance().get(modelName), _sceneGraph);
    _sceneNode = move(model);

    _closedWalkmesh = Walkmeshes::instance().get(modelName + "0", ResourceType::Dwk);
    _open1Walkmesh = Walkmeshes::instance().get(modelName + "1", ResourceType::Dwk);
    _open2Walkmesh = Walkmeshes::instance().get(modelName + "2", ResourceType::Dwk);
}

void Door::loadTransformFromGIT(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, gffs.getFloat("Bearing")));

    updateTransform();
}

bool Door::isSelectable() const {
    return !_static && !_open;
}

void Door::open(const shared_ptr<Object> &triggerrer) {
    shared_ptr<ModelSceneNode> model(getModelSceneNode());
    if (model) {
        model->animator().setDefaultAnimation("opened1", AnimationProperties::fromFlags(AnimationFlags::loop));
        model->animator().playAnimation("opening1");
    }
    _open = true;
}

void Door::close(const shared_ptr<Object> &triggerrer) {
    shared_ptr<ModelSceneNode> model(getModelSceneNode());
    if (model) {
        model->animator().setDefaultAnimation("closed", AnimationProperties::fromFlags(AnimationFlags::loop));
        model->animator().playAnimation("closing1");
    }
    _open = false;
}

shared_ptr<Walkmesh> Door::getWalkmesh() const {
    return _open ? _open1Walkmesh : _closedWalkmesh;
}

void Door::setLocked(bool locked) {
    _locked = locked;
}

} // namespace game

} // namespace reone
