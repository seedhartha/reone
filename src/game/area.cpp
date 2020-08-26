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

#include <cassert>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

#include "SDL2/SDL.h"

#include "../core/jobs.h"
#include "../core/log.h"
#include "../core/streamutil.h"
#include "../resources/lytfile.h"
#include "../resources/resources.h"
#include "../resources/visfile.h"
#include "../script/execution.h"

#include "object/factory.h"
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
static const float kElevationTestOffset = 0.1f;
static const float kElevationTestDistance = 1.0f;

static const char kPartyLeaderTag[] = "party-leader";
static const char kPartyMember1Tag[] = "party-member-1";
static const char kPartyMember2Tag[] = "party-member-2";

Area::Area(uint32_t id, GameVersion version, ObjectFactory *factory) :
    Object(id), _version(version), _objectFactory(factory), _navMesh(new NavMesh()) {

    assert(_objectFactory);

    _type = ObjectType::Area;

    _objects.insert(make_pair(ObjectType::Creature, ObjectList()));
    _objects.insert(make_pair(ObjectType::Door, ObjectList()));
    _objects.insert(make_pair(ObjectType::Placeable, ObjectList()));
    _objects.insert(make_pair(ObjectType::Waypoint, ObjectList()));
    _objects.insert(make_pair(ObjectType::Trigger, ObjectList()));

    _renderLists.insert(make_pair(RenderListName::Opaque, RenderList()));
    _renderLists.insert(make_pair(RenderListName::Transparent, RenderList()));
}

void Area::load(const string &name, const GffStruct &are, const GffStruct &git) {
    _name = name;

    loadProperties(git.getStruct("AreaProperties"));
    loadVisibility();
    loadLayout();
    loadCameraStyle(are);
    loadScripts(are);

    for (auto &gffs : git.getList("Creature List")) {
        shared_ptr<Creature> creature(_objectFactory->newCreature());
        creature->load(gffs);
        landObject(*creature);
        creature->setSynchronize(true);
        _objects[ObjectType::Creature].push_back(move(creature));
    }
    for (auto &gffs : git.getList("Door List")) {
        shared_ptr<Door> door(_objectFactory->newDoor());
        door->load(gffs);
        door->setSynchronize(true);
        _objects[ObjectType::Door].push_back(move(door));
    }
    for (auto &gffs : git.getList("Placeable List")) {
        shared_ptr<Placeable> placeable(_objectFactory->newPlaceable());
        placeable->load(gffs);
        if (placeable->walkmesh()) {
            _navMesh->add(placeable->walkmesh(), placeable->transform());
        }
        _objects[ObjectType::Placeable].push_back(move(placeable));
    }
    for (auto &gffs : git.getList("WaypointList")) {
        shared_ptr<Waypoint> waypoint(_objectFactory->newWaypoint());
        waypoint->load(gffs);
        _objects[ObjectType::Waypoint].push_back(move(waypoint));
    }
    for (auto &gffs : git.getList("TriggerList")) {
        shared_ptr<Trigger> trigger(_objectFactory->newTrigger());
        trigger->load(gffs);
        _objects[ObjectType::Trigger].push_back(move(trigger));
    }

    TheJobExecutor.enqueue([this](const atomic_bool &cancel) {
        debug("Area: compute nav mesh");
        _navMesh->compute(cancel);
        debug("Area: nav mesh computed");
    });
}

void Area::loadProperties(const GffStruct &gffs) {
    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<TwoDaTable> musicTable(resources.find2DA("ambientmusic"));

    int musicIdx = gffs.getInt("MusicDay");
    if (musicIdx) {
        _music = musicTable->getString(musicIdx, "resource");
    }
}

void Area::loadLayout() {
    ResourceManager &resources = ResourceManager::instance();

    LytFile lyt;
    lyt.load(wrap(resources.find(_name, ResourceType::AreaLayout)));

    for (auto &lytRoom : lyt.rooms()) {
        unique_ptr<ModelInstance> model(new ModelInstance(resources.findModel(lytRoom.name)));
        model->animate("animloop1", kAnimationLoop);

        shared_ptr<Walkmesh> walkmesh(resources.findWalkmesh(lytRoom.name, ResourceType::Walkmesh));
        if (walkmesh) {
            _navMesh->add(walkmesh, glm::mat4(1.0f));
        }

        unique_ptr<Room> room(new Room(lytRoom.name, lytRoom.position, move(model), move(walkmesh)));
        _rooms.push_back(move(room));
    }
}

void Area::loadVisibility() {
    ResourceManager &resources = ResourceManager::instance();

    VisFile vis;
    vis.load(wrap(resources.find(_name, ResourceType::Vis)));

    _visibility = make_unique<Visibility>(vis.visibility());
}

void Area::loadCameraStyle(const GffStruct &are) {
    int styleIdx = are.getInt("CameraStyle");

    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<TwoDaTable> styleTable(resources.find2DA("camerastyle"));

    _cameraStyle.distance = styleTable->getFloat(styleIdx, "distance", 0.0f);
    _cameraStyle.pitch = styleTable->getFloat(styleIdx, "pitch", 0.0f);
    _cameraStyle.viewAngle = styleTable->getFloat(styleIdx, "viewangle", 0.0f);
    _cameraStyle.height = styleTable->getFloat(styleIdx, "height", 0.0f);
}

void Area::loadScripts(const GffStruct &are) {
    _scripts[ScriptType::OnEnter] = are.getString("OnEnter");
    _scripts[ScriptType::OnExit] = are.getString("OnExit");
    _scripts[ScriptType::OnHeartbeat] = are.getString("OnHeartbeat");
    _scripts[ScriptType::OnUserDefined] = are.getString("OnUserDefined");
}

void Area::landObject(SpatialObject &object) {
    glm::vec3 position(object.position());
    if (findElevationAt(position, position.z)) {
        object.setPosition(position);
    }
}

void Area::loadParty(const PartyConfiguration &party, const glm::vec3 &position, float heading) {
    if (party.memberCount > 0) {
        shared_ptr<Creature> partyLeader(makeCharacter(party.leader, kPartyLeaderTag, position, heading));
        _objects[ObjectType::Creature].push_back(partyLeader);
        landObject(*partyLeader);
        partyLeader->setSynchronize(true);
        _player = partyLeader;
        _partyLeader = partyLeader;
    }

    if (party.memberCount > 1) {
        shared_ptr<Creature> partyMember(makeCharacter(party.member1, kPartyMember1Tag, position, heading));
        landObject(*partyMember);
        partyMember->setSynchronize(true);
        _objects[ObjectType::Creature].push_back(partyMember);
        _partyMember1 = partyMember;

        Creature::Action action(Creature::ActionType::Follow, _partyLeader, kPartyMemberFollowDistance);
        partyMember->enqueueAction(move(action));
    }

    if (party.memberCount > 2) {
        shared_ptr<Creature> partyMember(makeCharacter(party.member2, kPartyMember2Tag, position, heading));
        landObject(*partyMember);
        partyMember->setSynchronize(true);
        _objects[ObjectType::Creature].push_back(partyMember);
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
    return false;
}

void Area::update(const UpdateContext &updateCtx, GuiContext &guiCtx) {
    updateDelayedCommands();

    for (auto &creature : _objects[ObjectType::Creature]) {
        updateCreature(static_cast<Creature &>(*creature), updateCtx.deltaTime);
    }

    if (_partyLeader) {
        guiCtx.hud.partyPortraits.push_back(static_cast<Creature &>(*_partyLeader).portrait());
    }
    if (_partyMember1) {
        guiCtx.hud.partyPortraits.push_back(static_cast<Creature &>(*_partyMember1).portrait());
    }
    if (_partyMember2) {
        guiCtx.hud.partyPortraits.push_back(static_cast<Creature &>(*_partyMember2).portrait());
    }

    for (auto &room : _rooms) {
        shared_ptr<ModelInstance> model(room->model());
        if (model) {
            model->update(updateCtx.deltaTime);
        }
    }
    for (auto &pair : _objects) {
        for (auto &object : pair.second) {
            object->update(updateCtx);
        }
    }

    fillRenderLists(updateCtx.cameraPosition);

    switch (_debugMode) {
        case DebugMode::ModelNodes:
            for (auto &list : _renderLists) {
                for (auto &item : list.second) {
                    if (glm::distance2(item.origin, updateCtx.cameraPosition) > kDrawDebugDistance) continue;

                    addToDebugContext(item, updateCtx, guiCtx.debug);
                }
            }
            break;
        case DebugMode::GameObjects:
            for (auto &list : _objects) {
                for (auto &object : list.second) {
                    if (glm::distance2(object->position(), updateCtx.cameraPosition) > kDrawDebugDistance) continue;

                    addToDebugContext(*object, updateCtx, guiCtx.debug);
                }
            }
            break;
        default:
            break;
    }
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

bool Area::moveCreatureTowards(Creature &creature, const glm::vec3 &point, float dt) {
    glm::vec3 position(creature.position());
    glm::vec3 delta = point - position;
    glm::vec3 dir(glm::normalize(delta));

    float heading = -glm::atan(dir.x, dir.y);
    creature.setHeading(heading);

    glm::vec3 newPosition(position);
    newPosition.x += creature.runSpeed() * dir.x * dt;
    newPosition.y += creature.runSpeed() * dir.y * dt;

    SpatialObject *obstacle = nullptr;
    glm::vec3 intersection(0.0f);

    if (findObstacleByAABB(position, newPosition + 1.0f * dir, kObstacleCreature, &creature, &obstacle)) {
        return false;
    }
    if (findElevationAt(newPosition, newPosition.z)) {
        creature.setPosition(newPosition);
        if (&creature == &*_partyLeader) {
            updateTriggers(creature);
        }
    }

    return true;
}

void Area::updateTriggers(const Creature &creature) {
    glm::vec3 liftedPosition(creature.position() + glm::vec3(0.0f, 0.0f, kElevationTestOffset));
    glm::vec3 down(0.0f, 0.0f, -kElevationTestDistance);
    glm::vec2 intersection;
    float distance;

    for (auto &object : _objects[ObjectType::Trigger]) {
        Trigger &trigger = static_cast<Trigger &>(*object);
        if (trigger.distanceTo(liftedPosition) > kMaxDistanceToTestCollision) continue;

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

void Area::addToDebugContext(const RenderListItem &item, const UpdateContext &updateCtx, DebugContext &debugCtx) const {
    const ModelNode &node = *item.node;

    glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec3 position(item.transform[3]);

    DebugObject object;
    object.tag = node.name();
    object.text = str(boost::format("%s %s") % item.model->name() % node.name());
    object.screenCoords = glm::project(item.origin, updateCtx.view, updateCtx.projection, viewport);

    debugCtx.objects.push_back(move(object));
}

void Area::addToDebugContext(const SpatialObject &object, const UpdateContext &updateCtx, DebugContext &debugCtx) const {
    glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec3 position(object.position());

    DebugObject debugObj;
    debugObj.tag = object.tag();
    debugObj.text = str(boost::format("%s %.2f %.2f %.2f") % object.tag() % position.x % position.y % position.z);
    debugObj.screenCoords = glm::project(position, updateCtx.view, updateCtx.projection, viewport);

    debugCtx.objects.push_back(move(debugObj));
}

void Area::setDebugMode(DebugMode mode) {
    _debugMode = mode;
}

void Area::saveTo(GameState &state) const {
    AreaState areaState;

    for (auto &list : _objects) {
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

    for (auto &list : _objects) {
        if (list.first != ObjectType::Creature && list.first != ObjectType::Door) continue;

        for (auto &object : list.second) {
            object->loadState(areaState);
        }
    }
}

shared_ptr<SpatialObject> Area::find(uint32_t id) const {
    shared_ptr<SpatialObject> object;
    for (auto &list : _objects) {
        object = find(id, list.first);
        if (object) return object;
    }

    return nullptr;
}

shared_ptr<SpatialObject> Area::find(const string &tag, int nth) const {
    shared_ptr<SpatialObject> object;
    for (auto &list : _objects) {
        object = find(tag, list.first, nth);
        if (object) return object;
    }

    return nullptr;
}

shared_ptr<SpatialObject> Area::find(uint32_t id, ObjectType type) const {
    const ObjectList &list = _objects.find(type)->second;

    auto it = find_if(
        list.begin(),
        list.end(),
        [&id](const shared_ptr<SpatialObject> &object) { return object->id() == id; });

    return it != list.end() ? *it : nullptr;
}

shared_ptr<SpatialObject> Area::find(const string &tag, ObjectType type, int nth) const {
    const ObjectList &list = _objects.find(type)->second;
    int idx = 0;

    for (auto &object : list) {
        if (object->tag() == tag && idx++ == nth) return object;
    }

    return nullptr;
}

bool Area::findObstacleByWalkmesh(const glm::vec3 &from, const glm::vec3 &to, int mask, glm::vec3 &intersection, SpatialObject **obstacle) const {
    vector<pair<SpatialObject *, float>> candidates;

    for (auto &list : _objects) {
        if (list.first != ObjectType::Door && list.first != ObjectType::Placeable) continue;
        if (list.first == ObjectType::Door && (mask & kObstacleDoor) == 0) continue;
        if (list.first == ObjectType::Placeable && (mask & kObstaclePlaceable) == 0) continue;

        for (auto &object : list.second) {
            if (!object->walkmesh() ||
                (object->type() == ObjectType::Door && static_cast<Door &>(*object).isOpen())) continue;

            float distToFrom = object->distanceTo(from);
            if (distToFrom > kMaxDistanceToTestCollision) continue;

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
        shared_ptr<Walkmesh> walkmesh(room->walkmesh());
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

    for (auto &list : _objects) {
        if (list.first != ObjectType::Creature &&
            list.first != ObjectType::Door &&
            list.first != ObjectType::Placeable) continue;

        if (list.first == ObjectType::Creature && (mask & kObstacleCreature) == 0) continue;
        if (list.first == ObjectType::Door && (mask & kObstacleDoor) == 0) continue;
        if (list.first == ObjectType::Placeable && (mask & kObstaclePlaceable) == 0) continue;

        for (auto &object : list.second) {
            if (!object->model() ||
                (object->type() == ObjectType::Door && static_cast<Door &>(*object).isOpen()) ||
                (except && object.get() == except)) continue;

            float distToFrom = object->distanceTo(from);
            if (distToFrom > kMaxDistanceToTestCollision) continue;

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

bool Area::findElevationAt(const glm::vec3 &position, float &z) const {
    glm::vec3 from(position + glm::vec3(0.0f, 0.0f, kElevationTestOffset));
    glm::vec3 to(from + glm::vec3(0.0f, 0.0f, -kElevationTestDistance));
    glm::vec3 intersection(0.0f);
    SpatialObject *obstacle = nullptr;

    if (findObstacleByWalkmesh(from, to, kObstacleDoor | kObstaclePlaceable, intersection, &obstacle)) {
        return false;
    }
    for (auto &room : _rooms) {
        shared_ptr<Walkmesh> walkmesh(room->walkmesh());
        if (!walkmesh) continue;

        AABB aabb(walkmesh->aabb());
        if (!aabb.contains(glm::vec2(position))) continue;

        if (room->walkmesh()->findElevationAt(from, z)) {
            return true;
        }
    }

    return false;
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
