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

#include "reone/game/object/module.h"

#include "reone/game/action/attackobject.h"
#include "reone/game/action/opencontainer.h"
#include "reone/game/action/opendoor.h"
#include "reone/game/action/startconversation.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/party.h"
#include "reone/game/reputes.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/resources.h"
#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Module::load(std::string name, const Gff &ifo, bool fromSave) {
    _name = std::move(name);

    auto ifoParsed = resource::generated::parseIFO(ifo);
    loadInfo(ifoParsed);
    loadArea(ifoParsed);

    _area->initCameras(_info.entryPosition, _info.entryFacing);

    loadPlayer();

    if (!fromSave) {
        _area->runSpawnScripts();
    }
}

void Module::loadInfo(const resource::generated::IFO &ifo) {
    // Entry location

    _info.entryArea = ifo.Mod_Entry_Area;
    if (_info.entryArea.empty()) {
        throw ValidationException("Mod_Entry_Area must not be empty");
    }

    _info.entryPosition.x = ifo.Mod_Entry_X;
    _info.entryPosition.y = ifo.Mod_Entry_Y;
    _info.entryPosition.z = ifo.Mod_Entry_Z;

    float dirX = ifo.Mod_Entry_Dir_X;
    float dirY = ifo.Mod_Entry_Dir_Y;
    _info.entryFacing = -glm::atan(dirX, dirY);
}

void Module::loadArea(const resource::generated::IFO &ifo, bool fromSave) {
    reone::info("Load area '" + _info.entryArea + "'");

    _area = _game.newArea();

    std::shared_ptr<Gff> are(_services.resource.gffs.get(_info.entryArea, ResType::Are));
    if (!are) {
        throw ResourceNotFoundException("Area ARE not found: " + _info.entryArea);
    }

    std::shared_ptr<Gff> git(_services.resource.gffs.get(_info.entryArea, ResType::Git));
    if (!git) {
        throw ResourceNotFoundException("Area GIT not found: " + _info.entryArea);
    }

    _area->load(_info.entryArea, *are, *git, fromSave);
}

void Module::loadPlayer() {
    _player = std::make_unique<Player>(*this, *_area, *_area->getCamera(CameraType::ThirdPerson), _game.party());
}

void Module::loadParty(const std::string &entry, bool fromSave) {
    glm::vec3 position(0.0f);
    float facing = 0.0f;
    getEntryPoint(entry, position, facing);

    _area->loadParty(position, facing, fromSave);
    _area->onPartyLeaderMoved(true);
    _area->update3rdPersonCameraFacing();

    if (!fromSave) {
        _area->runOnEnterScript();
    }
}

void Module::getEntryPoint(const std::string &waypoint, glm::vec3 &position, float &facing) const {
    position = _info.entryPosition;
    facing = _info.entryFacing;

    if (!waypoint.empty()) {
        std::shared_ptr<Object> object(_area->getObjectByTag(waypoint));
        if (object) {
            position = object->position();
            facing = object->getFacing();
        }
    }
}

bool Module::handle(const SDL_Event &event) {
    if (_player && _player->handle(event))
        return true;
    if (_area->handle(event))
        return true;

    switch (event.type) {
    case SDL_MOUSEMOTION:
        if (handleMouseMotion(event.motion))
            return true;
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (handleMouseButtonDown(event.button))
            return true;
        break;
    case SDL_KEYDOWN:
        if (handleKeyDown(event.key))
            return true;
        break;
    default:
        break;
    }

    return false;
}

bool Module::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    CursorType cursor = CursorType::Default;

    auto object = _area->getObjectAt(event.x, event.y);
    if (object && object->isSelectable()) {
        auto objectPtr = _game.getObjectById(object->id());
        _area->hilightObject(objectPtr);

        switch (object->type()) {
        case ObjectType::Creature: {
            if (object->isDead()) {
                cursor = CursorType::Pickup;
            } else {
                auto creature = static_cast<Creature *>(object);
                bool isEnemy = _services.game.reputes.getIsEnemy(*creature, *_game.party().getLeader());
                cursor = isEnemy ? CursorType::Attack : CursorType::Talk;
            }
            break;
        }
        case ObjectType::Door:
            cursor = CursorType::Door;
            break;
        case ObjectType::Placeable:
            cursor = CursorType::Pickup;
            break;
        default:
            break;
        }
    } else {
        _area->hilightObject(nullptr);
    }

    _game.setCursorType(cursor);

    return true;
}

bool Module::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button != SDL_BUTTON_LEFT) {
        return false;
    }
    auto object = _area->getObjectAt(event.x, event.y);
    if (!object || !object->isSelectable()) {
        return false;
    }
    auto objectPtr = _game.getObjectById(object->id());
    auto selectedObject = _area->selectedObject();
    if (objectPtr != selectedObject) {
        _area->selectObject(objectPtr);
        return true;
    }
    onObjectClick(objectPtr);

    return true;
}

void Module::onObjectClick(const std::shared_ptr<Object> &object) {
    switch (object->type()) {
    case ObjectType::Creature:
        onCreatureClick(std::static_pointer_cast<Creature>(object));
        break;
    case ObjectType::Door:
        onDoorClick(std::static_pointer_cast<Door>(object));
        break;
    case ObjectType::Placeable:
        onPlaceableClick(std::static_pointer_cast<Placeable>(object));
        break;
    default:
        break;
    }
}

void Module::onCreatureClick(const std::shared_ptr<Creature> &creature) {
    debug(str(boost::format("Module: click: creature '%s', faction %d") % creature->tag() % static_cast<int>(creature->faction())));

    std::shared_ptr<Creature> partyLeader(_game.party().getLeader());

    if (creature->isDead()) {
        if (!creature->items().empty()) {
            partyLeader->clearAllActions();
            partyLeader->addAction(_game.newAction<OpenContainerAction>(creature));
        }
    } else {
        bool isEnemy = _services.game.reputes.getIsEnemy(*partyLeader, *creature);
        if (isEnemy) {
            partyLeader->clearAllActions();
            auto action = _game.newAction<AttackObjectAction>(creature);
            action->setUserAction(true);
            partyLeader->addAction(std::move(action));
        } else if (!creature->conversation().empty()) {
            partyLeader->clearAllActions();
            partyLeader->addAction(_game.newAction<StartConversationAction>(creature, creature->conversation()));
        }
    }
}

void Module::onDoorClick(const std::shared_ptr<Door> &door) {
    if (!door->linkedToModule().empty()) {
        _game.scheduleModuleTransition(door->linkedToModule(), door->linkedTo());
        return;
    }
    if (!door->isOpen()) {
        std::shared_ptr<Creature> partyLeader(_game.party().getLeader());
        partyLeader->clearAllActions();
        partyLeader->addAction(_game.newAction<OpenDoorAction>(door));
    }
}

void Module::onPlaceableClick(const std::shared_ptr<Placeable> &placeable) {
    std::shared_ptr<Creature> partyLeader(_game.party().getLeader());

    if (placeable->hasInventory()) {
        partyLeader->clearAllActions();
        partyLeader->addAction(_game.newAction<OpenContainerAction>(placeable));
    } else if (!placeable->conversation().empty()) {
        partyLeader->clearAllActions();
        partyLeader->addAction(_game.newAction<StartConversationAction>(placeable, placeable->conversation()));
    } else {
        placeable->runOnUsed(std::move(partyLeader));
    }
}

void Module::update(float dt) {
    if (_game.cameraType() == CameraType::ThirdPerson) {
        _player->update(dt);
    }
    _area->update(dt);
}

std::vector<ContextAction> Module::getContextActions(const std::shared_ptr<Object> &object) const {
    std::vector<ContextAction> actions;

    switch (object->type()) {
    case ObjectType::Creature: {
        auto leader = _game.party().getLeader();
        auto creature = std::static_pointer_cast<Creature>(object);
        if (!creature->isDead() && _services.game.reputes.getIsEnemy(*leader, *creature)) {
            actions.push_back(ContextAction(ActionType::AttackObject));
            auto weapon = leader->getEquippedItem(InventorySlot::rightWeapon);
            if (weapon && weapon->isRanged()) {
                if (leader->attributes().hasFeat(FeatType::MasterPowerBlast)) {
                    actions.push_back(ContextAction(FeatType::MasterPowerBlast));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedPowerBlast)) {
                    actions.push_back(ContextAction(FeatType::ImprovedPowerBlast));
                } else if (leader->attributes().hasFeat(FeatType::PowerBlast)) {
                    actions.push_back(ContextAction(FeatType::PowerBlast));
                }
                if (leader->attributes().hasFeat(FeatType::MasterSniperShot)) {
                    actions.push_back(ContextAction(FeatType::MasterSniperShot));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedSniperShot)) {
                    actions.push_back(ContextAction(FeatType::ImprovedSniperShot));
                } else if (leader->attributes().hasFeat(FeatType::SniperShot)) {
                    actions.push_back(ContextAction(FeatType::SniperShot));
                }
                if (leader->attributes().hasFeat(FeatType::MultiShot)) {
                    actions.push_back(ContextAction(FeatType::MultiShot));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedRapidShot)) {
                    actions.push_back(ContextAction(FeatType::ImprovedRapidShot));
                } else if (leader->attributes().hasFeat(FeatType::RapidShot)) {
                    actions.push_back(ContextAction(FeatType::RapidShot));
                }
            } else {
                if (leader->attributes().hasFeat(FeatType::MasterPowerAttack)) {
                    actions.push_back(ContextAction(FeatType::MasterPowerAttack));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedPowerAttack)) {
                    actions.push_back(ContextAction(FeatType::ImprovedPowerAttack));
                } else if (leader->attributes().hasFeat(FeatType::PowerAttack)) {
                    actions.push_back(ContextAction(FeatType::PowerAttack));
                }
                if (leader->attributes().hasFeat(FeatType::MasterCriticalStrike)) {
                    actions.push_back(ContextAction(FeatType::MasterCriticalStrike));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedCriticalStrike)) {
                    actions.push_back(ContextAction(FeatType::ImprovedCriticalStrike));
                } else if (leader->attributes().hasFeat(FeatType::CriticalStrike)) {
                    actions.push_back(ContextAction(FeatType::CriticalStrike));
                }
                if (leader->attributes().hasFeat(FeatType::WhirlwindAttack)) {
                    actions.push_back(ContextAction(FeatType::WhirlwindAttack));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedFlurry)) {
                    actions.push_back(ContextAction(FeatType::ImprovedFlurry));
                } else if (leader->attributes().hasFeat(FeatType::Flurry)) {
                    actions.push_back(ContextAction(FeatType::Flurry));
                }
            }
        }
        break;
    }
    case ObjectType::Door: {
        auto door = std::static_pointer_cast<Door>(object);
        if (door->isLocked() && !door->isKeyRequired() && _game.party().getLeader()->attributes().hasSkill(SkillType::Security)) {
            actions.push_back(ContextAction(SkillType::Security));
        }
        break;
    }
    default:
        break;
    }

    return actions;
}

bool Module::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.sym) {
    case SDLK_SPACE: {
        bool paused = !_game.isPaused();
        _game.setPaused(paused);
        return true;
    }
    default:
        return false;
    }
}

} // namespace game

} // namespace reone
