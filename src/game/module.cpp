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

#include "../core/debug.h"
#include "../core/log.h"
#include "../core/streamutil.h"
#include "../render/camera/firstperson.h"
#include "../render/camera/thirdperson.h"
#include "../resources/gfffile.h"
#include "../resources/resources.h"

#include "object/door.h"
#include "object/factory.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

static const float kDefaultFieldOfView = 75.0f;

Module::Module(uint32_t id, GameVersion version, ObjectFactory *objectFactory, const GraphicsOptions &opts) :
    Object(id, ObjectType::Module),
    _version(version),
    _objectFactory(objectFactory),
    _opts(opts) {

    assert(_objectFactory);
    _cameraAspect = opts.width / static_cast<float>(opts.height);
}

void Module::load(const string &name, const GffStruct &ifo) {
    _name = name;

    loadInfo(ifo);
    loadArea(ifo);
    loadCameras();

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
        update3rdPersonCameraTarget();
        switchTo3rdPersonCamera();
    });
    area->setOnStartDialog([this](const Object &object, const string &resRef) {
        if (!_startDialog) return;

        const Creature &creature = static_cast<const Creature &>(object);

        string finalResRef(resRef);
        if (resRef.empty()) finalResRef = creature.conversation();
        if (resRef.empty()) return;

        _startDialog(object, finalResRef);
    });
    area->load(_info.entryArea, *are, *git);
    _area = move(area);
}

void Module::loadCameras() {
    glm::vec3 position(_info.entryPosition);
    position.z += 1.7f;

    float heading = _info.entryHeading;

    unique_ptr<FirstPersonCamera> firstPersonCamera(new FirstPersonCamera(_cameraAspect, glm::radians(kDefaultFieldOfView)));
    firstPersonCamera->setPosition(position);
    firstPersonCamera->setHeading(heading);
    _firstPersonCamera = move(firstPersonCamera);

    unique_ptr<ThirdPersonCamera> thirdPersonCamera(new ThirdPersonCamera(_cameraAspect, _area->cameraStyle()));
    thirdPersonCamera->setFindObstacleFunc(bind(&Module::findObstacle, this, _1, _2, _3));
    thirdPersonCamera->setTargetPosition(position);
    thirdPersonCamera->setHeading(heading);
    _thirdPersonCamera = move(thirdPersonCamera);

    if (_onCameraChanged) {
        _onCameraChanged(_cameraType);
    }
}

bool Module::findObstacle(const glm::vec3 &from, const glm::vec3 &to, glm::vec3 &intersection) const {
    SpatialObject *obstacle = nullptr;
    if (_area->findObstacleByWalkmesh(from, to, kObstacleRoom | kObstacleDoor, intersection, &obstacle)) {
        return true;
    }

    return false;
}

void Module::loadParty(const PartyConfiguration &party, const string &entry) {
    _party = party;

    glm::vec3 position(0.0f);
    float heading = 0.0f;
    getEntryPoint(entry, position, heading);

    _area->loadParty(_party, position, heading);
    _area->updateRoomVisibility();

    update3rdPersonCameraTarget();
    update3rdPersonCameraHeading();
    switchTo3rdPersonCamera();

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

void Module::update3rdPersonCameraTarget() {
    shared_ptr<SpatialObject> player(_area->player());
    if (!player) return;

    _thirdPersonCamera->setTargetPosition(player->position() + player->model()->getNodeAbsolutePosition("camerahook"));
}

void Module::update3rdPersonCameraHeading() {
    shared_ptr<SpatialObject> player(_area->player());
    if (!player) return;

    _thirdPersonCamera->setHeading(player->heading());
}

void Module::switchTo3rdPersonCamera() {
    if (_cameraType == CameraType::ThirdPerson) return;

    _cameraType = CameraType::ThirdPerson;

    if (_onCameraChanged) {
        _onCameraChanged(_cameraType);
    }
}

bool Module::handle(const SDL_Event &event) {
    if (!_loaded) return false;
    if (getCamera()->handle(event)) return true;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            if (handleMouseMotion(event.motion)) return true;
            break;
        case SDL_MOUSEBUTTONUP:
            if (handleMouseButtonUp(event.button)) return true;
            break;
        case SDL_KEYDOWN:
            if (handleKeyDown(event.key)) return true;
            break;
        case SDL_KEYUP:
            if (handleKeyUp(event.key)) return true;
            break;
    }

    if (_area->handle(event)) return true;

    return false;
}

bool Module::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    const SpatialObject *object = getObjectAt(event.x, event.y);
    _area->hilight(object ? object->id() : -1);

    return true;
}

SpatialObject *Module::getObjectAt(int x, int y) const {
    shared_ptr<Camera> camera(getCamera());
    glm::vec4 viewport(0.0f, 0.0f, _opts.width, _opts.height);
    glm::vec3 fromWorld(glm::unProject(glm::vec3(x, _opts.height - y, 0.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 toWorld(glm::unProject(glm::vec3(x, _opts.height - y, 1.0f), camera->view(), camera->projection(), viewport));

    shared_ptr<SpatialObject> player(_area->player());
    SpatialObject *except = player ? player.get() : nullptr;
    SpatialObject *obstacle = nullptr;

    _area->findObstacleByAABB(fromWorld, toWorld, kObstacleCreature | kObstacleDoor | kObstaclePlaceable, except, &obstacle);

    return obstacle;
}

bool Module::handleMouseButtonUp(const SDL_MouseButtonEvent &event) {
    SpatialObject *object = getObjectAt(event.x, event.y);
    if (!object) {
        return false;
    }
    debug(boost::format("Object '%s' clicked on") % object->tag());

    uint32_t selectedObjectId = _area->selectedObjectId();
    if (object->id() != selectedObjectId) {
        _area->select(object->id());
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
            resetPlayerMovement();
            getCamera()->resetInput();

            if (_startDialog) {
                _startDialog(*creature, creature->conversation());
            }
        }
        return true;
    }

    return true;
}

void Module::resetPlayerMovement() {
    _moveForward = false;
    _moveLeft = false;
    _moveBackward = false;
    _moveRight = false;
}

bool Module::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            _moveForward = true;
            return true;

        case SDL_SCANCODE_Z:
            _moveLeft = true;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = true;
            return true;

        case SDL_SCANCODE_C:
            _moveRight = true;
            return true;

        default:
            break;
    }

    return false;
}

bool Module::handleKeyUp(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_V:
            toggleCameraType();
            return true;

        case SDL_SCANCODE_LEFTBRACKET:
            cycleDebugMode(false);
            return true;

        case SDL_SCANCODE_RIGHTBRACKET:
            cycleDebugMode(true);
            return true;

        case SDL_SCANCODE_W:
            _moveForward = false;
            return true;

        case SDL_SCANCODE_Z:
            _moveLeft = false;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = false;
            return true;

        case SDL_SCANCODE_C:
            _moveRight = false;
            return true;

        case SDL_SCANCODE_X:
            static_cast<Creature &>(*_area->player()).playGreetingAnimation();
            return true;

        default:
            break;
    }

    return false;
}

void Module::toggleCameraType() {
    bool changed = false;

    switch (_cameraType) {
        case CameraType::FirstPerson:
            if (_area->partyLeader()) {
                _cameraType = CameraType::ThirdPerson;
                changed = true;
            }
            break;

        case CameraType::ThirdPerson:
            _cameraType = CameraType::FirstPerson;
            _firstPersonCamera->setPosition(_thirdPersonCamera->position());
            _firstPersonCamera->setHeading(_thirdPersonCamera->heading());
            changed = true;
            break;
    }

    if (changed && _onCameraChanged) {
        _onCameraChanged(_cameraType);
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

    shared_ptr<Camera> camera(getCamera());
    camera->update(dt);

    updatePlayer(dt);

    UpdateContext ctx;
    ctx.deltaTime = dt;
    ctx.cameraPosition = camera->position();
    ctx.projection = camera->projection();
    ctx.view = camera->view();

    _area->update(ctx);
    _area->fill(ctx, guiCtx);
}

void Module::updatePlayer(float dt) {
    if (_cameraType != CameraType::ThirdPerson) return;

    ThirdPersonCamera &camera = static_cast<ThirdPersonCamera &>(*_thirdPersonCamera);
    shared_ptr<Object> playerObject(_area->player());
    if (!playerObject) return;

    Creature &player = static_cast<Creature &>(*playerObject);

    float heading = 0.0f;
    bool movement = true;

    if (_moveForward) {
        heading = camera.heading();
    } else if (_moveBackward) {
        heading = camera.heading() + glm::pi<float>();
    } else if (_moveLeft) {
        heading = camera.heading() + glm::half_pi<float>();
    } else if (_moveRight) {
        heading = camera.heading() - glm::half_pi<float>();
    } else {
        movement = false;
    }

    if (movement) {
        glm::vec3 target(player.position());
        target.x -= 100.0f * glm::sin(heading);
        target.y += 100.0f * glm::cos(heading);

        if (_area->moveCreatureTowards(player, target, dt)) {
            player.setMovementType(MovementType::Run);
            update3rdPersonCameraTarget();
            _area->updateRoomVisibility();
            _area->selectNearestObject();
        }

    } else {
        player.setMovementType(MovementType::None);
    }
}

void Module::saveTo(GameState &state) const {
    state.party = _party;
    _area->saveTo(state);
}

void Module::setOnCameraChanged(const function<void(CameraType)> &fn) {
    _onCameraChanged = fn;
}

void Module::setOnModuleTransition(const function<void(const string &, const string &)> &fn) {
    _onModuleTransition = fn;
}

void Module::setStartDialog(const function<void(const Object &, const string &)> &fn) {
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

shared_ptr<Camera> Module::getCamera() const {
    return _cameraType == CameraType::FirstPerson ? shared_ptr<Camera>(_firstPersonCamera) : _thirdPersonCamera;
}

const ModuleInfo &Module::info() const {
    return _info;
}

shared_ptr<Area> Module::area() const {
    return _area;
}

CameraType Module::cameraType() const {
    return _cameraType;
}

} // namespace game

} // namespace reone
