/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../core/streamutil.h"
#include "../../render/modelinstance.h"
#include "../../resources/resources.h"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

Door::Door(uint32_t id) : SpatialObject(id) {
    _type = ObjectType::Door;
    _drawDistance = FLT_MAX;
    _fadeDistance = 0.25f * _drawDistance;
}

void Door::load(const GffStruct &gffs) {
    ResourceManager &resources = ResourceManager::instance();

    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _heading = gffs.getFloat("Bearing");
    _linkedToModule = gffs.getString("LinkedToModule");

    _linkedTo = gffs.getString("LinkedTo");
    boost::to_lower(_linkedTo);

    int transDestStrRef = gffs.getInt("TransitionDestin");
    if (transDestStrRef != -1) {
        _transitionDestin = resources.getString(transDestStrRef).text;
    }

    updateTransform();

    string templResRef(gffs.getString("TemplateResRef"));
    shared_ptr<GffStruct> utd(resources.findGFF(templResRef, ResourceType::DoorBlueprint));
    loadBlueprint(*utd);
}

void Door::loadBlueprint(const GffStruct &gffs) {
    _tag = gffs.getString("Tag");
    boost::to_lower(_tag);

    _static = gffs.getInt("Static", 0) != 0;

    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<TwoDaTable> table = resources.find2DA("genericdoors");

    int type = gffs.getInt("GenericType");
    string model(table->getString(type, "modelname"));
    boost::to_lower(model);

    _model = make_unique<ModelInstance>(resources.findModel(model));
    _walkmesh = resources.findWalkmesh(model + "0", ResourceType::DoorWalkmesh);
}

void Door::open(const shared_ptr<Object> &triggerrer) {
    if (_model) {
        _model->setDefaultAnimation("opened1");
        _model->animate("opening1");
    }
    _open = true;
}

void Door::close(const shared_ptr<Object> &triggerrer) {
    if (_model) {
        _model->setDefaultAnimation("closed1");
        _model->animate("closing1");
    }
    _open = false;
}

void Door::saveTo(AreaState &state) const {
    if (_tag.empty()) return;

    DoorState doorState;
    doorState.open = _open;

    state.doors[_tag] = move(doorState);
}

void Door::loadState(const AreaState &state) {
    if (_tag.empty()) return;

    auto it = state.doors.find(_tag);
    if (it == state.doors.end()) return;

    const DoorState &doorState = it->second;
    if (doorState.open) open(nullptr);
}

bool Door::isOpen() const {
    return _open;
}

bool Door::isStatic() const {
    return _static;
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

} // namespace game

} // namespace reone
