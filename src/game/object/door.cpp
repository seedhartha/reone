/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../../render/models.h"
#include "../../render/walkmeshes.h"
#include "../../resource/resources.h"
#include "../../scene/modelscenenode.h"
#include "../../script/scripts.h"
#include "../../common/streamutil.h"

#include "../blueprint/blueprints.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

Door::Door(uint32_t id, SceneGraph *sceneGraph) : SpatialObject(id, ObjectType::Door, sceneGraph) {
    _drawDistance = FLT_MAX;
    _fadeDistance = 0.25f * _drawDistance;
    _selectable = true;
}

string Door::conversation() const {
    return _blueprint ? _blueprint->conversation() : "";
}

void Door::load(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _heading = gffs.getFloat("Bearing");
    _linkedToModule = gffs.getString("LinkedToModule");

    _linkedTo = gffs.getString("LinkedTo");
    boost::to_lower(_linkedTo);

    int transDestStrRef = gffs.getInt("TransitionDestin");
    if (transDestStrRef != -1) {
        _transitionDestin = Resources::instance().getString(transDestStrRef);
    }

    string templResRef(gffs.getString("TemplateResRef"));
    boost::to_lower(templResRef);

    loadBlueprint(templResRef);
    updateTransform();
}

void Door::loadBlueprint(const string &resRef) {
    _blueprint = Blueprints::instance().getDoor(resRef);
    _tag = _blueprint->tag();
    _title = _blueprint->localizedName();
    _locked = _blueprint->isLocked();
    _selectable = !_blueprint->isStatic();

    shared_ptr<TwoDaTable> table = Resources::instance().get2DA("genericdoors");

    string model(table->getString(_blueprint->genericType(), "modelname"));
    boost::to_lower(model);

    _model = make_unique<ModelSceneNode>(_sceneGraph, Models::instance().get(model));
    _walkmesh = Walkmeshes::instance().get(model + "0", ResourceType::DoorWalkmesh);
}

void Door::open(Object *triggerrer) {
    if (_model) {
        _model->setDefaultAnimation("opened1");
        _model->playAnimation("opening1");
    }
    _open = true;
    _selectable = false;
}

void Door::close(Object *triggerrer) {
    if (_model) {
        _model->setDefaultAnimation("closed1");
        _model->playAnimation("closing1");
    }
    _open = false;
    _selectable = true;
}

bool Door::isOpen() const {
    return _open;
}

bool Door::isLocked() const {
    return _locked;
}

const DoorBlueprint &Door::blueprint() const {
    return *_blueprint;
}

const string &Door::linkedToModule() const {
    return _linkedToModule;
}

const string &Door::linkedTo() const {
    return _linkedTo;
}

const string &Door::transitionDestin() const {
    return _transitionDestin;
}

void Door::setLocked(bool locked) {
    _locked = locked;
}

} // namespace game

} // namespace reone
