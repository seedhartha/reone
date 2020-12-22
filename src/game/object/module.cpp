/*
 * Copyright (c) 2020 The reone project contributors
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
#include "../rp/factionutil.h"

#include "objectfactory.h"

using namespace std;

using namespace reone::net;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

Module::Module(uint32_t id, Game *game) :
    Object(id, ObjectType::Module),
    _game(game) {

    if (!game) {
        throw invalid_argument("game must not be null");
    }
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
    _info.entryPosition.x = ifo.getFloat("Mod_Entry_X");
    _info.entryPosition.y = ifo.getFloat("Mod_Entry_Y");
    _info.entryPosition.z = ifo.getFloat("Mod_Entry_Z");

    float dirX = ifo.getFloat("Mod_Entry_Dir_X");
    float dirY = ifo.getFloat("Mod_Entry_Dir_Y");
    _info.entryFacing = -glm::atan(dirX, dirY);

    _info.entryArea = ifo.getString("Mod_Entry_Area");
}

void Module::loadArea(const GffStruct &ifo) {
    reone::info("Module: load area: " + _info.entryArea);

    shared_ptr<GffStruct> are(Resources::instance().getGFF(_info.entryArea, ResourceType::Area));
    shared_ptr<GffStruct> git(Resources::instance().getGFF(_info.entryArea, ResourceType::GameInstance));

    shared_ptr<Area> area(_game->objectFactory().newArea());
    area->load(_info.entryArea, *are, *git);
    _area = move(area);
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
        shared_ptr<SpatialObject> object(_area->find(waypoint));
        if (object) {
            position = object->position();
            facing = object->facing();
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
        default:
            break;
    }

    return false;
}

bool Module::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    CursorType cursor = CursorType::Default;

    shared_ptr<SpatialObject> object(_area->getObjectAt(event.x, event.y));
    if (object && object->isSelectable()) {
        _area->objectSelector().hilight(object);

        switch (object->type()) {
            case ObjectType::Creature: {
                if (object->isDead()) {
                    cursor = CursorType::Pickup;
                } else {
                    auto creature = static_pointer_cast<Creature>(object);
                    bool isEnemy = getIsEnemy(*creature, *_game->party().leader());
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
        _area->objectSelector().hilight(nullptr);
    }

    _game->setCursorType(cursor);

    return true;
}

bool Module::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button != SDL_BUTTON_LEFT) return false;

    shared_ptr<SpatialObject> object(_area->getObjectAt(event.x, event.y));
    if (!object || !object->isSelectable()) return false;

    auto selectedObject = _area->objectSelector().selectedObject();
    if (object != selectedObject) {
        _area->objectSelector().select(object);
        return true;
    }
    onObjectClick(object);

    return true;
}

void Module::onObjectClick(const shared_ptr<SpatialObject> &object) {
    shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
    if (creature) {
        onCreatureClick(creature);
        return;
    }
    debug(boost::format("Module: click: object '%s'") % object->tag());

    shared_ptr<Door> door(dynamic_pointer_cast<Door>(object));
    if (door) {
        onDoorClick(door);
        return;
    }
    shared_ptr<Placeable> placeable(dynamic_pointer_cast<Placeable>(object));
    if (placeable) {
        onPlaceableClick(placeable);
        return;
    }
}

void Module::onCreatureClick(const shared_ptr<Creature> &creature) {
    debug(boost::format("Module: click: creature '%s', faction %d") % creature->tag() % static_cast<int>(creature->faction()));

    shared_ptr<Creature> partyLeader(_game->party().leader());
    ActionQueue &actions = partyLeader->actionQueue();

    if (creature->isDead()) {
        if (!creature->items().empty()) {
            actions.clear();
            actions.add(make_unique<ObjectAction>(ActionType::OpenContainer, creature));
        }
    } else {
        bool isEnemy = getIsEnemy(*partyLeader, *creature);
        if (isEnemy) {
            actions.clear();
            actions.add(make_unique<AttackAction>(creature));
        } else if (!creature->conversation().empty()) {
            actions.clear();
            actions.add(make_unique<StartConversationAction>(creature, creature->conversation()));
        }
    }
}

void Module::onDoorClick(const shared_ptr<Door> &door) {
    if (!door->linkedToModule().empty()) {
        _game->scheduleModuleTransition(door->linkedToModule(), door->linkedTo());
        return;
    }
    if (!door->isOpen()) {
        shared_ptr<Creature> partyLeader(_game->party().leader());
        ActionQueue &actions = partyLeader->actionQueue();
        actions.clear();
        actions.add(make_unique<ObjectAction>(ActionType::OpenDoor, door));
    }
}

void Module::onPlaceableClick(const shared_ptr<Placeable> &placeable) {
    shared_ptr<Creature> partyLeader(_game->party().leader());
    ActionQueue &actions = partyLeader->actionQueue();

    if (placeable->hasInventory()) {
        actions.clear();
        actions.add(make_unique<ObjectAction>(ActionType::OpenContainer, placeable));

    } else if (!placeable->conversation().empty()) {
        actions.clear();
        actions.add(make_unique<StartConversationAction>(placeable, placeable->conversation()));
    }
}

void Module::update(float dt) {
    if (_game->cameraType() == CameraType::ThirdPerson) {
        _player->update(dt);
    }
    _area->update(dt);
}

vector<ContextualAction> Module::getContextualActions(const shared_ptr<Object> &object) const {
    vector<ContextualAction> actions;

    shared_ptr<Door> door(dynamic_pointer_cast<Door>(object));
    if (door && door->isLocked()) {
        actions.push_back(ContextualAction::Unlock);
    }

    shared_ptr<Creature> hostile(dynamic_pointer_cast<Creature>(object));
    if (hostile && !hostile->isDead() && getIsEnemy(*(_game->party().leader()), *hostile)) {
        actions.push_back(ContextualAction::Attack);
    }
    return move(actions);
}

const string &Module::name() const {
    return _name;
}

const ModuleInfo &Module::info() const {
    return _info;
}

shared_ptr<Area> Module::area() const {
    return _area;
}

Player &Module::player() {
    return *_player;
}

} // namespace game

} // namespace reone
