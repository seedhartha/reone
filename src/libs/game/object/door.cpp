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

#include "reone/game/object/door.h"

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
#include "reone/scene/types.h"
#include "reone/script/scripts.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

void Door::loadFromGIT(const generated::GIT_Door_List &git) {
    std::string templateResRef(boost::to_lower_copy(git.TemplateResRef));
    loadFromBlueprint(templateResRef);

    _linkedToModule = boost::to_lower_copy(git.LinkedToModule);
    _linkedTo = boost::to_lower_copy(git.LinkedTo);
    _linkedToFlags = git.LinkedToFlags;
    _transitionDestin = _services.resource.strings.getText(git.TransitionDestin.first);

    loadTransformFromGIT(git);
}

void Door::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> utd(_services.resource.gffs.get(resRef, ResType::Utd));
    if (!utd) {
        return;
    }
    auto utdParsed = generated::parseUTD(*utd);
    loadUTD(utdParsed);
    std::shared_ptr<TwoDa> doors(_services.resource.twoDas.get("genericdoors"));
    std::string modelName(boost::to_lower_copy(doors->getString(_genericType, "modelname")));

    auto model = _services.graphics.models.get(modelName);
    if (!model) {
        return;
    }
    auto &sceneGraph = _services.scene.graphs.get(_sceneName);

    auto modelSceneNode = sceneGraph.newModel(*model, ModelUsage::Door);
    modelSceneNode->setUser(*this);
    modelSceneNode->setCullable(true);
    // modelSceneNode->setDrawDistance(_game.options().graphics.drawDistance);
    _sceneNode = std::move(modelSceneNode);

    auto walkmeshClosed = _services.graphics.walkmeshes.get(modelName + "0", ResType::Dwk);
    if (walkmeshClosed) {
        _walkmeshClosed = sceneGraph.newWalkmesh(*walkmeshClosed);
        _walkmeshClosed->setUser(*this);
    }

    auto walkmeshOpen1 = _services.graphics.walkmeshes.get(modelName + "1", ResType::Dwk);
    if (walkmeshOpen1) {
        _walkmeshOpen1 = sceneGraph.newWalkmesh(*walkmeshOpen1);
        _walkmeshOpen1->setUser(*this);
        _walkmeshOpen1->setEnabled(false);
    }

    auto walkmeshOpen2 = _services.graphics.walkmeshes.get(modelName + "2", ResType::Dwk);
    if (walkmeshOpen2) {
        _walkmeshOpen2 = sceneGraph.newWalkmesh(*walkmeshOpen2);
        _walkmeshOpen2->setUser(*this);
        _walkmeshOpen2->setEnabled(false);
    }
}

void Door::loadTransformFromGIT(const generated::GIT_Door_List &git) {
    _position[0] = git.X;
    _position[1] = git.Y;
    _position[2] = git.Z;

    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, git.Bearing));

    updateTransform();
}

bool Door::isSelectable() const {
    return !_static && !_open;
}

void Door::open(const std::shared_ptr<Object> &triggerrer) {
    auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
    if (model) {
        // model->setDefaultAnimation("opened1", AnimationProperties::fromFlags(AnimationFlags::loop));
        model->playAnimation("opening1");
    }
    if (_walkmeshOpen1) {
        _walkmeshOpen1->setEnabled(true);
    }
    if (_walkmeshOpen2) {
        _walkmeshOpen2->setEnabled(false);
    }
    if (_walkmeshClosed) {
        _walkmeshClosed->setEnabled(false);
    }
    _open = true;
}

void Door::close(const std::shared_ptr<Object> &triggerrer) {
    auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
    if (model) {
        // model->setDefaultAnimation("closed", AnimationProperties::fromFlags(AnimationFlags::loop));
        model->playAnimation("closing1");
    }
    if (_walkmeshOpen1) {
        _walkmeshOpen1->setEnabled(false);
    }
    if (_walkmeshOpen2) {
        _walkmeshOpen2->setEnabled(false);
    }
    if (_walkmeshClosed) {
        _walkmeshClosed->setEnabled(true);
    }
    _open = false;
}

void Door::setLocked(bool locked) {
    _locked = locked;
}

void Door::loadUTD(const generated::UTD &utd) {
    _tag = boost::to_lower_copy(utd.Tag);
    _name = _services.resource.strings.getText(utd.LocName.first);
    _blueprintResRef = boost::to_lower_copy(utd.TemplateResRef);
    _autoRemoveKey = utd.AutoRemoveKey;
    _conversation = boost::to_lower_copy(utd.Conversation);
    _interruptable = utd.Interruptable;
    _faction = static_cast<Faction>(utd.Faction);
    _plot = utd.Plot;
    _minOneHP = utd.Min1HP;
    _keyRequired = utd.KeyRequired;
    _lockable = utd.Lockable;
    _locked = utd.Locked;
    _openLockDC = utd.OpenLockDC;
    _keyName = utd.KeyName;
    _hitPoints = utd.HP;
    _currentHitPoints = utd.CurrentHP;
    _hardness = utd.Hardness;
    _fortitude = utd.Fort;
    _genericType = utd.GenericType;
    _static = utd.Static;

    _onClosed = utd.OnClosed;   // always empty, but could be useful
    _onDamaged = utd.OnDamaged; // always empty, but could be useful
    _onDeath = utd.OnDeath;
    _onHeartbeat = utd.OnHeartbeat;
    _onLock = utd.OnLock;                   // always empty, but could be useful
    _onMeleeAttacked = utd.OnMeleeAttacked; // always empty, but could be useful
    _onOpen = utd.OnOpen;
    _onSpellCastAt = utd.OnSpellCastAt; // always empty, but could be useful
    _onUnlock = utd.OnUnlock;           // always empty, but could be useful
    _onUserDefined = utd.OnUserDefined;
    _onClick = utd.OnClick;
    _onFailToOpen = utd.OnFailToOpen;

    // Unused fields:
    //
    // - Description (always -1)
    // - CloseLockDC (always 0)
    // - PortraitId (not applicable, mostly 0)
    // - TrapDetectable (not applicable, always 1)
    // - TrapDetectDC (not applicable, always 0)
    // - TrapDisarmable (not applicable, always 1)
    // - DisarmDC (not applicable, mostly 28)
    // - TrapFlag (not applicable, always 0)
    // - TrapOneShot (not applicable, always 1)
    // - TrapType (not applicable)
    // - AnimationState (always 0)
    // - Appearance (always 0)
    // - Ref (always 0)
    // - Will (always 0)
    // - OnDisarm (not applicable, always empty)
    // - OnTrapTriggered (not applicable, always empty)
    // - LoadScreenID (always 0)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Door::updateTransform() {
    Object::updateTransform();

    if (_walkmeshOpen1) {
        _walkmeshOpen1->setLocalTransform(_transform);
    }
    if (_walkmeshOpen2) {
        _walkmeshOpen2->setLocalTransform(_transform);
    }
    if (_walkmeshClosed) {
        _walkmeshClosed->setLocalTransform(_transform);
    }
}

} // namespace game

} // namespace reone
