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

#include "reone/game/object/placeable.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/script/runner.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/models.h"
#include "reone/graphics/walkmeshes.h"
#include "reone/resource/2da.h"
#include "reone/resource/2das.h"
#include "reone/resource/di/services.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/node/model.h"
#include "reone/script/types.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

void Placeable::loadFromGIT(const Gff &gffs) {
    std::string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);
    loadTransformFromGIT(gffs);
}

void Placeable::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> utp(_services.resource.gffs.get(resRef, ResourceType::Utp));
    if (!utp) {
        return;
    }
    loadUTP(*utp);
    std::shared_ptr<TwoDa> placeables(_services.resource.twoDas.get("placeables"));
    std::string modelName(boost::to_lower_copy(placeables->getString(_appearance, "modelname")));

    auto model = _services.graphics.models.get(modelName);
    if (!model) {
        return;
    }
    auto &sceneGraph = _services.scene.graphs.get(_sceneName);

    auto sceneNode = sceneGraph.newModel(*model, ModelUsage::Placeable);
    sceneNode->setUser(*this);
    sceneNode->setCullable(true);
    sceneNode->setDrawDistance(_game.options().graphics.drawDistance);
    _sceneNode = std::move(sceneNode);

    auto walkmesh = _services.graphics.walkmeshes.get(modelName, ResourceType::Pwk);
    if (walkmesh) {
        _walkmesh = sceneGraph.newWalkmesh(*walkmesh);
    }
}

void Placeable::loadTransformFromGIT(const Gff &gffs) {
    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, gffs.getFloat("Bearing")));

    updateTransform();
}

void Placeable::runOnUsed(std::shared_ptr<Object> usedBy) {
    if (!_onUsed.empty()) {
        _game.scriptRunner().run(_onUsed, _id, usedBy ? usedBy->id() : kObjectInvalid);
    }
}

void Placeable::runOnInvDisturbed(std::shared_ptr<Object> triggerrer) {
    if (!_onInvDisturbed.empty()) {
        _game.scriptRunner().run(_onInvDisturbed, _id, triggerrer ? triggerrer->id() : kObjectInvalid);
    }
}

void Placeable::loadUTP(const Gff &utp) {
    _utp = std::make_unique<UTP>(parseUTP(utp));

    _tag = boost::to_lower_copy(utp.getString("Tag"));
    _name = _services.resource.strings.get(utp.getInt("LocName"));
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
        std::string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
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

void Placeable::updateTransform() {
    Object::updateTransform();

    if (_walkmesh) {
        _walkmesh->setLocalTransform(_transform);
    }
}

} // namespace game

} // namespace reone
