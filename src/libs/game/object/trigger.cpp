/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/object/trigger.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/script/runner.h"
#include "reone/resource/di/services.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/node/trigger.h"
#include "reone/system/logutil.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Trigger::loadFromGIT(const schema::GIT_TriggerList &git) {
    std::string templateResRef(boost::to_lower_copy(git.TemplateResRef));
    loadFromBlueprint(templateResRef);

    // _tag = boost::to_lower_copy(gffs.getString("Tag"));
    _transitionDestin = _services.resource.strings.get(git.TransitionDestin.first);
    _linkedToModule = boost::to_lower_copy(git.LinkedToModule);
    _linkedTo = boost::to_lower_copy(git.LinkedTo);
    _linkedToFlags = git.LinkedToFlags;

    loadTransformFromGIT(git);
    loadGeometryFromGIT(git);

    auto &sceneGraph = _services.scene.graphs.get(_sceneName);
    _sceneNode = sceneGraph.newTrigger(_geometry);
    _sceneNode->setLocalTransform(glm::translate(_position));
}

void Trigger::loadTransformFromGIT(const schema::GIT_TriggerList &git) {
    _position.x = git.XPosition;
    _position.y = git.YPosition;
    _position.z = git.ZPosition;

    // Orientation is ignored as per Bioware specification

    updateTransform();
}

void Trigger::loadGeometryFromGIT(const schema::GIT_TriggerList &git) {
    for (auto &pointStruct : git.Geometry) {
        float x = pointStruct.PointX;
        float y = pointStruct.PointY;
        float z = pointStruct.PointZ;
        _geometry.push_back(glm::vec3(x, y, z));
    }
}

void Trigger::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> utt(_services.resource.gffs.get(resRef, ResourceType::Utt));
    if (utt) {
        auto uttParsed = schema::parseUTT(*utt);
        loadUTT(uttParsed);
    }
}

void Trigger::update(float dt) {
    std::set<std::shared_ptr<Object>> tenantsToRemove;
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

void Trigger::addTenant(const std::shared_ptr<Object> &object) {
    _tenants.insert(object);
}

bool Trigger::isIn(const glm::vec2 &point) const {
    return static_cast<TriggerSceneNode *>(_sceneNode.get())->isIn(point);
}

bool Trigger::isTenant(const std::shared_ptr<Object> &object) const {
    auto maybeTenant = find(_tenants.begin(), _tenants.end(), object);
    return maybeTenant != _tenants.end();
}

void Trigger::loadUTT(const schema::UTT &utt) {
    _tag = boost::to_lower_copy(utt.Tag);
    _blueprintResRef = boost::to_lower_copy(utt.TemplateResRef);
    _name = _services.resource.strings.get(utt.LocalizedName.first);
    _autoRemoveKey = utt.AutoRemoveKey; // always 0, but could be useful
    _faction = static_cast<Faction>(utt.Faction);
    _keyName = utt.KeyName;
    _triggerType = utt.Type; // could be Generic, Area Transition or Trap
    _trapDetectable = utt.TrapDetectable;
    _trapDetectDC = utt.TrapDetectDC;
    _trapDisarmable = utt.TrapDisarmable;
    _disarmDC = utt.DisarmDC;
    _trapFlag = utt.TrapFlag;
    _trapType = utt.TrapType; // index into traps.2da

    _onDisarm = boost::to_lower_copy(utt.OnDisarm);               // always empty, but could be useful
    _onTrapTriggered = boost::to_lower_copy(utt.OnTrapTriggered); // always empty, but could be useful
    _onHeartbeat = boost::to_lower_copy(utt.ScriptHeartbeat);
    _onEnter = boost::to_lower_copy(utt.ScriptOnEnter);
    _onExit = boost::to_lower_copy(utt.ScriptOnExit);
    _onUserDefined = boost::to_lower_copy(utt.ScriptUserDefine);

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
