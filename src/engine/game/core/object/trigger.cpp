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

#include "../../../common/logutil.h"
#include "../../../resource/gffs.h"
#include "../../../resource/resources.h"
#include "../../../resource/strings.h"

#include "../game.h"
#include "../script/runner.h"
#include "../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Trigger::loadFromGIT(const GffStruct &gffs) {
    string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);

    _tag = boost::to_lower_copy(gffs.getString("Tag"));
    _transitionDestin = _services.strings.get(gffs.getInt("TransitionDestin"));
    _linkedToModule = boost::to_lower_copy(gffs.getString("LinkedToModule"));
    _linkedTo = boost::to_lower_copy(gffs.getString("LinkedTo"));
    _linkedToFlags = gffs.getInt("LinkedToFlags");

    loadTransformFromGIT(gffs);
    loadGeometryFromGIT(gffs);
}

void Trigger::loadTransformFromGIT(const GffStruct &gffs) {
    _position.x = gffs.getFloat("XPosition");
    _position.y = gffs.getFloat("YPosition");
    _position.z = gffs.getFloat("ZPosition");

    // Orientation is ignored as per Bioware specification

    updateTransform();
}

void Trigger::loadGeometryFromGIT(const GffStruct &gffs) {
    for (auto &child : gffs.getList("Geometry")) {
        float x = child->getFloat("PointX");
        float y = child->getFloat("PointY");
        float z = child->getFloat("PointZ");
        _geometry.push_back(glm::vec3(x, y, z));
    }
}

void Trigger::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> utt(_services.gffs.get(resRef, ResourceType::Utt));
    if (utt) {
        loadUTT(*utt);
    }
}

void Trigger::update(float dt) {
    set<shared_ptr<SpatialObject>> tenantsToRemove;
    for (auto &tenant : _tenants) {
        if (tenant) {
            glm::vec2 position2d(tenant->position());
            if (isIn(position2d))
                continue;
        }
        tenantsToRemove.insert(tenant);
    }
    for (auto &tenant : tenantsToRemove) {
        _tenants.erase(tenant);
        if (!_onExit.empty()) {
            _game.scriptRunner().run(_onExit, _id, tenant->id());
        }
    }
}

void Trigger::addTenant(const std::shared_ptr<SpatialObject> &object) {
    _tenants.insert(object);
}

bool Trigger::isIn(const glm::vec2 &point) const {
    glm::vec3 modelSpacePoint(glm::inverse(_transform) * glm::vec4(point, 1000.0f, 1.0f));
    glm::vec3 down(0.0f, 0.0f, -1.0f);
    glm::vec2 intersection;
    float distance;

    return (_geometry.size() >= 3 && glm::intersectRayTriangle(modelSpacePoint, down, _geometry[0], _geometry[1], _geometry[2], intersection, distance)) ||
           (_geometry.size() >= 4 && glm::intersectRayTriangle(modelSpacePoint, down, _geometry[2], _geometry[3], _geometry[0], intersection, distance));
}

bool Trigger::isTenant(const std::shared_ptr<SpatialObject> &object) const {
    auto maybeTenant = find(_tenants.begin(), _tenants.end(), object);
    return maybeTenant != _tenants.end();
}

void Trigger::loadUTT(const GffStruct &utt) {
    _tag = boost::to_lower_copy(utt.getString("Tag"));
    _blueprintResRef = boost::to_lower_copy(utt.getString("TemplateResRef"));
    _name = _services.strings.get(utt.getInt("LocalizedName"));
    _autoRemoveKey = utt.getBool("AutoRemoveKey"); // always 0, but could be useful
    _faction = utt.getEnum("Faction", Faction::Invalid);
    _keyName = utt.getString("KeyName");
    _triggerType = utt.getInt("Type"); // could be Generic, Area Transition or Trap
    _trapDetectable = utt.getBool("TrapDetectable");
    _trapDetectDC = utt.getInt("TrapDetectDC");
    _trapDisarmable = utt.getBool("TrapDisarmable");
    _disarmDC = utt.getInt("DisarmDC");
    _trapFlag = utt.getBool("TrapFlag");
    _trapType = utt.getInt("TrapType"); // index into traps.2da

    _onDisarm = boost::to_lower_copy(utt.getString("OnDisarm"));               // always empty, but could be useful
    _onTrapTriggered = boost::to_lower_copy(utt.getString("OnTrapTriggered")); // always empty, but could be useful
    _onHeartbeat = boost::to_lower_copy(utt.getString("ScriptHeartbeat"));
    _onEnter = boost::to_lower_copy(utt.getString("ScriptOnEnter"));
    _onExit = boost::to_lower_copy(utt.getString("ScriptOnExit"));
    _onUserDefined = boost::to_lower_copy(utt.getString("ScriptUserDefine"));

    // Unused fields:
    //
    // - Cursor (not applicable)
    // - HighlightHeight (not applicable)
    // - LoadScreenID (always 0)
    // - PortraitId (not applicable, always 0)
    // - TrapOneShot (always 1)
    // - OnClick (not applicable)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

} // namespace game

} // namespace reone
