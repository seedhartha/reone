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

#include <boost/format.hpp>

#include "glm/gtx/intersect.hpp"

#include "../../render/models.h"
#include "../../render/walkmeshes.h"
#include "../../resource/lytfile.h"
#include "../../resource/visfile.h"
#include "../../resource/resources.h"
#include "../../scene/cubenode.h"
#include "../../system/debug.h"
#include "../../system/log.h"
#include "../../system/streamutil.h"

#include "../game.h"
#include "../room.h"
#include "../script/util.h"

#include "objectfactory.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static const float kDefaultFieldOfView = 75.0f;
static const float kDrawDebugDistance = 64.0f;
static const float kPartyMemberFollowDistance = 4.0f;
static const float kMaxDistanceToTestCollision = 64.0f;
static const float kElevationTestZ = 1024.0f;
static const float kCreatureObstacleTestZ = 0.1f;

Area::Area(uint32_t id, Game *game) :
    Object(id, ObjectType::Area),
    _game(game),
    _collisionDetector(this),
    _objectSelector(this, &game->party()),
    _actionExecutor(this) {

    if (!game) {
        throw invalid_argument("Game must not be null");
    }
    const GraphicsOptions &opts = _game->options().graphics;
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
    LytFile lyt;
    lyt.load(wrap(Resources::instance().get(_name, ResourceType::AreaLayout)));

    for (auto &lytRoom : lyt.rooms()) {
        shared_ptr<ModelSceneNode> model(new ModelSceneNode(&_game->sceneGraph(), Models::instance().get(lytRoom.name)));
        model->setLocalTransform(glm::translate(glm::mat4(1.0f), lytRoom.position));
        model->playAnimation("animloop1", kAnimationLoop);

        shared_ptr<Walkmesh> walkmesh(Walkmeshes::instance().get(lytRoom.name, ResourceType::Walkmesh));
        unique_ptr<Room> room(new Room(lytRoom.name, lytRoom.position, model, walkmesh));

        _rooms.insert(make_pair(room->name(), move(room)));
    }
}

void Area::loadVIS() {
    VisFile vis;
    vis.load(wrap(Resources::instance().get(_name, ResourceType::Vis)));

    _visibility = make_unique<Visibility>(vis.visibility());
}

void Area::loadPTH() {
    shared_ptr<GffStruct> pth(Resources::instance().getGFF(_name, ResourceType::Path));

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
    shared_ptr<TwoDaTable> styleTable(Resources::instance().get2DA("camerastyle"));

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

    _game->sceneGraph().setAmbientLightColor(ambientColor);
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
    shared_ptr<TwoDaTable> musicTable(Resources::instance().get2DA("ambientmusic"));

    int musicIdx = gffs.getInt("MusicDay");
    if (musicIdx) {
        _music = musicTable->getString(musicIdx, "resource");
    }
}

void Area::loadCreatures(const GffStruct &git) {
    for (auto &gffs : git.getList("Creature List")) {
        shared_ptr<Creature> creature(_game->objectFactory().newCreature());
        creature->load(gffs);
        landObject(*creature);
        add(creature);
    }
}

void Area::loadDoors(const GffStruct &git) {
    for (auto &gffs : git.getList("Door List")) {
        shared_ptr<Door> door(_game->objectFactory().newDoor());
        door->load(gffs);
        add(door);
    }
}

void Area::loadPlaceables(const GffStruct &git) {
    for (auto &gffs : git.getList("Placeable List")) {
        shared_ptr<Placeable> placeable(_game->objectFactory().newPlaceable());
        placeable->load(gffs);
        add(placeable);
    }
}

void Area::loadWaypoints(const GffStruct &git) {
    for (auto &gffs : git.getList("WaypointList")) {
        shared_ptr<Waypoint> waypoint(_game->objectFactory().newWaypoint());
        waypoint->load(gffs);
        add(waypoint);
    }
}

void Area::loadTriggers(const GffStruct &git) {
    for (auto &gffs : git.getList("TriggerList")) {
        shared_ptr<Trigger> trigger(_game->objectFactory().newTrigger());
        trigger->load(gffs);
        add(trigger);
    }
}

void Area::loadCameras(const glm::vec3 &entryPosition, float entryHeading) {
    glm::vec3 position(entryPosition);
    position.z += 1.7f;

    SceneGraph *sceneGraph = &_game->sceneGraph();

    _firstPersonCamera = make_unique<FirstPersonCamera>(sceneGraph, _cameraAspect, glm::radians(kDefaultFieldOfView));
    _firstPersonCamera->setPosition(position);
    _firstPersonCamera->setHeading(entryHeading);

    _thirdPersonCamera = make_unique<ThirdPersonCamera>(sceneGraph, _cameraAspect, _cameraStyle);
    _thirdPersonCamera->setFindObstacle(bind(&Area::findCameraObstacle, this, _1, _2, _3));
    _thirdPersonCamera->setTargetPosition(position);
    _thirdPersonCamera->setHeading(entryHeading);

    _dialogCamera = make_unique<DialogCamera>(sceneGraph, _cameraStyle, _cameraAspect);
    _dialogCamera->setFindObstacle(bind(&Area::findCameraObstacle, this, _1, _2, _3));

    _animatedCamera = make_unique<AnimatedCamera>(sceneGraph, _cameraAspect);
    _game->onCameraChanged(_cameraType);
}

bool Area::findCameraObstacle(const glm::vec3 &origin, const glm::vec3 &dest, glm::vec3 &intersection) const {
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
}

bool Area::findCreatureObstacle(const Creature &creature, const glm::vec3 &dest) const {
    glm::vec3 origin(creature.position());
    origin.z += kCreatureObstacleTestZ;

    glm::vec3 adjustedDest(dest);
    adjustedDest.z += kCreatureObstacleTestZ;

    glm::vec3 originToDest(adjustedDest - origin);
    glm::vec3 dir(glm::normalize(originToDest));

    RaycastProperties props;
    props.flags = kRaycastObjects | kRaycastAABB;
    props.origin = origin;
    props.direction = dir;
    props.except = &creature;

    RaycastResult result;

    if (_collisionDetector.raycast(props, result)) {
        return result.distance <= glm::length(originToDest);
    }

    return false;
}

void Area::add(const shared_ptr<SpatialObject> &object) {
    _objects.push_back(object);
    _objectsByType[object->type()].push_back(object);
    _objectById.insert(make_pair(object->id(), object));
    _objectsByTag[object->tag()].push_back(object);

    determineObjectRoom(*object);
}

void Area::determineObjectRoom(SpatialObject &object) {
    glm::vec3 position(object.position());
    Room *room = nullptr;

    if (getElevationAt(position, room, position.z)) {
        object.setRoom(room);
    }
}

void Area::doDestroyObjects() {
    for (auto &object : _objectsToDestroy) {
        doDestroyObject(object);
    }
    _objectsToDestroy.clear();
}

void Area::doDestroyObject(uint32_t objectId) {
    shared_ptr<SpatialObject> object(find(objectId));
    if (!object) return;
    {
        Room *room = object->room();
        if (room) {
            room->removeTenant(object.get());
        }
    }
    {
        shared_ptr<ModelSceneNode> sceneNode(object->model());
        if (sceneNode) {
            _game->sceneGraph().removeRoot(sceneNode);
        }
    }
    {
        auto maybeObject = find_if(_objects.begin(), _objects.end(), [&object](const shared_ptr<SpatialObject> &o) { return o.get() == object.get(); });
        if (maybeObject != _objects.end()) {
            _objects.erase(maybeObject);
        }
    }
    _objectById.erase(objectId);
    {
        auto maybeTagObjects = _objectsByTag.find(object->tag());
        if (maybeTagObjects != _objectsByTag.end()) {
            ObjectList &tagObjects = maybeTagObjects->second;
            auto maybeObject = find_if(tagObjects.begin(), tagObjects.end(), [&object](const shared_ptr<SpatialObject> &o) { return o.get() == object.get(); });
            if (maybeObject != tagObjects.end()) {
                tagObjects.erase(maybeObject);
            }
            _objectsByTag.erase(maybeTagObjects);
        }
    }
    {
        ObjectList &typeObjects = _objectsByType.find(object->type())->second;
        auto maybeObject = find_if(typeObjects.begin(), typeObjects.end(), [&object](const shared_ptr<SpatialObject> &o) { return o.get() == object.get(); });
        if (maybeObject != typeObjects.end()) {
            typeObjects.erase(maybeObject);
        }
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

void Area::loadParty(const glm::vec3 &position, float heading) {
    Party &party = _game->party();

    for (int i = 0; i < party.size(); ++i) {
        shared_ptr<Creature> member(party.getMember(i));
        member->setPosition(position);
        member->setHeading(heading);

        landObject(*member);
        add(member);
    }
}

void Area::unloadParty() {
    Party &party = _game->party();

    for (int i = 0; i < party.size(); ++i) {
        doDestroyObject(party.getMember(i)->id());
    }
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
    doDestroyObjects();

    _objectSelector.update();
    _game->sceneGraph().prepare();
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

    if (findCreatureObstacle(creature, position)) {
        return false;
    }
    if (getElevationAt(position, room, position.z)) {
        creature.setRoom(room);
        creature.setPosition(position);
        return true;
    }

    return false;
}

void Area::runOnEnterScript() {
    shared_ptr<Creature> player(_game->party().player());
    if (!player) return;

    if (!_scripts[ScriptType::OnEnter].empty()) {
        runScript(_scripts[ScriptType::OnEnter], _id, player->id(), -1);
    }
}

SpatialObject *Area::getObjectAt(int x, int y) const {
    Camera *camera = getCamera();
    shared_ptr<CameraSceneNode> sceneNode(camera->sceneNode());

    const GraphicsOptions &opts = _game->options().graphics;
    glm::vec4 viewport(0.0f, 0.0f, opts.width, opts.height);
    glm::vec3 fromWorld(glm::unProject(glm::vec3(x, opts.height - y, 0.0f), sceneNode->view(), sceneNode->projection(), viewport));
    glm::vec3 toWorld(glm::unProject(glm::vec3(x, opts.height - y, 1.0f), sceneNode->view(), sceneNode->projection(), viewport));

    shared_ptr<Creature> player(_game->party().player());

    RaycastProperties props;
    props.flags = kRaycastObjects | kRaycastAABB;
    props.origin = fromWorld;
    props.direction = glm::normalize(toWorld - fromWorld);
    props.except = player.get();

    RaycastResult result;

    if (_collisionDetector.raycast(props, result)) {
        return result.object;
    }

    return nullptr;
}

void Area::destroyObject(const SpatialObject &object) {
    _objectsToDestroy.insert(object.id());
}

void Area::fill(SceneGraph &sceneGraph) {
    sceneGraph.clear();

    for (auto &room : _rooms) {
        shared_ptr<ModelSceneNode> sceneNode(room.second->model());
        if (sceneNode) {
            sceneGraph.addRoot(sceneNode);
        }
    }
    for (auto &object : _objects) {
        shared_ptr<ModelSceneNode> sceneNode(object->model());
        if (sceneNode) {
            sceneGraph.addRoot(sceneNode);
        }
    }
}

void Area::fill(const UpdateContext &updateCtx, GuiContext &guiCtx) {
    addPartyMemberPortrait(_game->party().getMember(0), guiCtx);
    addPartyMemberPortrait(_game->party().getMember(1), guiCtx);
    addPartyMemberPortrait(_game->party().getMember(2), guiCtx);

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

void Area::update3rdPersonCameraHeading() {
    shared_ptr<SpatialObject> partyLeader(_game->party().leader());
    if (!partyLeader) return;

    _thirdPersonCamera->setHeading(partyLeader->heading());
}

void Area::switchTo3rdPersonCamera() {
    if (_cameraType == CameraType::ThirdPerson) return;

    _cameraType = CameraType::ThirdPerson;
    _game->onCameraChanged(_cameraType);
}

void Area::toggleCameraType() {
    bool changed = false;

    switch (_cameraType) {
        case CameraType::FirstPerson:
            if (_game->party().leader()) {
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

        default:
            break;
    }

    if (changed) {
        _game->onCameraChanged(_cameraType);
    }
}

Camera *Area::getCamera() const {
    return _cameraType == CameraType::ThirdPerson ? _thirdPersonCamera.get() : static_cast<Camera *>(_firstPersonCamera.get());
}

void Area::startDialog(Creature &creature, const string &resRef) {
    string finalResRef(resRef);
    if (resRef.empty()) finalResRef = creature.conversation();
    if (resRef.empty()) return;

    _game->startDialog(creature, finalResRef);
}

void Area::onPartyLeaderMoved() {
    shared_ptr<Creature> partyLeader(_game->party().leader());
    if (!partyLeader) return;

    update3rdPersonCameraTarget();
    updateRoomVisibility();
    _objectSelector.selectNearest();
    updateTriggers(*partyLeader);
}

void Area::update3rdPersonCameraTarget() {
    shared_ptr<SpatialObject> partyLeader(_game->party().leader());
    if (!partyLeader) return;

    glm::vec3 position;

    if (partyLeader->model()->getNodeAbsolutePosition("camerahook", position)) {
        position += partyLeader->position();
    } else {
        position = partyLeader->position();
    }
    _thirdPersonCamera->setTargetPosition(position);
}

void Area::updateRoomVisibility() {
    shared_ptr<Creature> partyLeader(_game->party().leader());
    if (!partyLeader) return;

    Room *leaderRoom = partyLeader->room();
    if (!leaderRoom) return;

    for (auto &room : _rooms) {
        room.second->setVisible(false);
    }
    leaderRoom->setVisible(true);

    auto adjRooms = _visibility->equal_range(leaderRoom->name());
    for (auto adjRoom = adjRooms.first; adjRoom != adjRooms.second; adjRoom++) {
        auto room = _rooms.find(adjRoom->second);
        if (room != _rooms.end()) {
            room->second->setVisible(true);
        }
    }
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
            if (!trigger.linkedToModule().empty()) {
                _game->scheduleModuleTransition(trigger.linkedToModule(), trigger.linkedTo());
            }
            break;
        }
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

ObjectSelector &Area::objectSelector() {
    return _objectSelector;
}

const Pathfinder &Area::pathfinder() const {
    return _pathfinder;
}

ThirdPersonCamera *Area::thirdPersonCamera() {
    return _thirdPersonCamera.get();
}

DialogCamera &Area::dialogCamera() {
    return *_dialogCamera;
}

AnimatedCamera &Area::animatedCamera() {
    return *_animatedCamera;
}

} // namespace game

} // namespace reone
