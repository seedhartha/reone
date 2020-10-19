/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../system/debug.h"
#include "../../system/log.h"
#include "../../system/streamutil.h"
#include "../../system/resource/gfffile.h"
#include "../../system/resource/resources.h"

#include "door.h"
#include "objectfactory.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

Module::Module(uint32_t id, GameVersion version, ObjectFactory *objectFactory, SceneGraph *sceneGraph, const GraphicsOptions &opts) :
    Object(id, ObjectType::Module),
    _version(version),
    _objectFactory(objectFactory),
    _sceneGraph(sceneGraph),
    _opts(opts) {
}

void Module::load(const string &name, const GffStruct &ifo) {
    _name = name;

    loadInfo(ifo);
    loadArea(ifo);

    _area->loadCameras(_info.entryPosition, _info.entryHeading);

    loadPlayer();

    _loaded = true;
}

void Module::loadInfo(const GffStruct &ifo) {
    _info.entryPosition.x = ifo.getFloat("Mod_Entry_X");
    _info.entryPosition.y = ifo.getFloat("Mod_Entry_Y");
    _info.entryPosition.z = ifo.getFloat("Mod_Entry_Z");

    float dirX = ifo.getFloat("Mod_Entry_Dir_X");
    float dirY = ifo.getFloat("Mod_Entry_Dir_Y");
    _info.entryHeading = -glm::atan(dirX, dirY);

    _info.entryArea = ifo.getString("Mod_Entry_Area");
}

void Module::loadArea(const GffStruct &ifo) {
    reone::info("Module: load area: " + _info.entryArea);

    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<GffStruct> are(resources.findGFF(_info.entryArea, ResourceType::Area));
    shared_ptr<GffStruct> git(resources.findGFF(_info.entryArea, ResourceType::GameInstance));

    shared_ptr<Area> area(_objectFactory->newArea());
    area->setOnModuleTransition([this](const string &module, const string &entry) {
        if (_onModuleTransition) {
            _onModuleTransition(module, entry);
        }
    });
    area->setOnPlayerChanged([this]() {
        _area->update3rdPersonCameraTarget();
        _area->switchTo3rdPersonCamera();
    });
    area->setOnStartDialog([this](SpatialObject &object, const string &resRef) {
        if (!_startDialog) return;

        Creature &creature = static_cast<Creature &>(object);

        string finalResRef(resRef);
        if (resRef.empty()) finalResRef = creature.conversation();
        if (resRef.empty()) return;

        _startDialog(object, finalResRef);
    });
    area->load(_info.entryArea, *are, *git);
    _area = move(area);
}

void Module::loadPlayer() {
    _player = make_unique<Player>(this, _area.get(), _area->thirdPersonCamera());
}

void Module::loadParty(const PartyConfiguration &party, const string &entry) {
    _party = party;

    glm::vec3 position(0.0f);
    float heading = 0.0f;
    getEntryPoint(entry, position, heading);

    _area->loadParty(_party, position, heading);
    _area->updateRoomVisibility();

    _player->setCreature(static_cast<Creature *>(_area->player().get()));

    _area->update3rdPersonCameraTarget();
    _area->update3rdPersonCameraHeading();
    _area->switchTo3rdPersonCamera();

    _area->runOnEnterScript();
}

void Module::getEntryPoint(const string &waypoint, glm::vec3 &position, float &heading) const {
    position = _info.entryPosition;
    heading = _info.entryHeading;

    if (!waypoint.empty()) {
        shared_ptr<SpatialObject> object(_area->find(waypoint));
        if (object) {
            position = object->position();
            heading = object->heading();
        }
    }
}

bool Module::handle(const SDL_Event &event) {
    if (!_loaded) return false;

    if (_area->getCamera()->handle(event)) return true;
    if (_player->handle(event)) return true;
    if (_area->handle(event)) return true;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            if (handleMouseMotion(event.motion)) return true;
            break;
        case SDL_MOUSEBUTTONUP:
            if (handleMouseButtonUp(event.button)) return true;
            break;
        case SDL_KEYUP:
            if (handleKeyUp(event.key)) return true;
            break;
    }

    return false;
}

bool Module::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    const SpatialObject *object = _area->getObjectAt(event.x, event.y);
    _area->objectSelector().hilight(object ? object->id() : -1);

    return true;
}

bool Module::handleMouseButtonUp(const SDL_MouseButtonEvent &event) {
    SpatialObject *object = _area->getObjectAt(event.x, event.y);
    if (!object) {
        return false;
    }
    debug(boost::format("Object '%s' clicked on") % object->tag());

    uint32_t selectedObjectId = _area->objectSelector().selectedObjectId();
    if (object->id() != selectedObjectId) {
        _area->objectSelector().select(object->id());
        return true;
    }

    Door *door = dynamic_cast<Door *>(object);
    if (door) {
        if (!door->linkedToModule().empty()) {
            if (_onModuleTransition) {
                _onModuleTransition(door->linkedToModule(), door->linkedTo());
            }
        } else if (!door->isOpen() && !door->isStatic()) {
            shared_ptr<SpatialObject> player(_area->player());
            door->open(player);
        }
        return true;
    }

    Placeable *placeable = dynamic_cast<Placeable *>(object);
    if (placeable && placeable->blueprint().hasInventory()) {
        if (_openContainer) {
            _openContainer(placeable);
        }
        return true;
    }

    Creature *creature = dynamic_cast<Creature *>(object);
    if (creature) {
        if (!creature->conversation().empty() && _startDialog) {
            _player->stopMovement();
            _area->getCamera()->stopMovement();

            if (_startDialog) {
                _startDialog(*creature, creature->conversation());
            }
        }
        return true;
    }

    return true;
}

bool Module::handleKeyUp(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_V:
            _area->toggleCameraType();
            return true;

        case SDL_SCANCODE_LEFTBRACKET:
            cycleDebugMode(false);
            return true;

        case SDL_SCANCODE_RIGHTBRACKET:
            cycleDebugMode(true);
            return true;

        default:
            return false;
    }
}

void Module::cycleDebugMode(bool forward) {
    DebugMode mode = getDebugMode();
    switch (mode) {
        case DebugMode::None:
            mode = forward ? DebugMode::GameObjects : DebugMode::Path;
            break;
        case DebugMode::GameObjects:
            mode = forward ? DebugMode::ModelNodes : DebugMode::None;
            break;
        case DebugMode::ModelNodes:
            mode = forward ? DebugMode::Path : DebugMode::GameObjects;
            break;
        case DebugMode::Path:
        default:
            mode = forward ? DebugMode::None : DebugMode::ModelNodes;
            break;
    }
    setDebugMode(mode);
}

void Module::update(float dt, GuiContext &guiCtx) {
    if (!_loaded) return;

    Camera *camera = _area->getCamera();
    camera->update(dt);

    if (_area->cameraType() == CameraType::ThirdPerson) {
        _player->update(dt);
    }
    UpdateContext ctx;
    ctx.deltaTime = dt;
    ctx.cameraPosition = camera->sceneNode()->absoluteTransform()[3];
    ctx.projection = camera->sceneNode()->projection();
    ctx.view = camera->sceneNode()->view();

    _area->update(ctx);
    _area->fill(ctx, guiCtx);
}

void Module::setOnModuleTransition(const function<void(const string &, const string &)> &fn) {
    _onModuleTransition = fn;
}

void Module::setStartDialog(const function<void(SpatialObject &, const string &)> &fn) {
    _startDialog = fn;
}

void Module::setOpenContainer(const function<void(SpatialObject *)> &fn) {
    _openContainer = fn;
}

const string &Module::name() const {
    return _name;
}

bool Module::loaded() const {
    return _loaded;
}

const ModuleInfo &Module::info() const {
    return _info;
}

shared_ptr<Area> Module::area() const {
    return _area;
}

} // namespace game

} // namespace reone
