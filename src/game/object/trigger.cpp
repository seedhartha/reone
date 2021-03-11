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

#include "trigger.h"

#include <boost/algorithm/string.hpp>

#include "glm/gtx/intersect.hpp"

#include "../../common/log.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"
#include "../../scene/node/meshnode.h"

#include "../blueprint/blueprints.h"
#include "../script/runner.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static bool g_debugTriggers = false;

Trigger::Trigger(
    uint32_t id,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph,
    ScriptRunner *scriptRunner
) :
    SpatialObject(
        id,
        ObjectType::Trigger,
        objectFactory,
        sceneGraph,
        scriptRunner) {
}

void Trigger::load(const GffStruct &gffs) {
    loadBlueprint(gffs);

    int transDestIdx = gffs.getInt("TransitionDestin", -1);
    if (transDestIdx != -1) {
        _transitionDestin = Strings::instance().get(transDestIdx);
    }

    _linkedToModule = boost::to_lower_copy(gffs.getString("LinkedToModule"));
    _linkedTo = boost::to_lower_copy(gffs.getString("LinkedTo"));

    for (auto &child : gffs.getList("Geometry")) {
        float x = child->getFloat("PointX");
        float y = child->getFloat("PointY");
        float z = child->getFloat("PointZ");
        _geometry.push_back(glm::vec4(x, y, z, 1.0f));
    }
    loadAppearance();

    _position.x = gffs.getFloat("XPosition");
    _position.y = gffs.getFloat("YPosition");
    _position.z = gffs.getFloat("ZPosition");
    updateTransform();
}

void Trigger::loadBlueprint(const GffStruct &gffs) {
    string resRef(gffs.getString("TemplateResRef"));
    shared_ptr<TriggerBlueprint> blueprint(Blueprints::instance().getTrigger(resRef));
    blueprint->load(*this);
}

void Trigger::loadAppearance() {
    if (_geometry.empty() || !g_debugTriggers) return;

    vector<float> vertices;
    for (auto &point : _geometry) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(point.z);
    }

    vector<uint16_t> indices;
    if (_geometry.size() >= 3) {
        indices.insert(indices.end(), { 0, 1, 2 });
    }
    if (_geometry.size() >= 4) {
        indices.insert(indices.end(), { 2, 3, 0 });
    }
    if (_geometry.size() >= 5) {
        warn("Trigger: geometry contains more than 4 points - they will be ignored");
    }

    Mesh::VertexOffsets offsets;
    offsets.stride = 3 * sizeof(float);

    auto mesh = make_shared<Mesh>(static_cast<int>(_geometry.size()), move(vertices), move(indices), move(offsets));
    mesh->init();

    auto sceneNode = make_shared<MeshSceneNode>(_sceneGraph, mesh);
    sceneNode->setTransparent(true);
    sceneNode->setAlpha(0.5f);
    _sceneNode = sceneNode;
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
        _tenants.erase(tenant);
        if (!_onExit.empty()) {
            _scriptRunner->run(_onExit, _id, tenant->id());
        }
    }
    if (_sceneNode) {
        auto meshSceneNode = static_pointer_cast<MeshSceneNode>(_sceneNode);
        meshSceneNode->setColor(!_tenants.empty() ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f));
    }

    SpatialObject::update(dt);
}

void Trigger::addTenant(const std::shared_ptr<SpatialObject> &object) {
    _tenants.insert(object);
}

bool Trigger::isIn(const glm::vec2 &point) const {
    glm::vec3 modelSpacePoint(glm::inverse(_transform) * glm::vec4(point, 1000.0f, 1.0f));
    glm::vec3 down(0.0f, 0.0f, -1.0f);
    glm::vec2 intersection;
    float distance;

    return
        (_geometry.size() >= 3 && glm::intersectRayTriangle(modelSpacePoint, down, _geometry[0], _geometry[1], _geometry[2], intersection, distance)) ||
        (_geometry.size() >= 4 && glm::intersectRayTriangle(modelSpacePoint, down, _geometry[2], _geometry[3], _geometry[0], intersection, distance));
}

bool Trigger::isTenant(const std::shared_ptr<SpatialObject> &object) const {
    auto maybeTenant = find(_tenants.begin(), _tenants.end(), object);
    return maybeTenant != _tenants.end();
}

} // namespace game

} // namespace reone
