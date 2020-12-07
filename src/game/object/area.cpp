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

#include "area.h"

#include <algorithm>
#include <sstream>

#include <boost/format.hpp>

#include "glm/gtx/norm.hpp"

#include "../../common/log.h"
#include "../../common/streamutil.h"
#include "../../render/models.h"
#include "../../render/walkmeshes.h"
#include "../../resource/lytfile.h"
#include "../../resource/visfile.h"
#include "../../resource/resources.h"
#include "../../scene/node/cubenode.h"

#include "../blueprint/trigger.h"
#include "../blueprint/sound.h"
#include "../game.h"
#include "../room.h"
#include "../script/util.h"

#include "objectfactory.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static const float kDefaultFieldOfView = 75.0f;
static const float kMaxDistanceToTestCollision = 8.0f;
static const float kElevationTestZ = 1024.0f;
static const float kCreatureObstacleTestZ = 0.1f;
static const int kMaxSoundCount = 4;

Area::Area(uint32_t id, Game *game) :
    Object(id, ObjectType::Area),
    _game(game),
    _collisionDetector(this),
    _objectSelector(this, &game->party()),
    _actionExecutor(game),
    _combat(game),
    _map(game) {

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
        shared_ptr<Model> model(Models::instance().get(lytRoom.name));
        if (!model) continue;

        glm::vec3 position(lytRoom.position.x, lytRoom.position.y, lytRoom.position.z);

        shared_ptr<ModelSceneNode> sceneNode(new ModelSceneNode(&_game->sceneGraph(), model));
        sceneNode->setLocalTransform(glm::translate(glm::mat4(1.0f), position));
        sceneNode->playAnimation("animloop1", kAnimationLoop);

        shared_ptr<Walkmesh> walkmesh(Walkmeshes::instance().get(lytRoom.name, ResourceType::Walkmesh));
        unique_ptr<Room> room(new Room(lytRoom.name, position, sceneNode, walkmesh));

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

    Path path;
    path.load(*pth);

    const vector<Path::Point> &points = path.points();
    unordered_map<int, float> pointZ;

    for (int i = 0; i < points.size(); ++i) {
        const Path::Point &point = points[i];
        Room *room = nullptr;
        float z = 0.0f;

        if (!getElevationAt(glm::vec2(point.x, point.y), nullptr, room, z)) {
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
    loadMap(are);
}

void Area::loadCameraStyle(const GffStruct &are) {
    int styleIdx = are.getInt("CameraStyle");
    shared_ptr<TwoDaTable> styleTable(Resources::instance().get2DA("camerastyle"));

    _cameraStyle.distance = styleTable->getFloat(styleIdx, "distance", 0.0f);
    _cameraStyle.pitch = styleTable->getFloat(styleIdx, "pitch", 0.0f);
    _cameraStyle.viewAngle = styleTable->getFloat(styleIdx, "viewangle", 0.0f);
    _cameraStyle.height = styleTable->getFloat(styleIdx, "height", 0.0f);

    auto combatStyleRow = styleTable->findRowByColumnValue("name", "Combat");
    _combatCamStyle.distance = combatStyleRow->getFloat("distance");
    _combatCamStyle.pitch = combatStyleRow->getFloat("pitch");
    _combatCamStyle.viewAngle = combatStyleRow->getFloat("viewangle");
    _combatCamStyle.height = combatStyleRow->getFloat("height");

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
    _onEnter = are.getString("OnEnter");
    _onExit = are.getString("OnExit");
    _onHeartbeat = are.getString("OnHeartbeat");
    _onUserDefined = are.getString("OnUserDefined");
}

void Area::loadMap(const GffStruct &are) {
    _map.load(_name, are.getStruct("Map"));
}

void Area::loadGIT(const GffStruct &git) {
    loadProperties(git);
    loadCreatures(git);
    loadDoors(git);
    loadPlaceables(git);
    loadWaypoints(git);
    loadTriggers(git);
    loadSounds(git);
    loadCameras(git);
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

void Area::loadSounds(const GffStruct &git) {
    for (auto &gffs : git.getList("SoundList")) {
        shared_ptr<Sound> sound(_game->objectFactory().newSound());
        sound->load(gffs);
        add(sound);
    }
}

void Area::loadCameras(const GffStruct &git) {
    for (auto &gffs : git.getList("CameraList")) {
        shared_ptr<CameraObject> camera(_game->objectFactory().newCamera());
        camera->load(gffs);
        add(camera);
    }
}

void Area::initCameras(const glm::vec3 &entryPosition, float entryHeading) {
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
    _staticCamera = make_unique<StaticCamera>(sceneGraph, _cameraAspect);
}

bool Area::findCameraObstacle(const glm::vec3 &origin, const glm::vec3 &dest, glm::vec3 &intersection) const {
    glm::vec3 originToDest(dest - origin);
    glm::vec3 dir(glm::normalize(originToDest));

    RaycastProperties props;
    props.flags = kRaycastRooms | kRaycastObjects;
    props.origin = origin;
    props.direction = dir;
    props.objectTypes = { ObjectType::Door };

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
    props.objectTypes = { ObjectType::Creature, ObjectType::Door };
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

    if (getElevationAt(position, &object, room, position.z)) {
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
        auto maybeObject = find_if(_objects.begin(), _objects.end(), [&object](auto &o) { return o.get() == object.get(); });
        if (maybeObject != _objects.end()) {
            _objects.erase(maybeObject);
        }
    }
    _objectById.erase(objectId);
    {
        auto maybeTagObjects = _objectsByTag.find(object->tag());
        if (maybeTagObjects != _objectsByTag.end()) {
            ObjectList &tagObjects = maybeTagObjects->second;
            auto maybeObject = find_if(tagObjects.begin(), tagObjects.end(), [&object](auto &o) { return o.get() == object.get(); });
            if (maybeObject != tagObjects.end()) {
                tagObjects.erase(maybeObject);
            }
            if (tagObjects.empty()) {
                _objectsByTag.erase(maybeTagObjects);
            }
        }
    }
    {
        ObjectList &typeObjects = _objectsByType.find(object->type())->second;
        auto maybeObject = find_if(typeObjects.begin(), typeObjects.end(), [&object](auto &o) { return o.get() == object.get(); });
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

ObjectList &Area::getObjectsByType(ObjectType type) {
    return _objectsByType.find(type)->second;
}

void Area::landObject(SpatialObject &object) {
    glm::vec3 position(object.position());
    float heading = object.heading();
    Room *room = nullptr;

    if (getElevationAt(position, &object, room, position.z)) {
        object.setPosition(position);
        return;
    }
    for (int i = 0; i < 4; ++i) {
        float angle = i * glm::half_pi<float>();
        position = object.position() + glm::vec3(glm::sin(angle), glm::cos(angle), 0.0f);

        if (getElevationAt(position, &object, room, position.z)) {
            object.setPosition(position);
            return;
        }
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

        case SDL_SCANCODE_SLASH: {
            int objectId = _objectSelector.selectedObjectId();
            if (objectId != -1) {
                shared_ptr<SpatialObject> object(find(objectId));
                printDebugInfo(*object);
            }
            return true;
        }

        default:
            return false;
    }
}

void Area::printDebugInfo(const SpatialObject &object) {
    ostringstream ss;
    ss << boost::format("tag='%s'") % object.tag();
    ss << boost::format(",pos=[%0.2f,%0.2f,%0.2f]") % object.position().x % object.position().y % object.position().z;
    ss << boost::format(",model='%s'") % object.model()->name();

    debug("Selected object: " + ss.str());
}

bool Area::getElevationAt(const glm::vec2 &position, const SpatialObject *except, Room *&room, float &z) const {
    RaycastProperties props;
    props.origin = glm::vec3(position, kElevationTestZ);
    props.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    props.maxDistance = 2.0f * kElevationTestZ;

    RaycastResult result;

    props.flags = kRaycastObjects;
    props.objectTypes = { ObjectType::Placeable };
    props.except = nullptr;

    if (_collisionDetector.raycast(props, result)) return false;

    props.flags = kRaycastObjects | kRaycastAABB;
    props.objectTypes = { ObjectType::Creature };
    props.except = except;

    if (_collisionDetector.raycast(props, result)) return false;

    props.flags = kRaycastRooms | kRaycastWalkable;
    props.objectTypes.clear();
    props.except = nullptr;

    if (_collisionDetector.raycast(props, result)) {
        room = result.room;
        z = result.intersection.z;
        return true;
    }

    return false;
}

void Area::update(float dt) {
    doDestroyObjects();

    Object::update(dt);
    _actionExecutor.executeActions(*this, dt);

    for (auto &room : _rooms) {
        room.second->update(dt);
    }
    updateVisibility();
    updateSounds();

    for (auto &object : _objects) {
        object->update(dt);
        _actionExecutor.executeActions(*object, dt);
    }
    _objectSelector.update();
    updateHeartbeat(dt);

    // TODO: enable when polished enough
    _combat.update(dt);
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
    if (getElevationAt(position, &creature, room, position.z)) {
        creature.setRoom(room);
        creature.setPosition(position);

        if (creature.id() == _game->party().leader()->id()) {
            onPartyLeaderMoved();
        }
        checkTriggersIntersection(creature);
        return true;
    }

    return false;
}

void Area::runOnEnterScript() {
    shared_ptr<Creature> player(_game->party().player());
    if (!player) return;

    if (!_onEnter.empty()) {
        runScript(_onEnter, _id, player->id(), -1);
    }
}

shared_ptr<SpatialObject> Area::getObjectAt(int x, int y) const {
    Camera *camera = _game->getActiveCamera();
    shared_ptr<CameraSceneNode> sceneNode(camera->sceneNode());

    const GraphicsOptions &opts = _game->options().graphics;
    glm::vec4 viewport(0.0f, 0.0f, opts.width, opts.height);
    glm::vec3 fromWorld(glm::unProject(glm::vec3(x, opts.height - y, 0.0f), sceneNode->view(), sceneNode->projection(), viewport));
    glm::vec3 toWorld(glm::unProject(glm::vec3(x, opts.height - y, 1.0f), sceneNode->view(), sceneNode->projection(), viewport));

    shared_ptr<Creature> partyLeader(_game->party().leader());

    RaycastProperties props;
    props.flags = kRaycastObjects | kRaycastAABB | kRaycastSelectable;
    props.origin = fromWorld;
    props.direction = glm::normalize(toWorld - fromWorld);
    props.objectTypes = { ObjectType::Creature, ObjectType::Door, ObjectType::Placeable };
    props.except = partyLeader.get();
    props.maxDistance = 2.0f * kSelectionDistance;

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
    sceneGraph.build();
}

glm::vec3 Area::getSelectableScreenCoords(const shared_ptr<SpatialObject> &object, const glm::mat4 &projection, const glm::mat4 &view) const {
    static glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);

    glm::vec3 position(object->selectablePosition());

    return glm::project(position, view, projection, viewport);
}

void Area::update3rdPersonCameraHeading() {
    shared_ptr<SpatialObject> partyLeader(_game->party().leader());
    if (!partyLeader) return;

    _thirdPersonCamera->setHeading(partyLeader->heading());
}

void Area::startDialog(SpatialObject &object, const string &resRef) {
    string finalResRef(resRef);
    if (resRef.empty()) finalResRef = object.conversation();
    if (resRef.empty()) return;

    _game->startDialog(object, finalResRef);
}

void Area::onPartyLeaderMoved() {
    shared_ptr<Creature> partyLeader(_game->party().leader());
    if (!partyLeader) return;

    update3rdPersonCameraTarget();
    _objectSelector.selectNearest();
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

void Area::updateVisibility() {
    shared_ptr<Creature> partyLeader(_game->party().leader());
    Room *leaderRoom = partyLeader ? partyLeader->room() : nullptr;
    bool allVisible = _game->cameraType() != CameraType::ThirdPerson || !leaderRoom;

    if (allVisible) {
        for (auto &room : _rooms) {
            room.second->setVisible(true);
        }
    } else {
        auto adjRoomNames = _visibility->equal_range(leaderRoom->name());
        for (auto &room : _rooms) {
            bool visible = !leaderRoom || room.second.get() == leaderRoom;
            if (!visible) {
                for (auto adjRoom = adjRoomNames.first; adjRoom != adjRoomNames.second; adjRoom++) {
                    if (adjRoom->second == room.first) {
                        visible = true;
                        break;
                    }
                }
            }
            room.second->setVisible(visible);
        }
    }

    Camera *camera = _game->getActiveCamera();
    if (!camera) return;

    shared_ptr<CameraSceneNode> cameraNode(camera->sceneNode());
    glm::vec4 viewport(-1.0f, -1.0f, 1.0f, 1.0f);

    for (auto &object : _objects) {
        if (!object->visible()) continue;

        shared_ptr<ModelSceneNode> model(object->model());
        if (!model) continue;

        AABB aabb(model->aabb() * model->absoluteTransform());
        glm::vec3 objectCenter(model->getCenterOfAABB());
        glm::vec3 cameraPosition(cameraNode->absoluteTransform()[3]);
        float distanceToCamera = glm::distance2(objectCenter, cameraPosition);
        float drawDistance = object->drawDistance();
        bool onScreen = distanceToCamera < drawDistance && cameraNode->isInFrustum(aabb);
        model->setOnScreen(onScreen);
    }
}

void Area::updateSounds() {
    Camera *camera = _game->getActiveCamera();
    if (!camera) return;

    vector<pair<Sound *, float>> soundDistances;
    glm::vec3 cameraPosition(camera->sceneNode()->absoluteTransform()[3]);

    for (auto &sound : _objectsByType[ObjectType::Sound]) {
        Sound *soundPtr = static_cast<Sound *>(sound.get());
        soundPtr->setAudible(false);

        if (!soundPtr->isActive()) continue;

        shared_ptr<SoundBlueprint> blueprint(soundPtr->blueprint());
        float maxDist = blueprint->maxDistance();

        float dist = soundPtr->distanceTo(cameraPosition);
        if (dist > maxDist) continue;

        soundDistances.push_back(make_pair(soundPtr, dist));
    }

    sort(soundDistances.begin(), soundDistances.end(), [](auto &left, auto &right) {
        int leftPriority = left.first->priority();
        int rightPriority = right.first->priority();

        if (leftPriority < rightPriority) return true;
        if (leftPriority > rightPriority) return false;

        return left.second < right.second;
    });
    if (soundDistances.size() > kMaxSoundCount) {
        soundDistances.erase(soundDistances.begin() + kMaxSoundCount, soundDistances.end());
    }
    for (auto &sound : soundDistances) {
        sound.first->setAudible(true);
    }
}

void Area::checkTriggersIntersection(SpatialObject &triggerrer) {
    glm::vec2 position2d(triggerrer.position());
    shared_ptr<SpatialObject> triggererPtr(find(triggerrer.id()));

    for (auto &object : _objectsByType[ObjectType::Trigger]) {
        Trigger &trigger = static_cast<Trigger &>(*object);
        if (trigger.distanceTo(position2d) > kMaxDistanceToTestCollision) continue;
        if (trigger.isTenant(triggererPtr) || !trigger.isIn(position2d)) continue;

        debug(boost::format("Trigger %s triggerred by %s") % trigger.tag() % triggerrer.tag());
        trigger.addTenant(triggererPtr);

        if (!trigger.linkedToModule().empty()) {
            _game->scheduleModuleTransition(trigger.linkedToModule(), trigger.linkedTo());
            return;
        }
        if (!trigger.blueprint().onEnter().empty()) {
            runScript(trigger.blueprint().onEnter(), trigger.id(), triggerrer.id(), -1);
        }
    }
}

void Area::updateHeartbeat(float dt) {
    _heartbeatTimeout = glm::max(0.0f, _heartbeatTimeout - dt);

    if (_heartbeatTimeout == 0.0f) {
        if (!_onHeartbeat.empty()) {
            runScript(_onHeartbeat, _id, -1, -1);
        }
        _heartbeatTimeout = kHeartbeatInterval;
    }
}

const CameraStyle &Area::cameraStyle() const {
    return _cameraStyle;
}

const string &Area::music() const {
    return _music;
}

const RoomMap &Area::rooms() const {
    return _rooms;
}

Combat &Area::combat() {
    return _combat;
}

Map &Area::map() {
    return _map;
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

Camera &Area::getCamera(CameraType type) {
    switch (type) {
        case CameraType::FirstPerson:
            return *_firstPersonCamera;
        case CameraType::ThirdPerson:
            return *_thirdPersonCamera;
        case CameraType::Static:
            return *_staticCamera;
        case CameraType::Animated:
            return *_animatedCamera;
        case CameraType::Dialog:
            return *_dialogCamera;
        default:
            throw invalid_argument("Unsupported camera type: " + to_string(static_cast<int>(type)));
    }
}

void Area::setStaticCamera(int cameraId) {
    for (auto &object : _objectsByType[ObjectType::Camera]) {
        CameraObject &camera = static_cast<CameraObject &>(*object);
        if (camera.cameraId() == cameraId) {
            _staticCamera->setObject(camera);
            break;
        }
    }
}

void Area::setCombatTPCamera() {
    _thirdPersonCamera->setStyle(_combatCamStyle);
}

void Area::setDefaultTPCamera() {
    _thirdPersonCamera->setStyle(_cameraStyle);
}

} // namespace game

} // namespace reone
