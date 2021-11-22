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

#include "../../../common/exception/validation.h"
#include "../../../common/logutil.h"
#include "../../../common/randomutil.h"
#include "../../../common/streamutil.h"
#include "../../../graphics/barycentricutil.h"
#include "../../../graphics/mesh/mesh.h"
#include "../../../graphics/mesh/meshes.h"
#include "../../../graphics/model/models.h"
#include "../../../graphics/texture/textures.h"
#include "../../../graphics/triangleutil.h"
#include "../../../graphics/walkmesh/walkmesh.h"
#include "../../../graphics/walkmesh/walkmeshes.h"
#include "../../../resource/2da.h"
#include "../../../resource/2das.h"
#include "../../../resource/format/lytreader.h"
#include "../../../resource/format/visreader.h"
#include "../../../resource/gffs.h"
#include "../../../resource/resources.h"
#include "../../../resource/strings.h"
#include "../../../scene/graphs.h"
#include "../../../scene/node/grass.h"
#include "../../../scene/node/grasscluster.h"
#include "../../../scene/node/walkmesh.h"
#include "../../../scene/types.h"

#include "../../core/types.h"

#include "../game.h"
#include "../location.h"
#include "../party.h"
#include "../reputes.h"
#include "../room.h"
#include "../script/runner.h"
#include "../services.h"
#include "../surfaces.h"

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
static constexpr int kMaxSoundCount = 4;
static constexpr float kGrassDensityFactor = 0.25f;

static constexpr float kUpdatePerceptionInterval = 1.0f; // seconds

static constexpr float kElevationTestZ = 1024.0f;
static constexpr float kMaxCollisionDistance = 8.0f;
static constexpr float kMaxCollisionDistance2 = kMaxCollisionDistance * kMaxCollisionDistance;
static constexpr float kLineOfSightTestHeight = 1.7f; // TODO: make it appearance-based

static glm::vec3 g_defaultAmbientColor {0.2f};
static CameraStyle g_defaultCameraStyle {3.2f, 83.0f, 0.45f, 55.0f};

static bool g_debugPath = false;

Area::Area(
    uint32_t id,
    Game &game,
    Services &services) :
    Object(
        id,
        ObjectType::Area,
        game,
        services),
    _map(game, game.party(), services.context, services.meshes, services.shaders, services.textures, services.window) {

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

void Area::loadLYT() {
    shared_ptr<ByteArray> lytData(_services.resources.getRaw(_name, ResourceType::Lyt));
    if (!lytData) {
        throw ValidationException("Area LYT file not found");
    }
    LytReader lyt;
    lyt.load(wrap(lytData));

    auto &sceneGraph = _services.sceneGraphs.get(kSceneMain);

    for (auto &lytRoom : lyt.rooms()) {
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
        auto aabbNode = modelSceneNode->model().getAABBNode();
        if (_grass.texture && aabbNode) {
            glm::mat4 aabbTransform(glm::translate(aabbNode->absoluteTransform(), position));
            auto grassSceneNode = sceneGraph.newGrass(glm::vec2(_grass.quadSize), _grass.texture, aabbNode->mesh()->lightmap);
            for (auto &material : _services.surfaces.getGrassSurfaceIndices()) {
                for (auto &face : aabbNode->getFacesByMaterial(material)) {
                    vector<glm::vec3> vertices(aabbNode->mesh()->mesh->getTriangleCoords(face));
                    float triArea = calculateTriangleArea(vertices);
                    for (int i = 0; i < getNumGrassClusters(triArea); ++i) {
                        glm::vec3 baryPosition(getRandomBarycentric());
                        glm::vec3 position(aabbTransform * glm::vec4(barycentricToCartesian(vertices[0], vertices[1], vertices[2], baryPosition), 1.0f));
                        glm::vec2 lightmapUV(aabbNode->mesh()->mesh->getTriangleTexCoords2(face, baryPosition));
                        auto cluster = grassSceneNode->newCluster();
                        cluster->setPosition(move(position));
                        cluster->setVariant(getRandomGrassVariant());
                        cluster->setLightmapUV(move(lightmapUV));
                        grassSceneNode->addChild(move(cluster));
                    }
                }
            }
            sceneGraph.addRoot(move(grassSceneNode));
        }

        auto room = make_unique<Room>(lytRoom.name, position, move(modelSceneNode), move(walkmeshSceneNode));
        _rooms.insert(make_pair(room->name(), move(room)));
    }
}

void Area::loadVIS() {
    auto visData = _services.resources.getRaw(_name, ResourceType::Vis);
    if (!visData) {
        return;
    }

    VisReader vis;
    vis.load(wrap(visData));

    _visibility = fixVisibility(vis.visibility());
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
    shared_ptr<GffStruct> pth(_services.gffs.get(_name, ResourceType::Pth));
    if (!pth) {
        return;
    }

    Path path;
    path.load(*pth);

    const vector<Path::Point> &points = path.points();
    unordered_map<int, float> pointZ;

    for (size_t i = 0; i < points.size(); ++i) {
        const Path::Point &point = points[i];
        Room *room = nullptr;
        float z = 0.0f;
        int material = 0;

        if (!testElevationAt(glm::vec2(point.x, point.y), z, material, room)) {
            warn(boost::format("Point %d elevation not found") % i);
            continue;
        }
        pointZ.insert(make_pair(i, z));
    }

    _pathfinder.load(points, pointZ);
}

void Area::initCameras(const glm::vec3 &entryPosition, float entryFacing) {
    glm::vec3 position(entryPosition);
    position.z += 1.7f;

    auto &sceneGraph = _services.sceneGraphs.get(kSceneMain);

    _firstPersonCamera = make_unique<FirstPersonCamera>(_cameraAspect, glm::radians(kDefaultFieldOfView), sceneGraph);
    _firstPersonCamera->setPosition(position);
    _firstPersonCamera->setFacing(entryFacing);

    _thirdPersonCamera = make_unique<ThirdPersonCamera>(_cameraAspect, _camStyleDefault, _game, sceneGraph);
    _thirdPersonCamera->setFindObstacle(bind(&Area::getCameraObstacle, this, _1, _2, _3));
    _thirdPersonCamera->setTargetPosition(position);
    _thirdPersonCamera->setFacing(entryFacing);

    _dialogCamera = make_unique<DialogCamera>(_cameraAspect, _camStyleDefault, sceneGraph);
    _dialogCamera->setFindObstacle(bind(&Area::getCameraObstacle, this, _1, _2, _3));

    _animatedCamera = make_unique<AnimatedCamera>(_cameraAspect, sceneGraph);
    _staticCamera = make_unique<StaticCamera>(_cameraAspect, sceneGraph);
}

void Area::add(const shared_ptr<SpatialObject> &object) {
    _objects.push_back(object);
    _objectsByType[object->type()].push_back(object);
    _objectsByTag[object->tag()].push_back(object);

    determineObjectRoom(*object);

    auto &sceneGraph = _services.sceneGraphs.get(kSceneMain);
    auto model = object->sceneNode();
    if (model) {
        sceneGraph.addRoot(model);
    }
    if (object->type() == ObjectType::Placeable) {
        auto placeable = static_pointer_cast<Placeable>(object);
        auto walkmesh = placeable->walkmesh();
        if (walkmesh) {
            sceneGraph.addRoot(walkmesh);
        }
    } else if (object->type() == ObjectType::Door) {
        auto door = static_pointer_cast<Door>(object);
        auto walkmeshOpen1 = door->walkmeshOpen1();
        if (walkmeshOpen1) {
            sceneGraph.addRoot(walkmeshOpen1);
        }
        auto walkmeshOpen2 = door->walkmeshOpen2();
        if (walkmeshOpen2) {
            sceneGraph.addRoot(walkmeshOpen2);
        }
        auto walkmeshClosed = door->walkmeshClosed();
        if (walkmeshClosed) {
            sceneGraph.addRoot(walkmeshClosed);
        }
    }
}

void Area::determineObjectRoom(SpatialObject &object) {
    glm::vec3 position(object.position());
    Room *room = nullptr;
    int material = 0;

    if (testElevationAt(position, position.z, material, room)) {
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
    shared_ptr<SpatialObject> object(dynamic_pointer_cast<SpatialObject>(_game.objectFactory().getObjectById(objectId)));
    if (!object) {
        return;
    }
    auto room = object->room();
    if (room) {
        room->removeTenant(object.get());
    }

    auto &sceneGraph = _services.sceneGraphs.get(kSceneMain);
    auto sceneNode = object->sceneNode();
    if (sceneNode) {
        sceneGraph.removeRoot(sceneNode);
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
    glm::vec3 position(object.position());
    Room *room = nullptr;
    int material = 0;

    if (testElevationAt(position, position.z, material, room)) {
        object.setPosition(position);
        return;
    }
    for (int i = 0; i < 4; ++i) {
        float angle = i * glm::half_pi<float>();
        position = object.position() + glm::vec3(glm::sin(angle), glm::cos(angle), 0.0f);

        if (testElevationAt(position, position.z, material, room)) {
            object.setPosition(position);
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
    switch (event.keysym.scancode) {
    case SDL_SCANCODE_Q:
        selectNextObject(true);
        return true;

    case SDL_SCANCODE_E:
        selectNextObject();
        return true;

    case SDL_SCANCODE_SLASH: {
        if (_selectedObject) {
            printDebugInfo(*_selectedObject);
        }
        return true;
    }

    default:
        return false;
    }
}

void Area::printDebugInfo(const SpatialObject &object) {
    auto model = static_pointer_cast<ModelSceneNode>(object.sceneNode());

    ostringstream ss;
    ss << boost::format("tag='%s'") % object.tag();
    ss << boost::format(",pos=[%0.2f,%0.2f,%0.2f]") % object.position().x % object.position().y % object.position().z;
    ss << boost::format(",model='%s'") % model->model().name();

    debug("Selected object: " + ss.str());
}

void Area::update(float dt) {
    doDestroyObjects();
    updateVisibility();
    updateSounds();
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

    float facing = -glm::atan(dir.x, dir.y);
    creature->setFacing(facing);

    float speed = run ? creature->runSpeed() : creature->walkSpeed();
    float speedDt = speed * dt;

    glm::vec3 dest(creature->position());
    dest.x += dir.x * speedDt;
    dest.y += dir.y * speedDt;

    // If obstacle is found once, try moving along the face normal
    glm::vec3 normal;
    if (getCreatureObstacle(creature->position() + zOffset, dest + zOffset, normal)) {
        glm::vec3 creatureToDest(glm::normalize(dest - creature->position()));

        glm::vec3 right(glm::cross(up, normal));
        right *= glm::dot(creatureToDest, right);

        glm::vec2 dir2(glm::normalize(glm::vec2(right)));

        dest = creature->position();
        dest.x += dir2.x * speedDt;
        dest.y += dir2.y * speedDt;

        // If obstacle is found twice, abort movement
        if (getCreatureObstacle(creature->position() + zOffset, dest + zOffset, normal))
            return false;
    }

    return doMoveCreature(creature, dest);
}

bool Area::doMoveCreature(const shared_ptr<Creature> &creature, const glm::vec3 &dest) {
    float z;
    Room *room;
    int material;

    if (testElevationAt(dest, z, material, room)) {
        const Room *oldRoom = creature->room();

        creature->setRoom(room);
        creature->setPosition(glm::vec3(dest.x, dest.y, z));
        creature->setWalkmeshMaterial(material);

        if (creature == _game.party().getLeader()) {
            onPartyLeaderMoved(room != oldRoom);
        }

        checkTriggersIntersection(creature);

        return true;
    }

    return false;
}

bool Area::moveCreatureTowards(const shared_ptr<Creature> &creature, const glm::vec2 &dest, bool run, float dt) {
    glm::vec2 delta(dest - glm::vec2(creature->position()));
    glm::vec2 dir(glm::normalize(delta));
    return moveCreature(creature, dir, run, dt);
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

int Area::getNumGrassClusters(float triArea) const {
    return static_cast<int>(glm::round(kGrassDensityFactor * _grass.density * triArea));
}

int Area::getRandomGrassVariant() const {
    float sum = _grass.probabilities[0] + _grass.probabilities[1] + _grass.probabilities[2] + _grass.probabilities[3];
    float val = random(0.0f, 1.0f) * sum;
    float upper = 0.0f;

    for (int i = 0; i < 3; ++i) {
        upper += _grass.probabilities[i];
        if (val < upper)
            return i;
    }

    return 3;
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
    selectNearestObject();
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

void Area::updateSounds() {
    glm::vec3 refPosition;
    if (_game.cameraType() == CameraType::ThirdPerson && _game.party().getLeader()) {
        refPosition = _game.party().getLeader()->position();
    } else {
        refPosition = _game.getActiveCamera()->sceneNode()->absoluteTransform()[3];
    }

    vector<pair<Sound *, float>> soundDistances;

    for (auto &sound : _objectsByType[ObjectType::Sound]) {
        Sound *soundPtr = static_cast<Sound *>(sound.get());
        soundPtr->setAudible(false);

        if (!soundPtr->isActive())
            continue;

        float maxDist2 = soundPtr->maxDistance();
        maxDist2 *= maxDist2;

        float dist2 = soundPtr->getDistanceTo2(refPosition);
        if (dist2 > maxDist2)
            continue;

        soundDistances.push_back(make_pair(soundPtr, dist2));
    }

    sort(soundDistances.begin(), soundDistances.end(), [](auto &left, auto &right) {
        int leftPriority = left.first->priority();
        int rightPriority = right.first->priority();

        if (leftPriority < rightPriority)
            return true;
        if (leftPriority > rightPriority)
            return false;

        return left.second < right.second;
    });
    if (soundDistances.size() > kMaxSoundCount) {
        soundDistances.erase(soundDistances.begin() + kMaxSoundCount, soundDistances.end());
    }
    for (auto &sound : soundDistances) {
        sound.first->setAudible(true);
    }
}

void Area::checkTriggersIntersection(const shared_ptr<SpatialObject> &triggerrer) {
    glm::vec2 position2d(triggerrer->position());

    for (auto &object : _objectsByType[ObjectType::Trigger]) {
        auto trigger = static_pointer_cast<Trigger>(object);
        if (trigger->getDistanceTo2(position2d) > kDefaultRaycastDistance * kDefaultRaycastDistance)
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

void Area::selectNextObject(bool reverse) {
    vector<shared_ptr<SpatialObject>> selectables(getSelectableObjects());

    if (selectables.empty()) {
        _selectedObject.reset();
        return;
    }
    if (!_selectedObject) {
        _selectedObject = selectables.front();
        return;
    }
    if (reverse) {
        auto selected = std::find(selectables.rbegin(), selectables.rend(), _selectedObject);
        if (selected != selectables.rend()) {
            selected++;
        }
        _selectedObject = selected != selectables.rend() ? *selected : selectables.back();

    } else {
        auto selected = std::find(selectables.begin(), selectables.end(), _selectedObject);
        if (selected != selectables.end()) {
            selected++;
        }
        _selectedObject = selected != selectables.end() ? *selected : selectables.front();
    }
}

vector<shared_ptr<SpatialObject>> Area::getSelectableObjects() const {
    vector<shared_ptr<SpatialObject>> result;
    vector<pair<shared_ptr<SpatialObject>, float>> distances;

    shared_ptr<SpatialObject> partyLeader(_game.party().getLeader());
    glm::vec3 origin(partyLeader->position());

    for (auto &object : objects()) {
        if (!object->isSelectable() || object.get() == partyLeader.get())
            continue;

        auto model = static_pointer_cast<ModelSceneNode>(object->sceneNode());
        if (!model || !model->isVisible())
            continue;

        float dist2 = object->getDistanceTo2(origin);
        if (dist2 > kSelectionDistance * kSelectionDistance)
            continue;

        distances.push_back(make_pair(object, dist2));
    }

    sort(distances.begin(), distances.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });
    for (auto &pair : distances) {
        result.push_back(pair.first);
    }

    return move(result);
}

void Area::selectNearestObject() {
    _selectedObject.reset();
    selectNextObject();
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
            candidates.push_back(make_pair(object, object->getDistanceTo2(origin)));
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
            float distance2 = creature->getDistanceTo2(*target);
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
            float distance2 = creature->getDistanceTo2(location.position());
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

            float distance2 = creature->getDistanceTo2(*other);
            if (distance2 <= hearingRange2) {
                heard = true;
            }
            if (distance2 <= sightRange2) {
                seen = isInLineOfSight(*creature, *other);
            }

            // Hearing
            bool wasHeard = creature->perception().heard.count(other) > 0;
            if (!wasHeard && heard) {
                debug(boost::format("%s heard by %s") % other->tag() % creature->tag());
                creature->onObjectHeard(other);
            } else if (wasHeard && !heard) {
                debug(boost::format("%s inaudible to %s") % other->tag() % creature->tag());
                creature->onObjectInaudible(other);
            }

            // Sight
            bool wasSeen = creature->perception().seen.count(other) > 0;
            if (!wasSeen && seen) {
                debug(boost::format("%s seen by %s") % other->tag() % creature->tag());
                creature->onObjectSeen(other);
            } else if (wasSeen && !seen) {
                debug(boost::format("%s vanished from %s") % other->tag() % creature->tag());
                creature->onObjectVanished(other);
            }
        }
    }
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
        shared_ptr<Creature> creature(_game.objectFactory().newCreature());
        creature->loadFromGIT(*gffs);
        landObject(*creature);
        add(creature);
    }
}

void Area::loadDoors(const GffStruct &git) {
    for (auto &gffs : git.getList("Door List")) {
        shared_ptr<Door> door(_game.objectFactory().newDoor());
        door->loadFromGIT(*gffs);
        add(door);
    }
}

void Area::loadPlaceables(const GffStruct &git) {
    for (auto &gffs : git.getList("Placeable List")) {
        shared_ptr<Placeable> placeable(_game.objectFactory().newPlaceable());
        placeable->loadFromGIT(*gffs);
        add(placeable);
    }
}

void Area::loadWaypoints(const GffStruct &git) {
    for (auto &gffs : git.getList("WaypointList")) {
        shared_ptr<Waypoint> waypoint(_game.objectFactory().newWaypoint());
        waypoint->loadFromGIT(*gffs);
        add(waypoint);
    }
}

void Area::loadTriggers(const GffStruct &git) {
    for (auto &gffs : git.getList("TriggerList")) {
        shared_ptr<Trigger> trigger(_game.objectFactory().newTrigger());
        trigger->loadFromGIT(*gffs);
        add(trigger);
    }
}

void Area::loadSounds(const GffStruct &git) {
    for (auto &gffs : git.getList("SoundList")) {
        shared_ptr<Sound> sound(_game.objectFactory().newSound());
        sound->loadFromGIT(*gffs);
        add(sound);
    }
}

void Area::loadCameras(const GffStruct &git) {
    for (auto &gffs : git.getList("CameraList")) {
        shared_ptr<PlaceableCamera> camera(_game.objectFactory().newCamera());
        camera->loadFromGIT(*gffs);
        add(camera);
    }
}

void Area::loadEncounters(const GffStruct &git) {
    for (auto &gffs : git.getList("Encounter List")) {
        shared_ptr<Encounter> encounter(_game.objectFactory().newEncounter());
        encounter->loadFromGIT(*gffs);
        add(encounter);
    }
}

bool Area::testElevationAt(const glm::vec2 &point, float &z, int &material, Room *&room) const {
    static glm::vec3 down(0.0f, 0.0f, -1.0f);

    auto walkcheckSurfaces = _services.surfaces.getWalkcheckSurfaceIndices();

    /*
    // Test object walkmeshes
    for (auto &o : _objects) {
        // Object must have a valid model and walkmesh
        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        shared_ptr<Walkmesh> walkmesh(o->getWalkmesh());
        if (!model || !walkmesh) {
            continue;
        }

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(point) > kMaxCollisionDistance2) {
            continue;
        }

        glm::vec2 objSpacePos(model->absoluteTransformInverse() * glm::vec4(point, 0.0f, 1.0f));
        glm::vec3 origin(objSpacePos, kElevationTestZ);
        float distance = 0.0f;
        auto face = walkmesh->raycast(walkcheckSurfaces, origin, down, 2.0f * kElevationTestZ, distance);
        if (face) {
            return false;
        }
    }
    */

    // Test room walkmeshes
    for (auto &r : _rooms) {
        // Room must have a valid model and walkmesh
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<WalkmeshSceneNode> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) {
            continue;
        }

        glm::vec3 origin(point, kElevationTestZ);
        float distance;
        auto face = walkmesh->walkmesh().raycast(walkcheckSurfaces, origin, down, 2.0f * kElevationTestZ, distance);
        if (face) {
            if (!face->walkable) {
                return false;
            }
            z = kElevationTestZ - distance;
            material = face->material;
            room = r.second.get();
            return true;
        }
    }

    return false;
}

shared_ptr<SpatialObject> Area::getObjectAt(int x, int y) const {
    shared_ptr<CameraSceneNode> camera(_services.sceneGraphs.get(kSceneMain).activeCamera());
    shared_ptr<Creature> partyLeader(_game.party().getLeader());
    if (!camera || !partyLeader) {
        return nullptr;
    }

    const GraphicsOptions &opts = _game.options().graphics;
    glm::vec4 viewport(0.0f, 0.0f, opts.width, opts.height);
    glm::vec3 start(glm::unProject(glm::vec3(x, opts.height - y, 0.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 end(glm::unProject(glm::vec3(x, opts.height - y, 1.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 dir(glm::normalize(end - start));

    // Calculate distances to all selectable objects, return the closest object
    vector<pair<shared_ptr<SpatialObject>, float>> distances;
    for (auto &o : _objects) {
        // Skip non-selectable objects and party leader
        if (!o->isSelectable() || o == partyLeader)
            continue;

        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        if (!model)
            continue;

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(start) > kMaxCollisionDistance2)
            continue;

        // Test object AABB (object space)
        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, kMaxCollisionDistance, distance)) {
            distances.push_back(make_pair(o, distance));
        }
    }
    if (distances.empty())
        return nullptr;
    std::sort(distances.begin(), distances.end(), [](auto &left, auto &right) { return left.second < right.second; });

    return distances[0].first;
}

bool Area::getCameraObstacle(const glm::vec3 &start, const glm::vec3 &end, glm::vec3 &intersection) const {
    glm::vec3 endToStart(end - start);
    glm::vec3 dir(glm::normalize(endToStart));
    float minDistance = numeric_limits<float>::max();
    float maxDistance = glm::length(endToStart);

    // Test AABB of door objects
    for (auto &o : _objects) {
        if (o->type() != ObjectType::Door) {
            continue;
        }

        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        if (!model) {
            continue;
        }

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(start) > kMaxCollisionDistance2) {
            continue;
        }

        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, maxDistance, distance) && distance < minDistance) {
            minDistance = distance;
        }
    }

    // Test room walkmeshes
    auto walkcheckSurfaces = _services.surfaces.getWalkcheckSurfaceIndices();
    for (auto &r : _rooms) {
        // Room must have a valid model and walkmesh
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<WalkmeshSceneNode> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) {
            continue;
        }

        float distance;
        auto face = walkmesh->walkmesh().raycast(walkcheckSurfaces, start, dir, maxDistance, distance);
        if (face && distance < minDistance) {
            minDistance = distance;
        }
    }

    if (minDistance != numeric_limits<float>::max()) {
        intersection = start + minDistance * dir;
        return true;
    }

    return false;
}

bool Area::getCreatureObstacle(const glm::vec3 &start, const glm::vec3 &end, glm::vec3 &normal) const {
    if (end == start) {
        return false;
    }
    auto walkcheckSurfaces = _services.surfaces.getWalkcheckSurfaceIndices();
    glm::vec3 endToStart(end - start);
    glm::vec3 dir(glm::normalize(endToStart));
    float minDistance = numeric_limits<float>::max();
    float maxDistance = glm::length(endToStart);

    // Test room walkmeshes
    for (auto &r : _rooms) {
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<WalkmeshSceneNode> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) {
            continue;
        }

        float distance;
        auto face = walkmesh->walkmesh().raycast(walkcheckSurfaces, start, dir, maxDistance, distance);
        if (face && distance < minDistance) {
            minDistance = distance;
            normal = face->normal;
        }
    }

    return minDistance != numeric_limits<float>::max();
}

bool Area::isInLineOfSight(const Creature &subject, const SpatialObject &target) const {
    static glm::vec3 offsetZ {0.0f, 0.0f, kLineOfSightTestHeight};

    glm::vec3 start(subject.position() + offsetZ);
    glm::vec3 end(target.position() + offsetZ);
    glm::vec3 startToEnd(end - start);
    glm::vec3 dir(glm::normalize(startToEnd));
    float maxDistance = glm::length(startToEnd);

    for (auto &o : _objects) {
        if (o->type() != ObjectType::Door) {
            continue;
        }

        auto model = static_pointer_cast<ModelSceneNode>(o->sceneNode());
        if (!model) {
            continue;
        }

        // Distance to object must not exceed maximum collision distance
        if (o->getDistanceTo2(start) > kMaxCollisionDistance2)
            continue;

        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, maxDistance, distance)) {
            return false;
        }
    }

    // Test room walkmeshes
    auto walkcheckSurfaces = _services.surfaces.getWalkcheckSurfaceIndices();
    for (auto &r : _rooms) {
        shared_ptr<ModelSceneNode> model(r.second->model());
        shared_ptr<WalkmeshSceneNode> walkmesh(r.second->walkmesh());
        if (!model || !walkmesh) {
            continue;
        }

        // Start or end of path must be inside room AABB
        glm::vec2 roomSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec2 roomSpaceEnd(model->absoluteTransformInverse() * glm::vec4(end, 1.0f));
        if (!model->aabb().contains(roomSpaceStart) && !model->aabb().contains(roomSpaceEnd)) {
            continue;
        }

        float distance;
        auto face = walkmesh->walkmesh().raycast(walkcheckSurfaces, start, dir, maxDistance, distance);
        if (face) {
            return false;
        }
    }

    return true;
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
    shared_ptr<TwoDA> cameraStyles(_services.twoDas.get("camerastyle"));
    if (!cameraStyles) {
        _camStyleDefault = g_defaultCameraStyle;
        _camStyleCombat = g_defaultCameraStyle;
        return;
    }

    int areaStyleIdx = are.getInt("CameraStyle");
    _camStyleDefault.load(*cameraStyles, areaStyleIdx);

    int combatStyleIdx = cameraStyles->indexByCellValue("name", "Combat");
    if (combatStyleIdx != -1) {
        _camStyleCombat.load(*cameraStyles, combatStyleIdx);
    }
}

void Area::loadAmbientColor(const GffStruct &are) {
    _ambientColor = are.getColor("DynAmbientColor", g_defaultAmbientColor);

    auto &sceneGraph = _services.sceneGraphs.get(kSceneMain);
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
    _map.load(_name, *mapStruct);
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

    auto &sceneGraph = _services.sceneGraphs.get(kSceneMain);
    sceneGraph.setFogEnabled(_fogEnabled);
    sceneGraph.setFogNear(_fogNear);
    sceneGraph.setFogFar(_fogFar);
    sceneGraph.setFogColor(_fogColor);
}

} // namespace game

} // namespace reone
