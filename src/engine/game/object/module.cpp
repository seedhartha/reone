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

#include "module.h"

#include "../../common/log.h"
#include "../../resource/resources.h"

#include "../game.h"
#include "../reputes.h"

#include "objectfactory.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr int kMaxMillisecond = 1000;
static constexpr int kMaxSecond = 60;
static constexpr int kMaxMinute = 60;
static constexpr int kMaxHour = 24;

Module::Module(uint32_t id, Game *game) : Object(id, ObjectType::Module, game) {
}

void Module::load(const string &name, const GffStruct &ifo) {
    _name = name;

    loadInfo(ifo);
    loadArea(ifo);

    _area->initCameras(_info.entryPosition, _info.entryFacing);
    _area->runSpawnScripts();

    loadPlayer();
}

void Module::loadInfo(const GffStruct &ifo) {
    // Entry location

    _info.entryArea = ifo.getString("Mod_Entry_Area");

    _info.entryPosition.x = ifo.getFloat("Mod_Entry_X");
    _info.entryPosition.y = ifo.getFloat("Mod_Entry_Y");
    _info.entryPosition.z = ifo.getFloat("Mod_Entry_Z");

    float dirX = ifo.getFloat("Mod_Entry_Dir_X");
    float dirY = ifo.getFloat("Mod_Entry_Dir_Y");
    _info.entryFacing = -glm::atan(dirX, dirY);

    // END Entry location

    // Time

    _info.dawnHour = ifo.getInt("Mod_DawnHour");
    _info.duskHour = ifo.getInt("Mod_DuskHour");
    _info.minPerHour = ifo.getInt("Mod_MinPerHour");

    _time.hour = ifo.getInt("Mod_StartHour");

    // END Time
}

void Module::loadArea(const GffStruct &ifo) {
    reone::info("Load area " + _info.entryArea);

    shared_ptr<GffStruct> are(_game->resources().getGFF(_info.entryArea, ResourceType::Are));
    shared_ptr<GffStruct> git(_game->resources().getGFF(_info.entryArea, ResourceType::Git));

    _area = _game->objectFactory().newArea();
    _area->load(_info.entryArea, *are, *git);
}

void Module::loadPlayer() {
    _player = make_unique<Player>(this, _area.get(), &_area->getCamera(CameraType::ThirdPerson), &_game->party());
}

void Module::loadParty(const string &entry) {
    glm::vec3 position(0.0f);
    float facing = 0.0f;
    getEntryPoint(entry, position, facing);

    _area->loadParty(position, facing);
    _area->onPartyLeaderMoved();
    _area->update3rdPersonCameraFacing();
    _area->runOnEnterScript();
}

void Module::getEntryPoint(const string &waypoint, glm::vec3 &position, float &facing) const {
    position = _info.entryPosition;
    facing = _info.entryFacing;

    if (!waypoint.empty()) {
        shared_ptr<SpatialObject> object(_area->getObjectByTag(waypoint));
        if (object) {
            position = object->position();
            facing = object->getFacing();
        }
    }
}

bool Module::handle(const SDL_Event &event) {
    if (_player->handle(event)) return true;
    if (_area->handle(event)) return true;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            if (handleMouseMotion(event.motion)) return true;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (handleMouseButtonDown(event.button)) return true;
            break;
        case SDL_KEYDOWN:
            if (handleKeyDown(event.key)) return true;
            break;
        default:
            break;
    }

    return false;
}

bool Module::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    CursorType cursor = CursorType::Default;

    shared_ptr<SpatialObject> object(_area->getObjectAt(event.x, event.y));
    if (object && object->isSelectable()) {
        _area->hilightObject(object);

        switch (object->type()) {
            case ObjectType::Creature: {
                if (object->isDead()) {
                    cursor = CursorType::Pickup;
                } else {
                    auto creature = static_pointer_cast<Creature>(object);
                    bool isEnemy = _game->reputes().getIsEnemy(*creature, *_game->party().getLeader());
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

    _game->setCursorType(cursor);

    return true;
}

bool Module::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button != SDL_BUTTON_LEFT) return false;

    shared_ptr<SpatialObject> object(_area->getObjectAt(event.x, event.y));
    if (!object || !object->isSelectable()) return false;

    auto selectedObject = _area->selectedObject();
    if (object != selectedObject) {
        _area->selectObject(object);
        return true;
    }
    onObjectClick(object);

    return true;
}

void Module::onObjectClick(const shared_ptr<SpatialObject> &object) {
    switch (object->type()) {
        case ObjectType::Creature:
            onCreatureClick(static_pointer_cast<Creature>(object));
            break;
        case ObjectType::Door:
            onDoorClick(static_pointer_cast<Door>(object));
            break;
        case ObjectType::Placeable:
            onPlaceableClick(static_pointer_cast<Placeable>(object));
            break;
    }
}

void Module::onCreatureClick(const shared_ptr<Creature> &creature) {
    debug(boost::format("Module: click: creature '%s', faction %d") % creature->tag() % static_cast<int>(creature->faction()));

    shared_ptr<Creature> partyLeader(_game->party().getLeader());

    if (creature->isDead()) {
        if (!creature->items().empty()) {
            partyLeader->clearAllActions();
            partyLeader->addAction(make_unique<ObjectAction>(ActionType::OpenContainer, creature));
        }
    } else {
        bool isEnemy = _game->reputes().getIsEnemy(*partyLeader, *creature);
        if (isEnemy) {
            partyLeader->clearAllActions();
            partyLeader->addAction(make_unique<AttackAction>(creature));
        } else if (!creature->conversation().empty()) {
            partyLeader->clearAllActions();
            partyLeader->addAction(make_unique<StartConversationAction>(creature, creature->conversation()));
        }
    }
}

void Module::onDoorClick(const shared_ptr<Door> &door) {
    if (!door->linkedToModule().empty()) {
        _game->scheduleModuleTransition(door->linkedToModule(), door->linkedTo());
        return;
    }
    if (!door->isOpen()) {
        shared_ptr<Creature> partyLeader(_game->party().getLeader());
        partyLeader->clearAllActions();
        partyLeader->addAction(make_unique<ObjectAction>(ActionType::OpenDoor, door));
    }
}

void Module::onPlaceableClick(const shared_ptr<Placeable> &placeable) {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());

    if (placeable->hasInventory()) {
        partyLeader->clearAllActions();
        partyLeader->addAction(make_unique<ObjectAction>(ActionType::OpenContainer, placeable));
    } else if (!placeable->conversation().empty()) {
        partyLeader->clearAllActions();
        partyLeader->addAction(make_unique<StartConversationAction>(placeable, placeable->conversation()));
    } else {
        placeable->runOnUsed(move(partyLeader));
    }
}

void Module::update(float dt) {
    if (_game->cameraType() == CameraType::ThirdPerson) {
        _player->update(dt);
    }
    _area->update(dt);
}

set<ContextualAction> Module::getContextualActions(const shared_ptr<Object> &object) const {
    set<ContextualAction> actions;

    switch (object->type()) {
        case ObjectType::Creature: {
            auto leader = _game->party().getLeader();
            auto creature = static_pointer_cast<Creature>(object);
            if (!creature->isDead() && _game->reputes().getIsEnemy(*leader, *creature)) {
                actions.insert(ContextualAction::Attack);
                auto weapon = leader->getEquippedItem(InventorySlot::rightWeapon);
                if (weapon && weapon->isRanged()) {
                    if (leader->attributes().hasFeat(FeatType::PowerBlast)) {
                        actions.insert(ContextualAction::PowerShot);
                    } else if (leader->attributes().hasFeat(FeatType::SniperShot)) {
                        actions.insert(ContextualAction::SniperShot);
                    } else if (leader->attributes().hasFeat(FeatType::RapidShot)) {
                        actions.insert(ContextualAction::RapidShot);
                    }
                } else {
                    if (leader->attributes().hasFeat(FeatType::PowerAttack)) {
                        actions.insert(ContextualAction::PowerAttack);
                    } else if (leader->attributes().hasFeat(FeatType::CriticalStrike)) {
                        actions.insert(ContextualAction::CriticalStrike);
                    } else if (leader->attributes().hasFeat(FeatType::Flurry)) {
                        actions.insert(ContextualAction::Flurry);
                    }
                }
            }
            break;
        }
        case ObjectType::Door: {
            auto door = static_pointer_cast<Door>(object);
            if (door->isLocked() && !door->isKeyRequired() && _game->party().getLeader()->attributes().hasSkill(Skill::Security)) {
                actions.insert(ContextualAction::Unlock);
            }
            break;
        }
        default:
            break;
    }

    return move(actions);
}

bool Module::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.sym) {
        case SDLK_SPACE: {
            bool paused = !_game->isPaused();
            _game->setPaused(paused);
            return true;
        }
        default:
            return false;
    }
}

void Module::setTime(int hour, int minute, int second, int millisecond) {
    if (millisecond <= _time.millisecond) {
        _time.millisecond = millisecond;
    } else {
        _time.millisecond = millisecond % kMaxMillisecond;
        second += millisecond / kMaxMillisecond;
    }
    if (second <= _time.second) {
        _time.second = second;
    } else {
        _time.second = second % kMaxSecond;
        minute += second / kMaxSecond;
    }
    if (minute <= _time.minute) {
        _time.minute = minute;
    } else {
        _time.minute = minute % kMaxMinute;
        hour += minute / kMaxMinute;
    }
    if (hour <= _time.hour) {
        _time.hour = hour;
    } else {
        _time.hour = hour % kMaxHour;
        _time.day += hour / kMaxHour;
    }
}

} // namespace game

} // namespace reone
