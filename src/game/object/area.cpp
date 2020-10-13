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

#include "area.h"

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

#include "SDL2/SDL.h"

#include "../../core/debug.h"
#include "../../core/jobs.h"
#include "../../core/log.h"
#include "../../core/streamutil.h"
#include "../../render/scene/cubenode.h"
#include "../../render/scene/scenegraph.h"
#include "../../resource/lytfile.h"
#include "../../resource/pthfile.h"
#include "../../resource/resources.h"
#include "../../resource/visfile.h"
#include "../../script/execution.h"

#include "../script/routines.h"
#include "../script/util.h"

#include "objectfactory.h"

using namespace std;

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

static const float kDefaultFieldOfView = 75.0f;
static const float kDrawDebugDistance = 64.0f;
static const float kPartyMemberFollowDistance = 4.0f;
static const float kMaxDistanceToTestCollision = 64.0f;
static const float kElevationTestZ = 1024.0f;

static const char kPartyLeaderTag[] = "party-leader";
static const char kPartyMember1Tag[] = "party-member-1";
static const char kPartyMember2Tag[] = "party-member-2";

Area::Area(
    uint32_t id,
    GameVersion version,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph,
    const GraphicsOptions &opts
) :
    Object(id, ObjectType::Area),
    _version(version),
    _objectFactory(objectFactory),
    _sceneGraph(sceneGraph),
    _opts(opts),
    _collisionDetector(this),
    _objectSelector(this),
    _actionExecutor(this) {

    _cameraAspect = opts.width / static_cast<float>(opts.height);
}

void Area::load(const string &name, const GffStruct &are, const GffStruct &git) {
    _name = name;

    loadLYT();
    loadVIS();
    loadPTH();
    loadARE(are);
    loadGIT(git);
}

void Area::loadLYT() {
    ResourceManager &resources = Resources;

    LytFile lyt;
    lyt.load(wrap(resources.findRaw(_name, ResourceType::AreaLayout)));

    for (auto &lytRoom : lyt.rooms()) {
        shared_ptr<ModelSceneNode> model(new ModelSceneNode(_sceneGraph, resources.findModel(lytRoom.name)));
        model->setLocalTransform(glm::translate(glm::mat4(1.0f), lytRoom.position));
        model->animate("animloop1", kAnimationLoop);

        _sceneGraph->addRoot(model);

        shared_ptr<Walkmesh> walkmesh(resources.findWalkmesh(lytRoom.name, ResourceType::Walkmesh));
        unique_ptr<Room> room(new Room(lytRoom.name, lytRoom.position, model, walkmesh));

        _rooms.insert(make_pair(room->name(), move(room)));
    }
}

void Area::loadVIS() {
    VisFile vis;
    vis.load(wrap(Resources.findRaw(_name, ResourceType::Vis)));

    _visibility = make_unique<Visibility>(vis.visibility());
}

void Area::loadPTH() {
    shared_ptr<GffStruct> pth(Resources.findGFF(_name, ResourceType::Path));

    PthFile path;
    path.load(*pth);

    const vector<PthFile::Point> &points = path.points();
    unordered_map<int, float> pointZ;

    for (int i = 0; i < points.size(); ++i) {
        const PthFile::Point &point = points[i];
        Room *room = nullptr;
        float z = 0.0f;

        if (!getElevationAt(glm::vec2(point.x, point.y), room, z)) {
            warn(boost::format("Area: point %d elevation not found") % i);
            continue;
        }
        pointZ.insert(make_pair(i, z));

        shared_ptr<CubeSceneNode> aabb(new CubeSceneNode(_sceneGraph, 0.5f));
        aabb->setLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(point.x, point.y, z + 0.25f)));

        _sceneGraph->addRoot(aabb);
    }

    _pathfinder.load(points, pointZ);
}

void Area::loadARE(const GffStruct &are) {
    loadCameraStyle(are);
    loadAmbientColor(are);
    loadScripts(are);
}

void Area::loadCameraStyle(const GffStruct &are) {
    int styleIdx = are.getInt("CameraStyle");
    shared_ptr<TwoDaTable> styleTable(Resources.find2DA("camerastyle"));

    _cameraStyle.distance = styleTable->getFloat(styleIdx, "distance", 0.0f);
    _cameraStyle.pitch = styleTable->getFloat(styleIdx, "pitch", 0.0f);
    _cameraStyle.viewAngle = styleTable->getFloat(styleIdx, "viewangle", 0.0f);
    _cameraStyle.height = styleTable->getFloat(styleIdx, "height", 0.0f);
}

void Area::loadAmbientColor(const GffStruct &are) {
    int ambientColorValue = are.getInt("DynAmbientColor");
    glm::vec3 ambientColor(
        ambientColorValue & 0xff,
        (ambientColorValue >> 8) & 0xff,
        (ambientColorValue >> 16) & 0xff);

    ambientColor /= 255.0f;

    _sceneGraph->setAmbientLightColor(ambientColor);
}

void Area::loadScripts(const GffStruct &are) {
    _scripts[ScriptType::OnEnter] = are.getString("OnEnter");
    _scripts[ScriptType::OnExit] = are.getString("OnExit");
    _scripts[ScriptType::OnHeartbeat] = are.getString("OnHeartbeat");
    _scripts[ScriptType::OnUserDefined] = are.getString("OnUserDefined");
}

void Area::loadGIT(const GffStruct &git) {
    loadProperties(git);
    loadCreatures(git);
    loadDoors(git);
    loadPlaceables(git);
    loadWaypoints(git);
    loadTriggers(git);
}

void Area::loadProperties(const GffStruct &git) {
    const GffStruct &gffs = git.getStruct("AreaProperties");
    shared_ptr<TwoDaTable> musicTable(Resources.find2DA("ambientmusic"));

    int musicIdx = gffs.getInt("MusicDay");
    if (musicIdx) {
        _music = musicTable->getString(musicIdx, "resource");
    }
}

void Area::loadCreatures(const GffStruct &git) {
    for (auto &gffs : git.getList("Creature List")) {
        shared_ptr<Creature> creature(_objectFactory->newCreature());
        creature->load(gffs);
        landObject(*creature);
        creature->setSynchronize(true);
        add(creature);
    }
}

void Area::loadDoors(const GffStruct &git) {
    for (auto &gffs : git.getList("Door List")) {
        shared_ptr<Door> door(_objectFactory->newDoor());
        door->load(gffs);
        door->setSynchronize(true);
        add(door);
    }
}

void Area::loadPlaceables(const GffStruct &git) {
    for (auto &gffs : git.getList("Placeable List")) {
        shared_ptr<Placeable> placeable(_objectFactory->newPlaceable());
        placeable->load(gffs);
        add(placeable);
    }
}

void Area::loadWaypoints(const GffStruct &git) {
    for (auto &gffs : git.getList("WaypointList")) {
        shared_ptr<Waypoint> waypoint(_objectFactory->newWaypoint());
        waypoint->load(gffs);
        add(waypoint);
    }
}

void Area::loadTriggers(const GffStruct &git) {
    for (auto &gffs : git.getList("TriggerList")) {
        shared_ptr<Trigger> trigger(_objectFactory->newTrigger());
        trigger->load(gffs);
        add(trigger);
    }
}

void Area::loadCameras(const glm::vec3 &entryPosition, float entryHeading) {
    glm::vec3 position(entryPosition);
    position.z += 1.7f;

    unique_ptr<FirstPersonCamera> firstPersonCamera(new FirstPersonCamera(_sceneGraph, _cameraAspect, glm::radians(kDefaultFieldOfView)));
    firstPersonCamera->setPosition(position);
    firstPersonCamera->setHeading(entryHeading);
    _firstPersonCamera = move(firstPersonCamera);

    unique_ptr<ThirdPersonCamera> thirdPersonCamera(new ThirdPersonCamera(_sceneGraph, _cameraAspect, _cameraStyle));
    thirdPersonCamera->setFindObstacleFunc([this](const glm::vec3 &origin, const glm::vec3 &dest, glm::vec3 &intersection) {
        glm::vec3 originToDest(dest - origin);
        glm::vec3 dir(glm::normalize(originToDest));

        RaycastProperties props;
        props.flags = kRaycastRooms | kRaycastObjects;
        props.origin = origin;
        props.direction = dir;

        RaycastResult result;

        if (_collisionDetector.raycast(props, result)) {
            float dist = glm::min(glm::length(originToDest), result.distance);
            intersection = origin + dist * dir;
            return true;
        }

        return false;
    });
    thirdPersonCamera->setTargetPosition(position);
    thirdPersonCamera->setHeading(entryHeading);
    _thirdPersonCamera = move(thirdPersonCamera);

    if (_onCameraChanged) {
        _onCameraChanged(_cameraType);
    }
}

void Area::add(const shared_ptr<SpatialObject> &object) {
    _objects.push_back(object);
    _objectsByType[object->type()].push_back(object);
    _objectById.insert(make_pair(object->id(), object));
    _objectsByTag[object->tag()].push_back(object);

    shared_ptr<ModelSceneNode> sceneNode(object->model());

    if (sceneNode) {
        _sceneGraph->addRoot(sceneNode);
    }
    determineObjectRoom(*object);
}

void Area::determineObjectRoom(SpatialObject &object) {
    glm::vec3 position(object.position());
    Room *room = nullptr;

    if (getElevationAt(position, room, position.z)) {
        object.setRoom(room);
    }
}

shared_ptr<SpatialObject> Area::find(uint32_t id) const {
    auto object = _objectById.find(id);
    if (object == _objectById.end()) return nullptr;

    return object->second;
}

shared_ptr<SpatialObject> Area::find(const string &tag, int nth) const {
    auto objects = _objectsByTag.find(tag);
    if (objects == _objectsByTag.end()) return nullptr;
    if (nth >= objects->second.size()) return nullptr;

    return objects->second[nth];
}

void Area::landObject(SpatialObject &object) {
    glm::vec3 position(object.position());
    Room *room = nullptr;

    if (getElevationAt(position, room, position.z)) {
        object.setPosition(position);
    }
}

void Area::loadParty(const PartyConfiguration &party, const glm::vec3 &position, float heading) {
    if (party.memberCount > 0) {
        shared_ptr<Creature> partyLeader(makeCharacter(party.leader, kPartyLeaderTag, position, heading));
        landObject(*partyLeader);
        partyLeader->setSynchronize(true);
        add(partyLeader);
        _player = partyLeader;
        _partyLeader = partyLeader;
    }
    if (party.memberCount > 1) {
        shared_ptr<Creature> partyMember(makeCharacter(party.member1, kPartyMember1Tag, position, heading));
        landObject(*partyMember);
        partyMember->setSynchronize(true);
        add(partyMember);
        _partyMember1 = partyMember;

        unique_ptr<FollowAction> action(new FollowAction(_partyLeader, kPartyMemberFollowDistance));
        partyMember->actionQueue().add(move(action));
    }
    if (party.memberCount > 2) {
        shared_ptr<Creature> partyMember(makeCharacter(party.member2, kPartyMember2Tag, position, heading));
        landObject(*partyMember);
        partyMember->setSynchronize(true);
        add(partyMember);
        _partyMember2 = partyMember;

        unique_ptr<FollowAction> action(new FollowAction(_partyLeader, kPartyMemberFollowDistance));
        partyMember->actionQueue().add(move(action));
    }
}

shared_ptr<Creature> Area::makeCharacter(const CreatureConfiguration &character, const string &tag, const glm::vec3 &position, float heading) {
    shared_ptr<Creature> creature(_objectFactory->newCreature());
    creature->setTag(tag);
    creature->load(character);
    creature->setPosition(position);
    creature->setHeading(heading);

    return move(creature);
}

bool Area::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        default:
            return false;
    }
}

bool Area::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_Q:
            _objectSelector.selectNext(true);
            return true;

        case SDL_SCANCODE_E:
            _objectSelector.selectNext();
            return true;

        default:
            return false;
    }
}

bool Area::getElevationAt(const glm::vec2 &position, Room *&room, float &z) const {
    RaycastProperties props;
    props.origin = glm::vec3(position, kElevationTestZ);
    props.direction = glm::vec3(0.0f, 0.0f, -1.0f);

    RaycastResult result;

    props.flags = kRaycastObjects;

    if (_collisionDetector.raycast(props, result)) return false;

    props.flags = kRaycastRooms | kRaycastWalkable;

    if (_collisionDetector.raycast(props, result)) {
        room = result.room;
        z = result.intersection.z;
        return true;
    }

    return false;
}

void Area::update(const UpdateContext &updateCtx) {
    Object::update(updateCtx);
    _actionExecutor.executeActions(*this, updateCtx.deltaTime);

    for (auto &room : _rooms) {
        room.second->update(updateCtx.deltaTime);
    }
    for (auto &object : _objects) {
        object->update(updateCtx);
        _actionExecutor.executeActions(*object, updateCtx.deltaTime);
    }
    _objectSelector.update();
    _sceneGraph->prepare();
}

bool Area::moveCreatureTowards(Creature &creature, const glm::vec2 &dest, bool run, float dt) {
    glm::vec3 position(creature.position());
    glm::vec2 delta(dest - glm::vec2(position));
    glm::vec2 dir(glm::normalize(delta));

    float heading = -glm::atan(dir.x, dir.y);
    creature.setHeading(heading);

    float speed = run ? creature.runSpeed() : creature.walkSpeed();
    float speedDt = speed * dt;
    position.x += dir.x * speedDt;
    position.y += dir.y * speedDt;

    Room *room = nullptr;

    if (getElevationAt(position, room, position.z)) {
        creature.setRoom(room);
        creature.setPosition(position);
        return true;
    }

    return false;
}

void Area::updateTriggers(const Creature &creature) {
    glm::vec2 position2d(creature.position());
    glm::vec3 liftedPosition(position2d, kElevationTestZ);
    glm::vec3 down(0.0f, 0.0f, -1.0f);
    glm::vec2 intersection;
    float distance;

    for (auto &object : _objectsByType[ObjectType::Trigger]) {
        Trigger &trigger = static_cast<Trigger &>(*object);
        if (trigger.distanceTo(position2d) > kMaxDistanceToTestCollision) continue;

        const vector<glm::vec3> &geometry = trigger.geometry();
        bool triggered =
            (geometry.size() >= 3 && glm::intersectRayTriangle(liftedPosition, down, geometry[0], geometry[1], geometry[2], intersection, distance)) ||
            (geometry.size() >= 4 && glm::intersectRayTriangle(liftedPosition, down, geometry[2], geometry[3], geometry[0], intersection, distance));

        if (triggered) {
            if (!trigger.linkedToModule().empty() && _onModuleTransition) {
                _onModuleTransition(trigger.linkedToModule(), trigger.linkedTo());
            }
            break;
        }
    }
}

void Area::runOnEnterScript() {
    if (_scriptsEnabled) {
        if (!_scripts[ScriptType::OnEnter].empty()) {
            runScript(_scripts[ScriptType::OnEnter], _id, _player->id(), -1);
        }
    }
}

void Area::saveTo(GameState &state) const {
    AreaState areaState;

    for (auto &list : _objectsByType) {
        if (list.first != ObjectType::Creature && list.first != ObjectType::Door) continue;

        for (auto &object : list.second) {
            object->saveTo(areaState);
        }
    }

    state.areas[_name] = move(areaState);
}

void Area::loadState(const GameState &state) {
    auto it = state.areas.find(_name);
    if (it == state.areas.end()) return;

    const AreaState &areaState = it->second;

    for (auto &list : _objectsByType) {
        if (list.first != ObjectType::Creature && list.first != ObjectType::Door) continue;

        for (auto &object : list.second) {
            object->loadState(areaState);
        }
    }
}

void Area::updateRoomVisibility() {
    Room *playerRoom = _player->room();
    if (!playerRoom) return;

    for (auto &room : _rooms) {
        room.second->setVisible(false);
    }
    playerRoom->setVisible(true);

    auto adjRooms = _visibility->equal_range(playerRoom->name());
    for (auto adjRoom = adjRooms.first; adjRoom != adjRooms.second; adjRoom++) {
        auto room = _rooms.find(adjRoom->second);
        if (room != _rooms.end()) {
            room->second->setVisible(true);
        }
    }
}

SpatialObject *Area::getObjectAt(int x, int y) const {
    Camera *camera = getCamera();
    shared_ptr<CameraSceneNode> sceneNode(camera->sceneNode());

    glm::vec4 viewport(0.0f, 0.0f, _opts.width, _opts.height);
    glm::vec3 fromWorld(glm::unProject(glm::vec3(x, _opts.height - y, 0.0f), sceneNode->view(), sceneNode->projection(), viewport));
    glm::vec3 toWorld(glm::unProject(glm::vec3(x, _opts.height - y, 1.0f), sceneNode->view(), sceneNode->projection(), viewport));

    RaycastProperties props;
    props.flags = kRaycastObjects | kRaycastAABB;
    props.origin = fromWorld;
    props.direction = glm::normalize(toWorld - fromWorld);
    props.except = _player.get();

    RaycastResult result;

    if (_collisionDetector.raycast(props, result)) {
        return result.object;
    }

    return nullptr;
}

void Area::fill(const UpdateContext &updateCtx, GuiContext &guiCtx) {
    addPartyMemberPortrait(_partyLeader, guiCtx);
    addPartyMemberPortrait(_partyMember1, guiCtx);
    addPartyMemberPortrait(_partyMember2, guiCtx);

    int hilightedObjectId = _objectSelector.hilightedObjectId();
    if (hilightedObjectId != -1) {
        glm::vec3 coords(getSelectableScreenCoords(hilightedObjectId, updateCtx));
        if (coords.z < 1.0f) {
            guiCtx.selection.hasHilighted = true;
            guiCtx.selection.hilightedScreenCoords = coords;
        }
    }
    int selectedObjectId = _objectSelector.selectedObjectId();
    if (selectedObjectId != -1) {
        glm::vec3 coords(getSelectableScreenCoords(selectedObjectId, updateCtx));
        if (coords.z < 1.0f) {
            guiCtx.selection.hasSelected = true;
            guiCtx.selection.selectedScreenCoords = coords;
        }
    }
    addDebugInfo(updateCtx, guiCtx);
}

glm::vec3 Area::getSelectableScreenCoords(uint32_t objectId, const UpdateContext &ctx) const {
    static glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);

    shared_ptr<SpatialObject> object(find(objectId));
    glm::vec3 position(object->selectablePosition());

    return glm::project(position, ctx.view, ctx.projection, viewport);
}

void Area::addPartyMemberPortrait(const shared_ptr<SpatialObject> &object, GuiContext &ctx) {
    if (object) {
        ctx.hud.partyPortraits.push_back(static_cast<Creature &>(*object).portrait());
    }
}

void Area::addDebugInfo(const UpdateContext &updateCtx, GuiContext &guiCtx) {
    if (getDebugMode() == DebugMode::GameObjects) {
        guiCtx.debug.objects.clear();
        glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);

        for (auto &object : _objects) {
            glm::vec3 position(object->position());
            if (glm::distance2(position, updateCtx.cameraPosition) > kDrawDebugDistance) continue;

            DebugObject debugObj;
            debugObj.tag = object->tag();
            debugObj.text = object->tag();
            debugObj.screenCoords = glm::project(position, updateCtx.view, updateCtx.projection, viewport);

            guiCtx.debug.objects.push_back(move(debugObj));
        }
    }
}

void Area::update3rdPersonCameraTarget() {
    shared_ptr<SpatialObject> player(_player);
    if (!player) return;

    glm::vec3 position;

    if (player->model()->getNodeAbsolutePosition("camerahook", position)) {
        position += player->position();
    } else {
        position = player->position();
    }
    _thirdPersonCamera->setTargetPosition(position);
}

void Area::update3rdPersonCameraHeading() {
    shared_ptr<SpatialObject> player(_player);
    if (!player) return;

    _thirdPersonCamera->setHeading(player->heading());
}

void Area::switchTo3rdPersonCamera() {
    if (_cameraType == CameraType::ThirdPerson) return;

    _cameraType = CameraType::ThirdPerson;

    if (_onCameraChanged) {
        _onCameraChanged(_cameraType);
    }
}

void Area::toggleCameraType() {
    bool changed = false;

    switch (_cameraType) {
        case CameraType::FirstPerson:
            if (_partyLeader) {
                _cameraType = CameraType::ThirdPerson;
                changed = true;
            }
            break;

        case CameraType::ThirdPerson:
            _cameraType = CameraType::FirstPerson;
            _firstPersonCamera->setPosition(_thirdPersonCamera->sceneNode()->absoluteTransform()[3]);
            _firstPersonCamera->setHeading(_thirdPersonCamera->heading());
            changed = true;
            break;
    }

    if (changed && _onCameraChanged) {
        _onCameraChanged(_cameraType);
    }
}

Camera *Area::getCamera() const {
    return _cameraType == CameraType::ThirdPerson ? _thirdPersonCamera.get() : static_cast<Camera *>(_firstPersonCamera.get());
}

void Area::startDialog(Creature &creature, const string &resRef) {
    if (_onStartDialog) {
        _onStartDialog(creature, resRef);
    }
}

const CameraStyle &Area::cameraStyle() const {
    return _cameraStyle;
}

CameraType Area::cameraType() const {
    return _cameraType;
}

const string &Area::music() const {
    return _music;
}

const RoomMap &Area::rooms() const {
    return _rooms;
}

const ObjectList &Area::objects() const {
    return _objects;
}

const CollisionDetector &Area::collisionDetector() const {
    return _collisionDetector;
}

const Pathfinder &Area::pathfinder() const {
    return _pathfinder;
}

ThirdPersonCamera *Area::thirdPersonCamera() {
    return _thirdPersonCamera.get();
}

ObjectSelector &Area::objectSelector() {
    return _objectSelector;
}

shared_ptr<SpatialObject> Area::player() const {
    return _player;
}

shared_ptr<SpatialObject> Area::partyLeader() const {
    return _partyLeader;
}

shared_ptr<SpatialObject> Area::partyMember1() const {
    return _partyMember1;
}

shared_ptr<SpatialObject> Area::partyMember2() const {
    return _partyMember2;
}

void Area::setOnCameraChanged(const function<void(CameraType)> &fn) {
    _onCameraChanged = fn;
}

void Area::setOnModuleTransition(const function<void(const string &, const string &)> &fn) {
    _onModuleTransition = fn;
}

void Area::setOnPlayerChanged(const function<void()> &fn) {
    _onPlayerChanged = fn;
}

void Area::setOnStartDialog(const function<void(const Object &, const string &)> &fn) {
    _onStartDialog = fn;
}

} // namespace game

} // namespace reone
