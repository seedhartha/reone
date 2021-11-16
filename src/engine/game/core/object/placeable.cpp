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

#include "../../../graphics/model/models.h"
#include "../../../graphics/walkmesh/walkmeshes.h"
#include "../../../resource/2da.h"
#include "../../../resource/2das.h"
#include "../../../resource/gffs.h"
#include "../../../resource/resources.h"
#include "../../../resource/strings.h"
#include "../../../scene/node/model.h"
#include "../../../script/types.h"

#include "../game.h"
#include "../script/runner.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

void Placeable::loadFromGIT(const GffStruct &gffs) {
    string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);

    loadTransformFromGIT(gffs);
}

void Placeable::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> utp(_gffs.get(resRef, ResourceType::Utp));
    if (!utp)
        return;

    loadUTP(*utp);

    shared_ptr<TwoDA> placeables(_twoDas.get("placeables"));
    string modelName(boost::to_lower_copy(placeables->getString(_appearance, "modelname")));

    auto model = _sceneGraph.newModel(_models.get(modelName), ModelUsage::Placeable);
    model->setCullable(true);
    model->setDrawDistance(64.0f);
    _sceneNode = move(model);

    _walkmesh = _walkmeshes.get(modelName, ResourceType::Pwk);
}

void Placeable::loadTransformFromGIT(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, gffs.getFloat("Bearing")));

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
        _scriptRunner.run(_onUsed, _id, usedBy ? usedBy->id() : kObjectInvalid);
    }
}

void Placeable::runOnInvDisturbed(shared_ptr<SpatialObject> triggerrer) {
    if (!_onInvDisturbed.empty()) {
        _scriptRunner.run(_onInvDisturbed, _id, triggerrer ? triggerrer->id() : kObjectInvalid);
    }
}

void Placeable::loadUTP(const GffStruct &utp) {
    _tag = boost::to_lower_copy(utp.getString("Tag"));
    _name = _strings.get(utp.getInt("LocName"));
    _blueprintResRef = boost::to_lower_copy(utp.getString("TemplateResRef"));
    _conversation = boost::to_lower_copy(utp.getString("Conversation"));
    _interruptable = utp.getBool("Interruptable");
    _faction = utp.getEnum("Faction", Faction::Invalid);
    _plot = utp.getBool("Plot");
    _minOneHP = utp.getBool("Min1HP");
    _keyRequired = utp.getBool("KeyRequired");
    _lockable = utp.getBool("Lockable");
    _locked = utp.getBool("Locked");
    _openLockDC = utp.getInt("OpenLockDC");
    _animationState = utp.getInt("AnimationState");
    _appearance = utp.getInt("Appearance");
    _hitPoints = utp.getInt("HP");
    _currentHitPoints = utp.getInt("CurrentHP");
    _hardness = utp.getInt("Hardness");
    _fortitude = utp.getInt("Fort");
    _hasInventory = utp.getBool("HasInventory");
    _partyInteract = utp.getBool("PartyInteract");
    _static = utp.getBool("Static");
    _usable = utp.getBool("Useable");

    _onClosed = boost::to_lower_copy(utp.getString("OnClosed"));
    _onDamaged = boost::to_lower_copy(utp.getString("OnDamaged")); // always empty, but could be useful
    _onDeath = boost::to_lower_copy(utp.getString("OnDeath"));
    _onHeartbeat = boost::to_lower_copy(utp.getString("OnHeartbeat"));
    _onLock = boost::to_lower_copy(utp.getString("OnLock"));                   // always empty, but could be useful
    _onMeleeAttacked = boost::to_lower_copy(utp.getString("OnMeleeAttacked")); // always empty, but could be useful
    _onOpen = boost::to_lower_copy(utp.getString("OnOpen"));
    _onSpellCastAt = boost::to_lower_copy(utp.getString("OnSpellCastAt"));
    _onUnlock = boost::to_lower_copy(utp.getString("OnUnlock")); // always empty, but could be useful
    _onUserDefined = boost::to_lower_copy(utp.getString("OnUserDefined"));
    _onEndDialogue = boost::to_lower_copy(utp.getString("OnEndDialogue"));
    _onInvDisturbed = boost::to_lower_copy(utp.getString("OnInvDisturbed"));
    _onUsed = boost::to_lower_copy(utp.getString("OnUsed"));

    for (auto &itemGffs : utp.getList("ItemList")) {
        string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
        addItem(resRef, 1, true);
    }

    // These fields are ignored as being most likely unused:
    //
    // - Description (not applicable, mostly -1)
    // - AutoRemoveKey (not applicable, always 0)
    // - CloseLockDC (not applicable, always 0)
    // - PortraitId (not applicable, always 0)
    // - TrapDetectable (not applicable, always 1)
    // - TrapDetectDC (not applicable, always 0)
    // - TrapDisarmable (not applicable, always 1)
    // - DisarmDC (not applicable)
    // - TrapFlag (not applicable, always 0)
    // - TrapOneShot (not applicable, always 1)
    // - TrapType (not applicable, mostly 0)
    // - KeyName (not applicable, always empty)
    // - Ref (not applicable, always 0)
    // - Will (not applicable, always 0)
    // - Type (obsolete, always 0)
    // - OnDisarm (not applicable, always empty)
    // - OnTrapTriggered (not applicable, always empty)
    // - BodyBag (not applicable, always 0)
    // - Type (obsolete, always 0)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

} // namespace game

} // namespace reone
