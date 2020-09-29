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
#include <cassert>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

#include "SDL2/SDL.h"

#include "../core/debug.h"
#include "../core/jobs.h"
#include "../core/log.h"
#include "../core/streamutil.h"
#include "../render/scene/cubenode.h"
#include "../render/scene/scenegraph.h"
#include "../resources/lytfile.h"
#include "../resources/resources.h"
#include "../resources/visfile.h"
#include "../script/execution.h"

#include "object/factory.h"
#include "paths.h"
#include "script/routines.h"
#include "script/util.h"

using namespace std;

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resources;
using namespace reone::script;

namespace reone {

namespace game {

static const float kDrawDebugDistance = 64.0f;
static const float kPartyMemberFollowDistance = 4.0f;
static const float kMaxDistanceToTestCollision = 64.0f;
static const float kElevationTestRadius = 1024.0f;
static const float kSelectionDistance = 64.0f;

static const char kPartyLeaderTag[] = "party-leader";
static const char kPartyMember1Tag[] = "party-member-1";
static const char kPartyMember2Tag[] = "party-member-2";

Area::Area(uint32_t id, GameVersion version, ObjectFactory *objectFactory, SceneGraph *sceneGraph) :
    Object(id, ObjectType::Area),
    _version(version),
    _objectFactory(objectFactory),
    _sceneGraph(sceneGraph),
    _navMesh(new NavMesh()) {

    assert(_objectFactory);
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
    lyt.load(wrap(resources.find(_name, ResourceType::AreaLayout)));

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
    vis.load(wrap(Resources.find(_name, ResourceType::Vis)));

    _visibility = make_unique<Visibility>(vis.visibility());
}

void Area::loadPTH() {
    shared_ptr<GffStruct> pth(Resources.findGFF(_name, ResourceType::Path));
    assert(pth);

    Paths paths;
    paths.load(*pth);

    const vector<Paths::Point> &points = paths.points();
    unordered_map<int, float> pointZ;

    for (int i = 0; i < points.size(); ++i) {
        const Paths::Point &point = points[i];
        Room *room = nullptr;
        float z = 0.0f;

        if (!findRoomElevationAt(glm::vec2(point.x, point.y), room, z)) {
            warn(boost::format("Area: point %d elevation not found") % i);
            continue;
        }
        pointZ.insert(make_pair(i, z));

        shared_ptr<CubeSceneNode> aabb(new CubeSceneNode(_sceneGraph, 0.5f));
        aabb->setLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(point.x, point.y, z + 0.25f)));

        _sceneGraph->addRoot(aabb);
    }

    _navMesh->load(paths, pointZ);
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

    if (findRoomElevationAt(position, room, position.z)) {
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

    if (findRoomElevationAt(position, room, position.z)) {
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

        Creature::Action action(Creature::ActionType::Follow, _partyLeader, kPartyMemberFollowDistance);
        partyMember->enqueueAction(move(action));
    }
    if (party.memberCount > 2) {
        shared_ptr<Creature> partyMember(makeCharacter(party.member2, kPartyMember2Tag, position, heading));
        landObject(*partyMember);
        partyMember->setSynchronize(true);
        add(partyMember);
        _partyMember2 = partyMember;

        Creature::Action action(Creature::ActionType::Follow, _partyLeader, kPartyMemberFollowDistance);
        partyMember->enqueueAction(move(action));
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
        case SDL_KEYUP:
            return handleKeyUp(event.key);
        default:
            return false;
    }
}

bool Area::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_Q:
            selectNextObject(true);
            return true;

        case SDL_SCANCODE_E:
            selectNextObject();
            return true;

        default:
            return false;
    }
}

void Area::selectNextObject(bool reverse) {
    static vector<uint32_t> selectables;

    selectables.clear();
    getSelectableObjects(selectables);

    if (selectables.empty()) {
        _selectedObjectId = -1;
        return;
    }
    if (_selectedObjectId == -1) {
        _selectedObjectId = selectables.front();
        return;
    }
    if (reverse) {
        auto selected = std::find(selectables.rbegin(), selectables.rend(), _selectedObjectId);
        if (selected != selectables.rend()) {
            selected++;
        }
        _selectedObjectId = selected != selectables.rend() ? *selected : selectables.back();

    } else {
        auto selected = std::find(selectables.begin(), selectables.end(), _selectedObjectId);
        if (selected != selectables.end()) {
            selected++;
        }
        _selectedObjectId = selected != selectables.end() ? *selected : selectables.front();
    }
}

void Area::getSelectableObjects(vector<uint32_t> &ids) const {
    static vector<pair<uint32_t, float>> selectables;

    glm::vec3 origin(_player->position());
    selectables.clear();

    for (auto &object : _objects) {
        if (!object->isSelectable() || object.get() == _player.get()) continue;

        shared_ptr<ModelSceneNode> model(object->model());
        if (!model || !model->isVisible()) continue;

        float dist = object->distanceTo(origin);
        if (dist > kSelectionDistance) continue;

        selectables.push_back(make_pair(object->id(), dist));
    }

    sort(selectables.begin(), selectables.end(), [](const pair<uint32_t, float> &left, const pair<uint32_t, float> &right) {
        return left.second < right.second;
    });
    for (auto &selectable : selectables) {
        ids.push_back(selectable.first);
    }
}

bool Area::handleKeyUp(const SDL_KeyboardEvent &event) {
    return false;
}

void Area::update(const UpdateContext &updateCtx) {
    updateDelayedCommands();

    for (auto &creature : _objectsByType[ObjectType::Creature]) {
        updateCreature(static_cast<Creature &>(*creature), updateCtx.deltaTime);
    }
    for (auto &room : _rooms) {
        room.second->update(updateCtx.deltaTime);
    }
    for (auto &object : _objects) {
        object->update(updateCtx);
    }

    updateSelection();

    _sceneGraph->prepare(updateCtx.cameraPosition);
}

void Area::updateDelayedCommands() {
    uint32_t now = SDL_GetTicks();

    for (auto &command : _delayed) {
        if (now >= command.timestamp) {
            shared_ptr<ScriptProgram> program(command.context.savedState->program);

            debug(boost::format("Area: run delayed: %s %08x") % program->name() % command.context.savedState->insOffset);
            ScriptExecution(program, command.context).run();

            command.executed = true;
        }
    }

    auto it = remove_if(
        _delayed.begin(),
        _delayed.end(),
        [](const DelayedCommand &command) { return command.executed; });

    _delayed.erase(it, _delayed.end());
}

bool Area::moveCreatureTowards(Creature &creature, const glm::vec2 &point, float dt) {
    glm::vec3 position(creature.position());
    glm::vec2 delta = point - glm::vec2(position);
    glm::vec2 dir(glm::normalize(delta));

    float heading = -glm::atan(dir.x, dir.y);
    creature.setHeading(heading);

    glm::vec3 newPosition(position);
    newPosition.x += creature.runSpeed() * dir.x * dt;
    newPosition.y += creature.runSpeed() * dir.y * dt;

    SpatialObject *obstacle = nullptr;
    Room *room = nullptr;

    if (findObstacleByAABB(position, newPosition + 1.0f * glm::vec3(dir, 0.0f), kObstacleCreature, &creature, &obstacle)) {
        return false;
    }
    if (findElevationAt(newPosition, room, newPosition.z)) {
        creature.setRoom(room);
        creature.setPosition(newPosition);

        if (&creature == &*_partyLeader) {
            updateTriggers(creature);
        }
    }

    return true;
}

void Area::updateTriggers(const Creature &creature) {
    glm::vec2 position2d(creature.position());
    glm::vec3 liftedPosition(position2d, kElevationTestRadius);
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

void Area::delayCommand(uint32_t timestamp, const ExecutionContext &ctx) {
    DelayedCommand action;
    action.timestamp = timestamp;
    action.context = ctx;
    _delayed.push_back(action);
}

int Area::eventUserDefined(int eventNumber) {
    int id = _eventCounter++;

    UserDefinedEvent event { eventNumber };
    _events.insert(make_pair(id, move(event)));

    return id;
}

void Area::signalEvent(int eventId) {
    auto it = _events.find(eventId);
    if (it == _events.end()) {
        warn("Event not found by id: " + to_string(eventId));
        return;
    }
    if (!_scripts[ScriptType::OnUserDefined].empty()) {
        runScript(_scripts[ScriptType::OnUserDefined], _id, kObjectInvalid, it->second.eventNumber);
    }
    _events.erase(it);
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

bool Area::findObstacleByWalkmesh(const glm::vec3 &from, const glm::vec3 &to, int mask, glm::vec3 &intersection, SpatialObject **obstacle) const {
    vector<pair<SpatialObject *, float>> candidates;

    for (auto &list : _objectsByType) {
        if (list.first != ObjectType::Door && list.first != ObjectType::Placeable) continue;
        if (list.first == ObjectType::Door && (mask & kObstacleDoor) == 0) continue;
        if (list.first == ObjectType::Placeable && (mask & kObstaclePlaceable) == 0) continue;

        for (auto &object : list.second) {
            if (!object->walkmesh() ||
                (object->type() == ObjectType::Door && static_cast<Door &>(*object).isOpen())) continue;

            shared_ptr<ModelSceneNode> model(object->model());
            if (!model || !model->isVisible() || !model->isOnScreen()) continue;

            float distToFrom = object->distanceTo(from);

            candidates.push_back(make_pair(object.get(), distToFrom));
        }
    }

    sort(
        candidates.begin(),
        candidates.end(),
        [](const pair<SpatialObject *, float> &left, const pair<SpatialObject *, float> &right) { return left.second < right.second; });

    for (auto &pair : candidates) {
        SpatialObject &object = *pair.first;

        glm::mat4 invObjectTransform(glm::inverse(object.transform()));
        glm::vec3 relFrom(invObjectTransform * glm::vec4(from, 1.0f));
        glm::vec3 relTo(invObjectTransform * glm::vec4(to, 1.0f));

        if (object.walkmesh()->findObstacle(relFrom, relTo, intersection)) {
            intersection = object.transform() * glm::vec4(intersection, 1.0f);
            *obstacle = &object;
            return true;
        }
    }

    if ((mask & kObstacleRoom) == 0) return false;

    for (auto &room : _rooms) {
        const Walkmesh *walkmesh = room.second->walkmesh();
        if (!walkmesh) continue;

        AABB aabb(walkmesh->aabb());
        if (!aabb.contains(from) && !aabb.contains(to)) continue;

        if (walkmesh->findObstacle(from, to, intersection)) {
            return true;
        }
    }

    return false;
}

bool Area::findObstacleByAABB(const glm::vec3 &from, const glm::vec3 &to, int mask, const SpatialObject *except, SpatialObject **obstacle) const {
    vector<pair<SpatialObject *, float>> candidates;

    for (auto &list : _objectsByType) {
        if (list.first != ObjectType::Creature &&
            list.first != ObjectType::Door &&
            list.first != ObjectType::Placeable) continue;

        if (list.first == ObjectType::Creature && (mask & kObstacleCreature) == 0) continue;
        if (list.first == ObjectType::Door && (mask & kObstacleDoor) == 0) continue;
        if (list.first == ObjectType::Placeable && (mask & kObstaclePlaceable) == 0) continue;

        for (auto &object : list.second) {
            shared_ptr<ModelSceneNode> model(object->model());
            if (!model ||
                !model->isVisible() ||
                !model->isOnScreen() ||
                (object->type() == ObjectType::Door && static_cast<Door &>(*object).isOpen()) ||
                (except && object.get() == except)) continue;

            float distToFrom = object->distanceTo(from);

            candidates.push_back(make_pair(object.get(), distToFrom));
        }
    }

    sort(
        candidates.begin(),
        candidates.end(),
        [](const pair<SpatialObject *, float> &left, const pair<SpatialObject *, float> &right) { return left.second < right.second; });

    for (auto &pair : candidates) {
        SpatialObject *object = pair.first;
        AABB aabb(object->model()->model()->aabb() * object->transform());
        float distance = 0.0f;

        if (aabb.intersectLine(from, to, distance) && distance > 0.0f) {
            *obstacle = &*object;
            return true;
        }
    }

    return false;
}

bool Area::findElevationAt(const glm::vec2 &position, Room *&roomAt, float &z) const {
    glm::vec3 from(position, kElevationTestRadius);
    glm::vec3 to(position, -kElevationTestRadius);
    glm::vec3 intersection(0.0f);
    SpatialObject *obstacle = nullptr;

    if (findObstacleByWalkmesh(from, to, kObstacleDoor | kObstaclePlaceable, intersection, &obstacle)) {
        return false;
    }
    if (findRoomElevationAt(position, roomAt, z)) {
        return true;
    }

    return false;
}

bool Area::findRoomElevationAt(const glm::vec2 &position, Room *&roomAt, float &z) const {
    glm::vec3 from(position, kElevationTestRadius);
    for (auto &pair : _rooms) {
        Room *room = pair.second.get();

        const Walkmesh *walkmesh = room->walkmesh();
        if (!walkmesh) continue;

        AABB aabb(walkmesh->aabb());
        if (!aabb.contains(position)) continue;

        if (room->walkmesh()->findElevationAt(from, z)) {
            roomAt = room;
            return true;
        }
    }

    return false;
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

void Area::selectNearestObject() {
    _selectedObjectId = -1;
    selectNextObject();
}

void Area::hilight(uint32_t objectId) {
    _hilightedObjectId = objectId;
}

void Area::select(uint32_t objectId) {
    _selectedObjectId = objectId;
}

void Area::updateSelection() {
    if (_hilightedObjectId != -1) {
        shared_ptr<SpatialObject> object(find(_hilightedObjectId));
        if (!object || !object->isSelectable()) {
            _hilightedObjectId = -1;
        }
    }
    if (_selectedObjectId != -1) {
        shared_ptr<SpatialObject> object(find(_selectedObjectId));
        if (!object || !object->isSelectable()) {
            _selectedObjectId = -1;
        }
    }
}

void Area::fill(const UpdateContext &updateCtx, GuiContext &guiCtx) {
    addPartyMemberPortrait(_partyLeader, guiCtx);
    addPartyMemberPortrait(_partyMember1, guiCtx);
    addPartyMemberPortrait(_partyMember2, guiCtx);

    if (_hilightedObjectId != -1) {
        glm::vec3 coords(getScreenCenterOfObject(_hilightedObjectId, updateCtx));
        if (coords.z < 1.0f) {
            guiCtx.target.hasHilighted = true;
            guiCtx.target.hilightedScreenCoords = coords;
        }
    }
    if (_selectedObjectId != -1) {
        glm::vec3 coords(getScreenCenterOfObject(_selectedObjectId, updateCtx));
        if (coords.z < 1.0f) {
            guiCtx.target.hasSelected = true;
            guiCtx.target.selectedScreenCoords = coords;
        }
    }

    addDebugInfo(updateCtx, guiCtx);
}

void Area::addPartyMemberPortrait(const shared_ptr<SpatialObject> &object, GuiContext &ctx) {
    if (object) {
        ctx.hud.partyPortraits.push_back(static_cast<Creature &>(*object).portrait());
    }
}

glm::vec3 Area::getScreenCenterOfObject(uint32_t objectId, const UpdateContext &ctx) const {
    static glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);

    shared_ptr<SpatialObject> object(find(objectId));
    glm::vec3 center(object->model()->getCenterOfAABB());

    return glm::project(center, ctx.view, ctx.projection, viewport);
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

uint32_t Area::selectedObjectId() const {
    return _selectedObjectId;
}

const CameraStyle &Area::cameraStyle() const {
    return _cameraStyle;
}

const string &Area::music() const {
    return _music;
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
