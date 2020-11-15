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

#include "trigger.h"

#include <boost/algorithm/string.hpp>

#include "glm/gtx/intersect.hpp"

#include "../../resource/resources.h"

#include "../blueprint/blueprints.h"
#include "../script/util.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

Trigger::Trigger(uint32_t id, SceneGraph *sceneGraph) : SpatialObject(id, ObjectType::Trigger, sceneGraph) {
}

void Trigger::load(const GffStruct &gffs) {
    string blueprintResRef(gffs.getString("TemplateResRef"));
    _blueprint = Blueprints::instance().getTrigger(blueprintResRef);
    _tag = _blueprint->tag();

    _position.x = gffs.getFloat("XPosition");
    _position.y = gffs.getFloat("YPosition");
    _position.z = gffs.getFloat("ZPosition");

    updateTransform();

    int transDestIdx = gffs.getInt("TransitionDestin", -1);
    if (transDestIdx != -1) {
        _transitionDestin = Resources::instance().getString(transDestIdx);
    }

    _linkedToModule = gffs.getString("LinkedToModule");
    boost::to_lower(_linkedToModule);

    _linkedTo = gffs.getString("LinkedTo");
    boost::to_lower(_linkedTo);

    for (auto &child : gffs.getList("Geometry")) {
        float x = child.getFloat("PointX");
        float y = child.getFloat("PointY");
        float z = child.getFloat("PointZ");

        _geometry.push_back(_transform * glm::vec4(x, y, z, 1.0f));
    }
}

void Trigger::update(float dt) {
    set<shared_ptr<SpatialObject>> tenantsToRemove;

    for (auto &tenant : _tenants) {
        if (tenant) {
            glm::vec2 position2d(tenant->position());
            if (isIn(position2d)) continue;
        }
        tenantsToRemove.insert(tenant);
    }
    for (auto &tenant : tenantsToRemove) {
        if (!_blueprint->onExit().empty()) {
            runScript(_blueprint->onExit(), id(), tenant->id(), -1);
        }
        _tenants.erase(tenant);
    }
}

bool Trigger::isIn(const glm::vec2 &point) const {
    glm::vec3 liftedPosition(point, 1000.0f);
    glm::vec3 down(0.0f, 0.0f, -1.0f);
    glm::vec2 intersection;
    float distance;

    return
        (_geometry.size() >= 3 && glm::intersectRayTriangle(liftedPosition, down, _geometry[0], _geometry[1], _geometry[2], intersection, distance)) ||
        (_geometry.size() >= 4 && glm::intersectRayTriangle(liftedPosition, down, _geometry[2], _geometry[3], _geometry[0], intersection, distance));
}

bool Trigger::isTenant(const std::shared_ptr<SpatialObject> &object) const {
    auto maybeTenant = find(_tenants.begin(), _tenants.end(), object);
    return maybeTenant != _tenants.end();
}

const TriggerBlueprint &Trigger::blueprint() const {
    return *_blueprint;
}

const string &Trigger::linkedToModule() const {
    return _linkedToModule;
}

const string &Trigger::linkedTo() const {
    return _linkedTo;
}

void Trigger::addTenant(const std::shared_ptr<SpatialObject> &object) {
    _tenants.insert(object);
}

} // namespace game

} // namespace reone
