#include "module.h"

#include "../core/log.h"
#include "../core/streamutil.h"
#include "../render/camera/firstperson.h"
#include "../render/camera/thirdperson.h"
#include "../resources/gfffile.h"
#include "../resources/manager.h"

#include "object/door.h"

using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

static const float kDefaultFieldOfView = 75.0f;

Module::Module(const std::string &name, GameVersion version, const GraphicsOptions &opts) :
    _name(name),
    _version(version),
    _opts(opts),
    _cameraAspect(opts.width / static_cast<float>(opts.height)) {
}

void Module::load(const GffStruct &ifo, const std::string &entry) {
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

    float headingSin = ifo.getFloat("Mod_Entry_Dir_X");
    float headingCos = ifo.getFloat("Mod_Entry_Dir_Y");
    _info.entryHeading = -glm::atan(headingSin, headingCos);

    _info.entryArea = ifo.getString("Mod_Entry_Area");
}

void Module::loadArea(const GffStruct &ifo) {
    reone::info("Loading area " + _info.entryArea);

    ResourceManager &resources = ResourceManager::instance();
    std::shared_ptr<GffStruct> are(resources.findGFF(_info.entryArea, ResourceType::Area));
    std::shared_ptr<GffStruct> git(resources.findGFF(_info.entryArea, ResourceType::GameInstance));

    std::shared_ptr<Area> area(makeArea());
    area->setOnModuleTransition([this](const std::string &module, const std::string &entry) {
        if (_transitionEnabled && _onModuleTransition) {
            _onModuleTransition(module, entry);
        }
    });
    area->setOnPlayerChanged([this]() {
        syncThirdPersonCamera();
        if (_cameraType != CameraType::ThirdPerson) {
            _cameraType = CameraType::ThirdPerson;
            if (_onCameraChanged) {
                _onCameraChanged(CameraType::ThirdPerson);
            }
        }
    });
    area->load(*are, *git);
    _area = std::move(area);
}

const std::shared_ptr<Area> Module::makeArea() const {
    return std::make_shared<Area>(_version, _info.entryArea);
}

void Module::loadCameras(const std::string &entry) {
    glm::vec3 position(0.0f);
    float heading = 0.0f;
    getEntryPoint(entry, position, heading);
    position += glm::vec3(0.0f, 0.0f, 1.7f);

    std::unique_ptr<FirstPersonCamera> firstPersonCamera(new FirstPersonCamera(_cameraAspect, glm::radians(kDefaultFieldOfView)));
    firstPersonCamera->setPosition(position);
    firstPersonCamera->setHeading(heading);
    _firstPersonCamera = std::move(firstPersonCamera);

    std::unique_ptr<ThirdPersonCamera> thirdPersonCamera(new ThirdPersonCamera(_cameraAspect, _area->cameraStyle()));
    thirdPersonCamera->setFindObstacleFunc(std::bind(&Module::findObstacle, this, _1, _2, _3));
    thirdPersonCamera->setTargetPosition(position);
    thirdPersonCamera->setHeading(heading);
    _thirdPersonCamera = std::move(thirdPersonCamera);

    if (_onCameraChanged) {
        _onCameraChanged(_cameraType);
    }
}

void Module::getEntryPoint(const std::string &waypoint, glm::vec3 &position, float &heading) {
    position = _info.entryPosition;
    heading = _info.entryHeading;

    if (!waypoint.empty()) {
        std::shared_ptr<Object> object(_area->find(waypoint, ObjectType::Waypoint));
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

void Module::loadParty(const std::string &entry) {
    if (!_loadParty) return;

    glm::vec3 position(0.0f);
    float heading = 0.0f;
    getEntryPoint(entry, position, heading);

    _area->loadParty(position, heading);

    syncThirdPersonCamera();
    _thirdPersonCamera->setHeading(heading);
    _cameraType = CameraType::ThirdPerson;

    if (_onCameraChanged) {
        _onCameraChanged(_cameraType);
    }
}

void Module::syncThirdPersonCamera() {
    Creature &player = static_cast<Creature &>(*_area->player());
    _thirdPersonCamera->setTargetPosition(player.position() + player.model()->getNodeAbsolutePosition("camerahook"));
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
    std::shared_ptr<Camera> camera(getCamera());
    glm::vec4 viewport(0.0f, 0.0f, _opts.width, _opts.height);
    glm::vec3 fromWorld(glm::unProject(glm::vec3(event.x, _opts.height - event.y, 0.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 toWorld(glm::unProject(glm::vec3(event.x, _opts.height - event.y, 1.0f), camera->view(), camera->projection(), viewport));

    std::shared_ptr<Object> player(_area->player());
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

        return true;
    }

    return false;
}

bool Module::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            _moveForward = true;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = true;
            return true;
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

    std::shared_ptr<Camera> camera(getCamera());
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
            syncThirdPersonCamera();
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

void Module::setOnCameraChanged(const std::function<void(CameraType)> &fn) {
    _onCameraChanged = fn;
}

void Module::setOnModuleTransition(const std::function<void(const std::string &, const std::string &)> &fn) {
    _onModuleTransition = fn;
}

const std::string &Module::name() const {
    return _name;
}

bool Module::loaded() const {
    return _loaded;
}

std::shared_ptr<Camera> Module::getCamera() const {
    return _cameraType == CameraType::FirstPerson ? std::shared_ptr<Camera>(_firstPersonCamera) : _thirdPersonCamera;
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
