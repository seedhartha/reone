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

#include "../core/log.h"
#include "../core/streamutil.h"
#include "../render/camera/firstperson.h"
#include "../render/camera/thirdperson.h"
#include "../resources/gfffile.h"
#include "../resources/manager.h"

#include "object/door.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

static const float kDefaultFieldOfView = 75.0f;

Module::Module(const string &name, GameVersion version, const GraphicsOptions &opts) :
    _name(name),
    _version(version),
    _opts(opts),
    _cameraAspect(opts.width / static_cast<float>(opts.height)) {
}

void Module::load(const GffStruct &ifo, const string &entry) {
    loadInfo(ifo);
    loadArea(ifo);
    loadCameras(entry);
    loadParty(entry);

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
    reone::info("Loading area " + _info.entryArea);

    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<GffStruct> are(resources.findGFF(_info.entryArea, ResourceType::Area));
    shared_ptr<GffStruct> git(resources.findGFF(_info.entryArea, ResourceType::GameInstance));

    shared_ptr<Area> area(makeArea());
    area->setOnModuleTransition([this](const string &module, const string &entry) {
        if (_transitionEnabled && _onModuleTransition) {
            _onModuleTransition(module, entry);
        }
    });
    area->setOnPlayerChanged([this]() {
        update3rdPersonCameraTarget();
        switchTo3rdPersonCamera();
    });
    area->load(*are, *git);
    _area = move(area);
}

const shared_ptr<Area> Module::makeArea() const {
    return make_shared<Area>(_version, _info.entryArea);
}

void Module::loadCameras(const string &entry) {
    glm::vec3 position(0.0f);
    float heading = 0.0f;
    getEntryPoint(entry, position, heading);
    position += glm::vec3(0.0f, 0.0f, 1.7f);

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

void Module::getEntryPoint(const string &waypoint, glm::vec3 &position, float &heading) {
    position = _info.entryPosition;
    heading = _info.entryHeading;

    if (!waypoint.empty()) {
        shared_ptr<Object> object(_area->find(waypoint, ObjectType::Waypoint));
        if (object) {
            position = object->position();
            heading = object->heading();
        }
    }
}

bool Module::findObstacle(const glm::vec3 &from, const glm::vec3 &to, glm::vec3 &intersection) const {
    Object *obstacle = nullptr;
    if (_area->findObstacleByWalkmesh(from, to, kObstacleRoom | kObstacleDoor, intersection, &obstacle)) {
        return true;
    }

    return false;
}

void Module::loadParty(const string &entry) {
    if (!_loadParty) return;

    glm::vec3 position(0.0f);
    float heading = 0.0f;
    getEntryPoint(entry, position, heading);

    _area->loadParty(position, heading);

    update3rdPersonCameraTarget();
    update3rdPersonCameraHeading();
    switchTo3rdPersonCamera();
}

void Module::update3rdPersonCameraTarget() {
    Object &player = *_area->player();
    _thirdPersonCamera->setTargetPosition(player.position() + player.model()->getNodeAbsolutePosition("camerahook"));
}

void Module::update3rdPersonCameraHeading() {
    Object &player = *_area->player();
    _thirdPersonCamera->setHeading(player.heading());
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

bool Module::handleMouseButtonUp(const SDL_MouseButtonEvent &event) {
    shared_ptr<Camera> camera(getCamera());
    glm::vec4 viewport(0.0f, 0.0f, _opts.width, _opts.height);
    glm::vec3 fromWorld(glm::unProject(glm::vec3(event.x, _opts.height - event.y, 0.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 toWorld(glm::unProject(glm::vec3(event.x, _opts.height - event.y, 1.0f), camera->view(), camera->projection(), viewport));

    shared_ptr<Object> player(_area->player());
    Object *except = player ? player.get() : nullptr;
    Object *obstacle = nullptr;

    if (_area->findObstacleByAABB(fromWorld, toWorld, kObstacleCreature | kObstacleDoor | kObstaclePlaceable, except, &obstacle)) {
        assert(obstacle);
        debug(boost::format("Object '%s' clicked on") % obstacle->tag());

        Door *door = dynamic_cast<Door *>(obstacle);
        if (door) {
            if (!door->linkedToModule().empty()) {
                if (_transitionEnabled && _onModuleTransition) {
                    _onModuleTransition(door->linkedToModule(), door->linkedTo());
                }
            } else if (!door->isOpen() && !door->isStatic()) {
                door->open(player);
            }
        }

        Creature *creature = dynamic_cast<Creature *>(obstacle);
        if (creature) {
            if (!creature->conversation().empty() && _startDialog) {
                resetInput();
                getCamera()->resetInput();
                _startDialog(creature->conversation(), creature->tag());
            }
        }

        return true;
    }

    return false;
}

void Module::resetInput() {
    _moveForward = false;
    _moveBackward = false;
}

bool Module::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            _moveForward = true;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = true;
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

        case SDL_SCANCODE_S:
            _moveBackward = false;
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
    switch (_debugMode) {
        case DebugMode::None:
            _debugMode = forward ? DebugMode::GameObjects : DebugMode::ModelNodes;
            break;
        case DebugMode::GameObjects:
            _debugMode = forward ? DebugMode::ModelNodes : DebugMode::None;
            break;
        case DebugMode::ModelNodes:
            _debugMode = forward ? DebugMode::None : DebugMode::GameObjects;
            break;
    }

    _area->setDebugMode(_debugMode);
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

    _area->update(ctx, guiCtx);
}

void Module::updatePlayer(float dt) {
    if (_cameraType != CameraType::ThirdPerson) return;

    ThirdPersonCamera &camera = static_cast<ThirdPersonCamera &>(*_thirdPersonCamera);
    Creature &player = static_cast<Creature &>(*_area->player());

    float heading = 0.0f;
    bool movement = false;

    if (_moveForward) {
        heading = camera.heading();
        movement = true;
    } else if (_moveBackward) {
        heading = camera.heading() + glm::pi<float>();
        movement = true;
    }

    if (movement) {
        glm::vec3 target(player.position());
        target.x -= 100.0f * glm::sin(heading);
        target.y += 100.0f * glm::cos(heading);

        if (_area->moveCreatureTowards(player, target, dt)) {
            player.setMovementType(MovementType::Run);
            update3rdPersonCameraTarget();
        }
    } else {
        player.setMovementType(MovementType::None);
    }
}

void Module::initGL() {
    _area->initGL();
}

void Module::render() const {
    _area->render();
}

void Module::setLoadParty(bool load) {
    _loadParty = load;
}

void Module::setTransitionEnabled(bool enabled) {
    _transitionEnabled = enabled;
}

void Module::setOnCameraChanged(const function<void(CameraType)> &fn) {
    _onCameraChanged = fn;
}

void Module::setOnModuleTransition(const function<void(const string &, const string &)> &fn) {
    _onModuleTransition = fn;
}

void Module::setStartDialog(const function<void(const string &, const string &)> &fn) {
    _startDialog = fn;
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

Area &Module::area() const {
    return *_area;
}

CameraType Module::cameraType() const {
    return _cameraType;
}

} // namespace game

} // namespace reone
