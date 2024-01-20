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
#include "reone/resource/2da.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/walkmeshes.h"
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

void Placeable::loadFromGIT(const resource::generated::GIT_Placeable_List &git) {
    std::string templateResRef(boost::to_lower_copy(git.TemplateResRef));
    loadFromBlueprint(templateResRef);
    loadTransformFromGIT(git);
}

void Placeable::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> utp(_services.resource.gffs.get(resRef, ResType::Utp));
    if (!utp) {
        return;
    }
    auto utpParsed = resource::generated::parseUTP(*utp);
    loadUTP(utpParsed);
    std::shared_ptr<TwoDA> placeables(_services.resource.twoDas.get("placeables"));
    std::string modelName(boost::to_lower_copy(placeables->getString(_appearance, "modelname")));

    auto model = _services.resource.models.get(modelName);
    if (!model) {
        return;
    }
    auto &sceneGraph = _services.scene.graphs.get(_sceneName);

    auto sceneNode = sceneGraph.newModel(*model, ModelUsage::Placeable);
    sceneNode->setUser(*this);
    sceneNode->setDrawDistance(_game.options().graphics.drawDistance);
    _sceneNode = std::move(sceneNode);

    auto walkmesh = _services.resource.walkmeshes.get(modelName, ResType::Pwk);
    if (walkmesh) {
        _walkmesh = sceneGraph.newWalkmesh(*walkmesh);
    }
}

void Placeable::loadTransformFromGIT(const resource::generated::GIT_Placeable_List &git) {
    _position[0] = git.X;
    _position[1] = git.Y;
    _position[2] = git.Z;

    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, git.Bearing));

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

void Placeable::loadUTP(const resource::generated::UTP &utp) {
    _tag = boost::to_lower_copy(utp.Tag);
    _name = _services.resource.strings.getText(utp.LocName.first);
    _blueprintResRef = boost::to_lower_copy(utp.TemplateResRef);
    _conversation = boost::to_lower_copy(utp.Conversation);
    _interruptable = utp.Interruptable;
    _faction = static_cast<Faction>(utp.Faction);
    _plot = utp.Plot;
    _minOneHP = utp.Min1HP;
    _keyRequired = utp.KeyRequired;
    _lockable = utp.Lockable;
    _locked = utp.Locked;
    _openLockDC = utp.OpenLockDC;
    _animationState = utp.AnimationState;
    _appearance = utp.Appearance;
    _hitPoints = utp.HP;
    _currentHitPoints = utp.CurrentHP;
    _hardness = utp.Hardness;
    _fortitude = utp.Fort;
    _hasInventory = utp.HasInventory;
    _partyInteract = utp.PartyInteract;
    _static = utp.Static;
    _usable = utp.Useable;

    _onClosed = boost::to_lower_copy(utp.OnClosed);
    _onDamaged = boost::to_lower_copy(utp.OnDamaged); // always empty, but could be useful
    _onDeath = boost::to_lower_copy(utp.OnDeath);
    _onHeartbeat = boost::to_lower_copy(utp.OnHeartbeat);
    _onLock = boost::to_lower_copy(utp.OnLock);                   // always empty, but could be useful
    _onMeleeAttacked = boost::to_lower_copy(utp.OnMeleeAttacked); // always empty, but could be useful
    _onOpen = boost::to_lower_copy(utp.OnOpen);
    _onSpellCastAt = boost::to_lower_copy(utp.OnSpellCastAt);
    _onUnlock = boost::to_lower_copy(utp.OnUnlock); // always empty, but could be useful
    _onUserDefined = boost::to_lower_copy(utp.OnUserDefined);
    _onEndDialogue = boost::to_lower_copy(utp.OnEndDialogue);
    _onInvDisturbed = boost::to_lower_copy(utp.OnInvDisturbed);
    _onUsed = boost::to_lower_copy(utp.OnUsed);

    for (auto &itemStrct : utp.ItemList) {
        std::string resRef(boost::to_lower_copy(itemStrct.InventoryRes));
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
