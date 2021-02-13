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
#include "../../render/model/models.h"
#include "../../render/walkmeshes.h"
#include "../../resource/resources.h"
#include "../../scene/node/modelscenenode.h"
#include "../../scene/types.h"
#include "../../script/scripts.h"

#include "../blueprint/blueprints.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

Door::Door(
    uint32_t id,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph,
    ScriptRunner *scriptRunner
) :
    SpatialObject(id, ObjectType::Door, objectFactory, sceneGraph, scriptRunner) {
    _drawDistance = FLT_MAX;
}

bool Door::isSelectable() const {
    return !_static && !_open;
}

void Door::load(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _facing = gffs.getFloat("Bearing");
    _linkedToModule = boost::to_lower_copy(gffs.getString("LinkedToModule"));
    _linkedTo = boost::to_lower_copy(gffs.getString("LinkedTo"));

    int transDestStrRef = gffs.getInt("TransitionDestin");
    if (transDestStrRef != -1) {
        _transitionDestin = Resources::instance().getString(transDestStrRef);
    }

    loadBlueprint(gffs);
    updateTransform();
}

void Door::loadBlueprint(const GffStruct &gffs) {
    string resRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));

    shared_ptr<DoorBlueprint> blueprint(Blueprints::instance().getDoor(resRef));
    blueprint->load(*this);

    shared_ptr<TwoDaTable> table(Resources::instance().get2DA("genericdoors"));

    string modelName(boost::to_lower_copy(table->getString(_genericType, "modelname")));
    auto model = make_unique<ModelSceneNode>(_sceneGraph, Models::instance().get(modelName));
    model->setLightingEnabled(true);
    _sceneNode = move(model);

    _walkmesh = Walkmeshes::instance().get(modelName + "0", ResourceType::Dwk);
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

void Door::setLocked(bool locked) {
    _locked = locked;
}

} // namespace game

} // namespace reone
