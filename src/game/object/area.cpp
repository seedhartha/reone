/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../common/randomutil.h"
#include "../../common/streamutil.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/models.h"
#include "../../graphics/textures.h"
#include "../../graphics/walkmesh.h"
#include "../../graphics/walkmeshes.h"
#include "../../resource/2da.h"
#include "../../resource/2das.h"
#include "../../resource/gffs.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"
#include "../../scene/collision.h"
#include "../../scene/graphs.h"
#include "../../scene/node/grass.h"
#include "../../scene/node/grasscluster.h"
#include "../../scene/node/model.h"
#include "../../scene/node/sound.h"
#include "../../scene/node/walkmesh.h"
#include "../../scene/types.h"

#include "../camerastyles.h"
#include "../game.h"
#include "../layouts.h"
#include "../location.h"
#include "../party.h"
#include "../paths.h"
#include "../reputes.h"
#include "../room.h"
#include "../script/runner.h"
#include "../services.h"
#include "../surfaces.h"
#include "../types.h"
#include "../visibilities.h"

#include "factory.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static constexpr float kDefaultFieldOfView = 75.0f;
static constexpr float kUpdatePerceptionInterval = 1.0f; // seconds
static constexpr float kLineOfSightHeight = 1.7f;        // TODO: make it appearance-based
static constexpr float kLineOfSightFOV = glm::radians(60.0f);

static constexpr float kMaxCollisionDistance = 8.0f;
static constexpr float kMaxCollisionDistance2 = kMaxCollisionDistance * kMaxCollisionDistance;

static glm::vec3 g_defaultAmbientColor {0.2f};
static CameraStyle g_defaultCameraStyle {"", 3.2f, 83.0f, 0.45f, 55.0f};

static bool g_debugPath = false;

Area::Area(
    uint32_t id,
    string sceneName,
    Game &game,
    Services &services) :
    Object(
        id,
        ObjectType::Area,
        game,
        services),
    _sceneName(move(sceneName)) {

    init();
    _heartbeatTimer.setTimeout(kHeartbeatInterval);
}

void Area::init() {
    const GraphicsOptions &opts = _game.options().graphics;
    _cameraAspect = opts.width / static_cast<float>(opts.height);

    _objectsByType.insert(make_pair(ObjectType::Creature, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Item, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Trigger, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Door, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::AreaOfEffect, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Waypoint, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Placeable, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Store, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Encounter, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Sound, ObjectList()));
}

void Area::load(string name, const GffStruct &are, const GffStruct &git, bool fromSave) {
    _name = move(name);

    loadARE(are);
    loadGIT(git);
    loadLYT();
    loadVIS();
    loadPTH();
}

void Area::loadARE(const GffStruct &are) {
    _localizedName = _services.strings.get(are.getInt("Name"));

    loadCameraStyle(are);
    loadAmbientColor(are);
    loadScripts(are);
    loadMap(are);
    loadStealthXP(are);
    loadGrass(are);
    loadFog(are);
}

void Area::loadCameraStyle(const GffStruct &are) {
    // Area
    int areaStyleIdx = are.getInt("CameraStyle");
    shared_ptr<CameraStyle> areaStyle(_services.cameraStyles.get(areaStyleIdx));
    if (areaStyle) {
        _camStyleDefault = *areaStyle;
    } else {
        _camStyleDefault = g_defaultCameraStyle;
    }

    // Combat
    shared_ptr<CameraStyle> combatStyle(_services.cameraStyles.get("Combat"));
    if (combatStyle) {
        _camStyleDefault = *combatStyle;
    } else {
        _camStyleCombat = g_defaultCameraStyle;
    }
}

void Area::loadAmbientColor(const GffStruct &are) {
    _ambientColor = are.getColor("DynAmbientColor", g_defaultAmbientColor);

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    sceneGraph.setAmbientLightColor(_ambientColor);
}

void Area::loadScripts(const GffStruct &are) {
    _onEnter = are.getString("OnEnter");
    _onExit = are.getString("OnExit");
    _onHeartbeat = are.getString("OnHeartbeat");
    _onUserDefined = are.getString("OnUserDefined");
}

void Area::loadMap(const GffStruct &are) {
    auto mapStruct = are.getStruct("Map");
    if (!mapStruct) {
        warn("Map properties not found in ARE");
        return;
    }
    _game.map().load(_name, *mapStruct);
}

void Area::loadStealthXP(const GffStruct &are) {
    _stealthXPEnabled = are.getBool("StealthXPEnabled");
    _stealthXPDecrement = are.getInt("StealthXPLoss"); // TODO: loss = decrement?
    _maxStealthXP = are.getInt("StealthXPMax");
}

void Area::loadGrass(const GffStruct &are) {
    string texName(boost::to_lower_copy(are.getString("Grass_TexName")));
    if (!texName.empty()) {
        _grass.texture = _services.textures.get(texName, TextureUsage::Diffuse);
    }
    _grass.density = are.getFloat("Grass_Density");
    _grass.quadSize = are.getFloat("Grass_QuadSize");
    _grass.ambient = are.getInt("Grass_Ambient");
    _grass.diffuse = are.getInt("Grass_Diffuse");
    _grass.probabilities[0] = are.getFloat("Grass_Prob_UL");
    _grass.probabilities[1] = are.getFloat("Grass_Prob_UR");
    _grass.probabilities[2] = are.getFloat("Grass_Prob_LL");
    _grass.probabilities[3] = are.getFloat("Grass_Prob_LR");
}

void Area::loadFog(const GffStruct &are) {
    _fogEnabled = are.getBool("SunFogOn");
    _fogNear = are.getFloat("SunFogNear");
    _fogFar = are.getFloat("SunFogFar");
    _fogColor = are.getColor("SunFogColor");

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    sceneGraph.setFogEnabled(_fogEnabled);
    sceneGraph.setFogNear(_fogNear);
    sceneGraph.setFogFar(_fogFar);
    sceneGraph.setFogColor(_fogColor);
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
    loadEncounters(git);
}

void Area::loadProperties(const GffStruct &git) {
    shared_ptr<GffStruct> props(git.getStruct("AreaProperties"));
    if (!props) {
        warn("Area properties not found in GIT");
        return;
    }
    int musicIdx = props->getInt("MusicDay");
    if (musicIdx) {
        shared_ptr<TwoDA> musicTable(_services.twoDas.get("ambientmusic"));
        _music = musicTable->getString(musicIdx, "resource");
    }
}

void Area::loadCreatures(const GffStruct &git) {
    for (auto &gffs : git.getList("Creature List")) {
        shared_ptr<Creature> creature(_game.objectFactory().newCreature(_sceneName));
        creature->loadFromGIT(*gffs);
        landObject(*creature);
        add(creature);
    }
}

void Area::loadDoors(const GffStruct &git) {
    for (auto &gffs : git.getList("Door List")) {
        shared_ptr<Door> door(_game.objectFactory().newDoor(_sceneName));
        door->loadFromGIT(*gffs);
        add(door);
    }
}

void Area::loadPlaceables(const GffStruct &git) {
    for (auto &gffs : git.getList("Placeable List")) {
        shared_ptr<Placeable> placeable(_game.objectFactory().newPlaceable(_sceneName));
        placeable->loadFromGIT(*gffs);
        add(placeable);
    }
}

void Area::loadWaypoints(const GffStruct &git) {
    for (auto &gffs : git.getList("WaypointList")) {
        shared_ptr<Waypoint> waypoint(_game.objectFactory().newWaypoint(_sceneName));
        waypoint->loadFromGIT(*gffs);
        add(waypoint);
    }
}

void Area::loadTriggers(const GffStruct &git) {
    for (auto &gffs : git.getList("TriggerList")) {
        shared_ptr<Trigger> trigger(_game.objectFactory().newTrigger(_sceneName));
        trigger->loadFromGIT(*gffs);
        add(trigger);
    }
}

void Area::loadSounds(const GffStruct &git) {
    for (auto &gffs : git.getList("SoundList")) {
        shared_ptr<Sound> sound(_game.objectFactory().newSound(_sceneName));
        sound->loadFromGIT(*gffs);
        add(sound);
    }
}

void Area::loadCameras(const GffStruct &git) {
    for (auto &gffs : git.getList("CameraList")) {
        shared_ptr<PlaceableCamera> camera(_game.objectFactory().newCamera(_sceneName));
        camera->loadFromGIT(*gffs);
        add(camera);
    }
}

void Area::loadEncounters(const GffStruct &git) {
    for (auto &gffs : git.getList("Encounter List")) {
        shared_ptr<Encounter> encounter(_game.objectFactory().newEncounter(_sceneName));
        encounter->loadFromGIT(*gffs);
        add(encounter);
    }
}

void Area::loadLYT() {
    auto layout = _services.layouts.get(_name);
    if (!layout) {
        throw ValidationException("Area LYT file not found");
    }
    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    for (auto &lytRoom : layout->rooms) {
        auto model = _services.models.get(lytRoom.name);
        if (!model) {
            continue;
        }

        // Model
        glm::vec3 position(lytRoom.position.x, lytRoom.position.y, lytRoom.position.z);
        shared_ptr<ModelSceneNode> modelSceneNode(sceneGraph.newModel(model, ModelUsage::Room));
        modelSceneNode->setLocalTransform(glm::translate(glm::mat4(1.0f), position));
        for (auto &anim : model->getAnimationNames()) {
            if (boost::starts_with(anim, "animloop")) {
                modelSceneNode->playAnimation(anim, AnimationProperties::fromFlags(AnimationFlags::loopOverlay));
            }
        }
        sceneGraph.addRoot(modelSceneNode);

        // Walkmesh
        shared_ptr<WalkmeshSceneNode> walkmeshSceneNode;
        auto walkmesh = _services.walkmeshes.get(lytRoom.name, ResourceType::Wok);
        if (walkmesh) {
            walkmeshSceneNode = sceneGraph.newWalkmesh(walkmesh);
            sceneGraph.addRoot(walkmeshSceneNode);
        }

        // Grass
        shared_ptr<GrassSceneNode> grassSceneNode;
        auto aabbNode = modelSceneNode->model().getAABBNode();
        if (_grass.texture && aabbNode && _game.options().graphics.grass) {
            grassSceneNode = sceneGraph.newGrass(
                _grass.density,
                _grass.quadSize,
                _grass.probabilities,
                _services.surfaces.getGrassSurfaces(),
                _grass.texture,
                aabbNode);
            grassSceneNode->setLocalTransform(glm::translate(position) * aabbNode->absoluteTransform());
            sceneGraph.addRoot(grassSceneNode);
        }

        auto room = make_unique<Room>(lytRoom.name, position, move(modelSceneNode), walkmeshSceneNode, move(grassSceneNode));
        if (walkmeshSceneNode) {
            walkmeshSceneNode->setUser(*room);
        }
        _rooms.insert(make_pair(room->name(), move(room)));
    }
}

void Area::loadVIS() {
    auto visibility = _services.visibilities.get(_name);
    if (!visibility) {
        return;
    }
    _visibility = fixVisibility(*visibility);
}

Visibility Area::fixVisibility(const Visibility &visibility) {
    Visibility result;
    for (auto &pair : visibility) {
        result.insert(pair);
        result.insert(make_pair(pair.second, pair.first));
    }
    return move(result);
}

void Area::loadPTH() {
    shared_ptr<Path> path(_services.paths.get(_name));
    if (!path) {
        return;
    }
    unordered_map<int, float> pointZ;

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);

    for (size_t i = 0; i < path->points.size(); ++i) {
        const Path::Point &point = path->points[i];
        Collision collision;
        if (!sceneGraph.testElevation(glm::vec2(point.x, point.y), collision)) {
            warn(boost::format("Point %d elevation not found") % i);
            continue;
        }
        pointZ.insert(make_pair(static_cast<int>(i), collision.intersection.z));
    }

    _pathfinder.load(path->points, pointZ);
}

void Area::initCameras(const glm::vec3 &entryPosition, float entryFacing) {
    glm::vec3 position(entryPosition);
    position.z += 1.7f;

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);

    _firstPersonCamera = make_unique<FirstPersonCamera>(glm::radians(kDefaultFieldOfView), _cameraAspect, sceneGraph);
    _firstPersonCamera->setPosition(position);
    _firstPersonCamera->setFacing(entryFacing);

    _thirdPersonCamera = make_unique<ThirdPersonCamera>(_camStyleDefault, _cameraAspect, _game, sceneGraph);
    _thirdPersonCamera->setTargetPosition(position);
    _thirdPersonCamera->setFacing(entryFacing);

    _dialogCamera = make_unique<DialogCamera>(_camStyleDefault, _cameraAspect, sceneGraph);
    _animatedCamera = make_unique<AnimatedCamera>(_cameraAspect, sceneGraph);
    _staticCamera = make_unique<StaticCamera>(_cameraAspect, sceneGraph);
}

void Area::add(const shared_ptr<SpatialObject> &object) {
    _objects.push_back(object);
    _objectsByType[object->type()].push_back(object);
    _objectsByTag[object->tag()].push_back(object);

    determineObjectRoom(*object);

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    auto sceneNode = object->sceneNode();
    if (sceneNode) {
        if (sceneNode->type() == SceneNodeType::Model) {
            sceneGraph.addRoot(static_pointer_cast<ModelSceneNode>(sceneNode));
        } else if (sceneNode->type() == SceneNodeType::Sound) {
            sceneGraph.addRoot(static_pointer_cast<SoundSceneNode>(sceneNode));
        }
    }
    if (object->type() == ObjectType::Placeable) {
        auto placeable = static_pointer_cast<Placeable>(object);
        auto walkmesh = placeable->walkmesh();
        if (walkmesh) {
            sceneGraph.addRoot(walkmesh);
        }
    } else if (object->type() == ObjectType::Door) {
        auto door = static_pointer_cast<Door>(object);
        auto walkmeshClosed = door->walkmeshClosed();
        if (walkmeshClosed) {
            sceneGraph.addRoot(walkmeshClosed);
        }
        auto walkmeshOpen1 = door->walkmeshOpen1();
        if (walkmeshOpen1) {
            sceneGraph.addRoot(walkmeshOpen1);
        }
        auto walkmeshOpen2 = door->walkmeshOpen2();
        if (walkmeshOpen2) {
            sceneGraph.addRoot(walkmeshOpen2);
        }
    }
}

void Area::determineObjectRoom(SpatialObject &object) {
    Room *room = nullptr;

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    Collision collision;
    if (sceneGraph.testElevation(object.position(), collision)) {
        auto userRoom = dynamic_cast<Room *>(collision.user);
        if (userRoom) {
            room = room;
        }
    }

    object.setRoom(room);
}

void Area::doDestroyObjects() {
    for (auto &object : _objectsToDestroy) {
        doDestroyObject(object);
    }
    _objectsToDestroy.clear();
}

void Area::doDestroyObject(uint32_t objectId) {
    shared_ptr<SpatialObject> object(dynamic_pointer_cast<SpatialObject>(_game.objectFactory().getObjectById(objectId)));
    if (!object) {
        return;
    }
    auto room = object->room();
    if (room) {
        room->removeTenant(object.get());
    }

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    auto sceneNode = object->sceneNode();
    if (sceneNode) {
        if (sceneNode->type() == SceneNodeType::Model) {
            sceneGraph.removeRoot(static_pointer_cast<ModelSceneNode>(sceneNode));
        } else if (sceneNode->type() == SceneNodeType::Sound) {
            sceneGraph.removeRoot(static_pointer_cast<SoundSceneNode>(sceneNode));
        }
    }
    if (object->type() == ObjectType::Placeable) {
        auto placeable = static_pointer_cast<Placeable>(object);
        auto walkmesh = placeable->walkmesh();
        if (walkmesh) {
            sceneGraph.removeRoot(walkmesh);
        }
    } else if (object->type() == ObjectType::Door) {
        auto door = static_pointer_cast<Door>(object);
        auto walkmeshOpen1 = door->walkmeshOpen1();
        if (walkmeshOpen1) {
            sceneGraph.removeRoot(walkmeshOpen1);
        }
        auto walkmeshOpen2 = door->walkmeshOpen2();
        if (walkmeshOpen2) {
            sceneGraph.removeRoot(walkmeshOpen2);
        }
        auto walkmeshClosed = door->walkmeshClosed();
        if (walkmeshClosed) {
            sceneGraph.removeRoot(walkmeshClosed);
        }
    }

    auto maybeObject = find_if(_objects.begin(), _objects.end(), [&object](auto &o) { return o.get() == object.get(); });
    if (maybeObject != _objects.end()) {
        _objects.erase(maybeObject);
    }
    auto maybeTagObjects = _objectsByTag.find(object->tag());
    if (maybeTagObjects != _objectsByTag.end()) {
        auto &tagObjects = maybeTagObjects->second;
        auto maybeObjectByTag = find_if(tagObjects.begin(), tagObjects.end(), [&object](auto &o) { return o.get() == object.get(); });
        if (maybeObjectByTag != tagObjects.end()) {
            tagObjects.erase(maybeObjectByTag);
        }
        if (tagObjects.empty()) {
            _objectsByTag.erase(maybeTagObjects);
        }
    }
    auto &typeObjects = _objectsByType.find(object->type())->second;
    auto maybeObjectByType = find_if(typeObjects.begin(), typeObjects.end(), [&object](auto &o) { return o.get() == object.get(); });
    if (maybeObjectByType != typeObjects.end()) {
        typeObjects.erase(maybeObjectByType);
    }
}

ObjectList &Area::getObjectsByType(ObjectType type) {
    return _objectsByType.find(type)->second;
}

shared_ptr<SpatialObject> Area::getObjectByTag(const string &tag, int nth) const {
    auto objects = _objectsByTag.find(tag);
    if (objects == _objectsByTag.end())
        return nullptr;
    if (nth >= objects->second.size())
        return nullptr;

    return objects->second[nth];
}

void Area::landObject(SpatialObject &object) {
    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    glm::vec3 position(object.position());
    Collision collision;

    // Test elevation at object position
    if (sceneGraph.testElevation(position, collision)) {
        object.setPosition(collision.intersection);
        return;
    }

    // Test elevations in a circle around object position
    for (int i = 0; i < 4; ++i) {
        float angle = i * glm::half_pi<float>();
        position = object.position() + glm::vec3(glm::sin(angle), glm::cos(angle), 0.0f);

        if (sceneGraph.testElevation(position, collision)) {
            object.setPosition(collision.intersection);
            return;
        }
    }
}

void Area::loadParty(const glm::vec3 &position, float facing, bool fromSave) {
    Party &party = _game.party();

    for (int i = 0; i < party.getSize(); ++i) {
        shared_ptr<Creature> member(party.getMember(i));
        if (!fromSave) {
            member->setPosition(position);
            member->setFacing(facing);
        }
        landObject(*member);
        add(member);
    }
}

void Area::unloadParty() {
    for (auto &member : _game.party().members()) {
        doDestroyObject(member.creature->id());
    }
}

void Area::reloadParty() {
    shared_ptr<Creature> player(_game.party().player());
    loadParty(player->position(), player->getFacing());
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
    return false;
}

void Area::update(float dt) {
    doDestroyObjects();
    updateVisibility();
    updateObjectSelection();

    if (!_game.isPaused()) {
        Object::update(dt);

        for (auto &object : _objects) {
            object->update(dt);
        }

        updatePerception(dt);
        updateHeartbeat(dt);
    }
}

bool Area::moveCreature(const shared_ptr<Creature> &creature, const glm::vec2 &dir, bool run, float dt) {
    static glm::vec3 up {0.0f, 0.0f, 1.0f};
    static glm::vec3 zOffset {0.0f, 0.0f, 0.1f};

    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);
    Collision collision;

    // Set creature facing

    float facing = -glm::atan(dir.x, dir.y);
    creature->setFacing(facing);

    // Test obstacle between origin and destination

    glm::vec3 origin(creature->position());
    origin.z += 0.1f;

    float speed = run ? creature->runSpeed() : creature->walkSpeed();
    float speedDt = speed * dt;

    glm::vec3 dest(origin);
    dest.x += dir.x * speedDt;
    dest.y += dir.y * speedDt;

    if (sceneGraph.testWalk(origin, dest, creature.get(), collision)) {
        // Try moving along the surface
        glm::vec2 right(glm::normalize(glm::vec2(glm::cross(up, collision.normal))));
        glm::vec2 newDir(glm::normalize(right * glm::dot(dir, right)));

        dest = origin;
        dest.x += newDir.x * speedDt;
        dest.y += newDir.y * speedDt;

        if (sceneGraph.testWalk(origin, dest, creature.get(), collision)) {
            return false;
        }
    }

    // Test elevation at destination

    if (!sceneGraph.testElevation(dest, collision)) {
        return false;
    }

    auto userRoom = dynamic_cast<Room *>(collision.user);
    auto prevRoom = creature->room();

    creature->setRoom(userRoom);
    creature->setPosition(glm::vec3(dest.x, dest.y, collision.intersection.z));
    creature->setWalkmeshMaterial(collision.material);

    if (creature == _game.party().getLeader()) {
        onPartyLeaderMoved(userRoom != prevRoom);
    }

    checkTriggersIntersection(creature);

    return true;
}

bool Area::moveCreatureTowards(const shared_ptr<Creature> &creature, const glm::vec2 &dest, bool run, float dt) {
    glm::vec2 delta(dest - glm::vec2(creature->position()));
    glm::vec2 dir(glm::normalize(delta));
    return moveCreature(creature, dir, run, dt);
}

bool Area::isObjectSeen(const Creature &subject, const SpatialObject &object) const {
    if (!subject.isInLineOfSight(object, kLineOfSightFOV)) {
        return false;
    }
    auto &sceneGraph = _services.sceneGraphs.get(_sceneName);

    glm::vec3 origin(subject.position());
    origin.z += kLineOfSightHeight;

    glm::vec3 dest(object.position());
    dest.z += kLineOfSightHeight;

    Collision collision;
    if (sceneGraph.testLineOfSight(origin, dest, collision)) {
        return collision.user == &object ||
               subject.getSquareDistanceTo(object) < glm::distance2(origin, collision.intersection);
    }

    return true;
}

void Area::runSpawnScripts() {
    for (auto &creature : _objectsByType[ObjectType::Creature]) {
        static_cast<Creature &>(*creature).runSpawnScript();
    }
}

void Area::runOnEnterScript() {
    if (_onEnter.empty())
        return;

    auto player = _game.party().player();
    if (!player)
        return;

    _game.scriptRunner().run(_onEnter, _id, player->id());
}

void Area::runOnExitScript() {
    if (_onExit.empty())
        return;

    auto player = _game.party().player();
    if (!player)
        return;

    _game.scriptRunner().run(_onExit, _id, player->id());
}

void Area::destroyObject(const SpatialObject &object) {
    _objectsToDestroy.insert(object.id());
}

glm::vec3 Area::getSelectableScreenCoords(const shared_ptr<SpatialObject> &object, const glm::mat4 &projection, const glm::mat4 &view) const {
    static glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);

    glm::vec3 position(object->getSelectablePosition());

    return glm::project(position, view, projection, viewport);
}

void Area::update3rdPersonCameraFacing() {
    shared_ptr<SpatialObject> partyLeader(_game.party().getLeader());
    if (!partyLeader)
        return;

    _thirdPersonCamera->setFacing(partyLeader->getFacing());
}

void Area::startDialog(const shared_ptr<SpatialObject> &object, const string &resRef) {
    string finalResRef(resRef);
    if (resRef.empty())
        finalResRef = object->conversation();
    if (resRef.empty())
        return;

    _game.startDialog(object, finalResRef);
}

void Area::onPartyLeaderMoved(bool roomChanged) {
    shared_ptr<Creature> partyLeader(_game.party().getLeader());
    if (!partyLeader)
        return;

    if (roomChanged) {
        updateRoomVisibility();
    }
    update3rdPersonCameraTarget();
}

void Area::updateRoomVisibility() {
    shared_ptr<Creature> partyLeader(_game.party().getLeader());
    Room *leaderRoom = partyLeader ? partyLeader->room() : nullptr;
    bool allVisible = _game.cameraType() != CameraType::ThirdPerson || !leaderRoom;

    if (allVisible) {
        for (auto &room : _rooms) {
            room.second->setVisible(true);
        }
    } else {
        auto adjRoomNames = _visibility.equal_range(leaderRoom->name());
        for (auto &room : _rooms) {
            // Room is visible if either of the following is true:
            // 1. party leader is not in a room
            // 2. this room is the party leaders room
            // 3. this room is adjacent to the party leaders room
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
}

void Area::update3rdPersonCameraTarget() {
    shared_ptr<SpatialObject> partyLeader(_game.party().getLeader());
    if (!partyLeader) {
        return;
    }
    glm::vec3 position(partyLeader->position());
    auto model = static_pointer_cast<ModelSceneNode>(partyLeader->sceneNode());
    if (!model) {
        return;
    }
    shared_ptr<ModelNode> cameraHook(model->model().getNodeByName("camerahook"));
    if (cameraHook) {
        position += glm::vec3(cameraHook->absoluteTransform()[3]);
    }
    _thirdPersonCamera->setTargetPosition(position);
}

void Area::updateVisibility() {
    if (_game.cameraType() != CameraType::ThirdPerson) {
        updateRoomVisibility();
    }
}

void Area::checkTriggersIntersection(const shared_ptr<SpatialObject> &triggerrer) {
    glm::vec2 position2d(triggerrer->position());

    for (auto &object : _objectsByType[ObjectType::Trigger]) {
        auto trigger = static_pointer_cast<Trigger>(object);
        if (trigger->getSquareDistanceTo(position2d) > kDefaultRaycastDistance2)
            continue;
        if (trigger->isTenant(triggerrer) || !trigger->isIn(position2d))
            continue;

        debug(boost::format("Trigger '%s' triggerred by '%s'") % trigger->tag() % triggerrer->tag());
        trigger->addTenant(triggerrer);

        if (!trigger->linkedToModule().empty()) {
            _game.scheduleModuleTransition(trigger->linkedToModule(), trigger->linkedTo());
            return;
        }
        if (!trigger->getOnEnter().empty()) {
            _game.scriptRunner().run(trigger->getOnEnter(), trigger->id(), triggerrer->id());
        }
    }
}

void Area::updateHeartbeat(float dt) {
    if (_heartbeatTimer.advance(dt)) {
        if (!_onHeartbeat.empty()) {
            _game.scriptRunner().run(_onHeartbeat, _id);
        }
        for (auto &object : _objects) {
            string heartbeat(object->getOnHeartbeat());
            if (!heartbeat.empty()) {
                _game.scriptRunner().run(heartbeat, object->id());
            }
        }
        _heartbeatTimer.setTimeout(kHeartbeatInterval);
    }
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
        PlaceableCamera &camera = static_cast<PlaceableCamera &>(*object);
        if (camera.cameraId() == cameraId) {
            _staticCamera->setObject(camera);
            break;
        }
    }
}

void Area::setThirdPartyCameraStyle(CameraStyleType type) {
    switch (type) {
    case CameraStyleType::Combat:
        _thirdPersonCamera->setStyle(_camStyleCombat);
        break;
    default:
        _thirdPersonCamera->setStyle(_camStyleDefault);
        break;
    }
}

void Area::setStealthXPEnabled(bool value) {
    _stealthXPEnabled = value;
}

void Area::setMaxStealthXP(int value) {
    _maxStealthXP = value;
}

void Area::setCurrentStealthXP(int value) {
    _currentStealthXP = value;
}

void Area::setStealthXPDecrement(int value) {
    _stealthXPDecrement = value;
}

void Area::setUnescapable(bool value) {
    _unescapable = value;
}

shared_ptr<Object> Area::createObject(ObjectType type, const string &blueprintResRef, const shared_ptr<Location> &location) {
    shared_ptr<Object> object;

    switch (type) {
    case ObjectType::Item: {
        auto item = _game.objectFactory().newItem();
        item->loadFromBlueprint(blueprintResRef);
        object = move(item);
        break;
    }
    case ObjectType::Creature: {
        auto creature = _game.objectFactory().newCreature();
        creature->loadFromBlueprint(blueprintResRef);
        creature->setPosition(location->position());
        creature->setFacing(location->facing());
        object = move(creature);
        break;
    }
    case ObjectType::Placeable: {
        auto placeable = _game.objectFactory().newPlaceable();
        placeable->loadFromBlueprint(blueprintResRef);
        object = move(placeable);
        break;
    }
    default:
        warn("Unsupported object type: " + to_string(static_cast<int>(type)));
        break;
    }
    if (!object)
        return nullptr;

    auto spatial = dynamic_pointer_cast<SpatialObject>(object);
    if (spatial) {
        add(spatial);
        auto creature = dynamic_pointer_cast<Creature>(spatial);
        if (creature) {
            creature->runSpawnScript();
        }
    }

    return move(object);
}

void Area::updateObjectSelection() {
    if (_hilightedObject && !_hilightedObject->isSelectable()) {
        _hilightedObject.reset();
    }
    if (_selectedObject && !_selectedObject->isSelectable()) {
        _selectedObject.reset();
    }
}

void Area::hilightObject(shared_ptr<SpatialObject> object) {
    _hilightedObject = move(object);
}

void Area::selectObject(shared_ptr<SpatialObject> object) {
    _selectedObject = move(object);
}

shared_ptr<SpatialObject> Area::getNearestObject(const glm::vec3 &origin, int nth, const std::function<bool(const std::shared_ptr<SpatialObject> &)> &predicate) {
    vector<pair<shared_ptr<SpatialObject>, float>> candidates;

    for (auto &object : _objects) {
        if (predicate(object)) {
            candidates.push_back(make_pair(object, object->getSquareDistanceTo(origin)));
        }
    }
    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) { return left.second < right.second; });

    int candidateCount = static_cast<int>(candidates.size());
    if (nth >= candidateCount) {
        debug(boost::format("getNearestObject: nth is out of bounds: %d/%d") % nth % candidateCount);
        return nullptr;
    }

    return candidates[nth].first;
}

shared_ptr<Creature> Area::getNearestCreature(const std::shared_ptr<SpatialObject> &target, const SearchCriteriaList &criterias, int nth) {
    vector<pair<shared_ptr<Creature>, float>> candidates;

    for (auto &object : getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(object);
        if (matchesCriterias(*creature, criterias, target)) {
            float distance2 = creature->getSquareDistanceTo(*target);
            candidates.push_back(make_pair(move(creature), distance2));
        }
    }

    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    return nth < candidates.size() ? candidates[nth].first : nullptr;
}

bool Area::matchesCriterias(const Creature &creature, const SearchCriteriaList &criterias, std::shared_ptr<SpatialObject> target) const {
    for (auto &criteria : criterias) {
        switch (criteria.first) {
        case CreatureType::Reputation: {
            auto reputation = static_cast<ReputationType>(criteria.second);
            switch (reputation) {
            case ReputationType::Friend:
                if (!target || !_services.reputes.getIsFriend(creature, *static_pointer_cast<Creature>(target)))
                    return false;
                break;
            case ReputationType::Enemy:
                if (!target || !_services.reputes.getIsEnemy(creature, *static_pointer_cast<Creature>(target)))
                    return false;
                break;
            case ReputationType::Neutral:
                if (!target || !_services.reputes.getIsNeutral(creature, *static_pointer_cast<Creature>(target)))
                    return false;
                break;
            default:
                break;
            }
            break;
        }
        case CreatureType::Perception: {
            if (!target)
                return false;

            bool seen = creature.perception().seen.count(target) > 0;
            bool heard = creature.perception().heard.count(target) > 0;
            bool matches = false;
            auto perception = static_cast<PerceptionType>(criteria.second);
            switch (perception) {
            case PerceptionType::SeenAndHeard:
                matches = seen && heard;
                break;
            case PerceptionType::NotSeenAndNotHeard:
                matches = !seen && !heard;
                break;
            case PerceptionType::HeardAndNotSeen:
                matches = heard && !seen;
                break;
            case PerceptionType::SeenAndNotHeard:
                matches = seen && !heard;
                break;
            case PerceptionType::NotHeard:
                matches = !heard;
                break;
            case PerceptionType::Heard:
                matches = heard;
                break;
            case PerceptionType::NotSeen:
                matches = !seen;
                break;
            case PerceptionType::Seen:
                matches = seen;
                break;
            default:
                break;
            }
            if (!matches)
                return false;
            break;
        }
        default:
            // TODO: implement other criterias
            break;
        }
    }

    return true;
}

shared_ptr<Creature> Area::getNearestCreatureToLocation(const Location &location, const SearchCriteriaList &criterias, int nth) {
    vector<pair<shared_ptr<Creature>, float>> candidates;

    for (auto &object : getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(object);
        if (matchesCriterias(*creature, criterias)) {
            float distance2 = creature->getSquareDistanceTo(location.position());
            candidates.push_back(make_pair(move(creature), distance2));
        }
    }

    std::sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    return nth < candidates.size() ? candidates[nth].first : nullptr;
}

void Area::updatePerception(float dt) {
    if (_perceptionTimer.advance(dt)) {
        doUpdatePerception();
        _perceptionTimer.setTimeout(kUpdatePerceptionInterval);
    }
}

void Area::doUpdatePerception() {
    // For each creature, determine a list of creatures it sees
    ObjectList &creatures = getObjectsByType(ObjectType::Creature);
    for (auto &object : creatures) {
        // Skip dead creatures
        if (object->isDead())
            continue;

        auto creature = static_pointer_cast<Creature>(object);
        float hearingRange2 = creature->perception().hearingRange * creature->perception().hearingRange;
        float sightRange2 = creature->perception().sightRange * creature->perception().sightRange;

        for (auto &other : creatures) {
            // Skip self
            if (other == object)
                continue;

            bool heard = false;
            bool seen = false;

            float distance2 = creature->getSquareDistanceTo(*other);
            if (distance2 <= hearingRange2) {
                heard = true;
            }
            if (distance2 <= sightRange2) {
                seen = isObjectSeen(*creature, *other);
            }

            // Hearing
            bool wasHeard = creature->perception().heard.count(other) > 0;
            if (!wasHeard && heard) {
                debug(boost::format("%s heard by %s") % other->tag() % creature->tag(), LogChannels::perception);
                creature->onObjectHeard(other);
            } else if (wasHeard && !heard) {
                debug(boost::format("%s inaudible to %s") % other->tag() % creature->tag(), LogChannels::perception);
                creature->onObjectInaudible(other);
            }

            // Sight
            bool wasSeen = creature->perception().seen.count(other) > 0;
            if (!wasSeen && seen) {
                debug(boost::format("%s seen by %s") % other->tag() % creature->tag(), LogChannels::perception);
                creature->onObjectSeen(other);
            } else if (wasSeen && !seen) {
                debug(boost::format("%s vanished from %s") % other->tag() % creature->tag(), LogChannels::perception);
                creature->onObjectVanished(other);
            }
        }
    }
}

SpatialObject *Area::getObjectAt(int x, int y) const {
    auto partyLeader = _game.party().getLeader();
    if (!partyLeader) {
        return nullptr;
    }
    auto &scene = _services.sceneGraphs.get(kSceneMain);
    auto model = scene.pickModelAt(x, y, partyLeader.get());
    if (!model) {
        return nullptr;
    }
    return dynamic_cast<SpatialObject *>(model->user());
}

} // namespace game

} // namespace reone
