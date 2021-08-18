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

#include "../../common/guardutil.h"
#include "../../common/log.h"
#include "../../common/random.h"
#include "../../common/streamutil.h"
#include "../../graphics/baryutil.h"
#include "../../graphics/mesh/mesh.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/model/models.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/walkmesh/walkmeshes.h"
#include "../../resource/format/lytreader.h"
#include "../../resource/format/visreader.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"
#include "../../scene/node/grass.h"
#include "../../scene/types.h"

#include "../game.h"
#include "../location.h"
#include "../objectconverter.h"
#include "../room.h"
#include "../surfaces.h"

#include "objectfactory.h"

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

static bool g_debugPath = false;

Area::Area(uint32_t id, Game *game) :
    Object(id, ObjectType::Area, game),
    _map(game) {

    init();

    _heartbeatTimer.setTimeout(kHeartbeatInterval);
}

void Area::init() {
    const GraphicsOptions &opts = _game->options().graphics;
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

    loadLYT();
    loadVIS();
    loadPTH();
    loadARE(are);
    loadGIT(git);
}

void Area::loadLYT() {
    LytReader lyt;
    lyt.load(wrap(_game->services().resource().resources().getRaw(_name, ResourceType::Lyt)));

    for (auto &lytRoom : lyt.rooms()) {
        shared_ptr<Model> model(_game->services().graphics().models().get(lytRoom.name));
        if (!model) continue;

        glm::vec3 position(lytRoom.position.x, lytRoom.position.y, lytRoom.position.z);

        auto sceneNode = make_shared<ModelSceneNode>(model, ModelUsage::Room, &_game->services().scene().graph());
        sceneNode->setLocalTransform(glm::translate(glm::mat4(1.0f), position));
        for (auto &anim : model->getAnimationNames()) {
            if (boost::starts_with(anim, "animloop")) {
                sceneNode->playAnimation(anim, AnimationProperties::fromFlags(AnimationFlags::loopOverlay));
            }
        }

        shared_ptr<Walkmesh> walkmesh(_game->services().graphics().walkmeshes().get(lytRoom.name, ResourceType::Wok));

        auto room = make_unique<Room>(lytRoom.name, position, sceneNode, walkmesh);
        _rooms.insert(make_pair(room->name(), move(room)));
    }
}

void Area::loadVIS() {
    VisReader vis;
    vis.load(wrap(_game->services().resource().resources().getRaw(_name, ResourceType::Vis)));

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
    shared_ptr<GffStruct> pth(_game->services().resource().resources().getGFF(_name, ResourceType::Pth));
    if (!pth) return;

    Path path;
    path.load(*pth);

    const vector<Path::Point> &points = path.points();
    unordered_map<int, float> pointZ;

    for (int i = 0; i < points.size(); ++i) {
        const Path::Point &point = points[i];
        Room *room = nullptr;
        float z = 0.0f;
        int material = 0;

        if (!testElevationAt(glm::vec2(point.x, point.y), z, material, room)) {
            warn(boost::format("Area: point %d elevation not found") % i);
            continue;
        }
        pointZ.insert(make_pair(i, z));
    }

    _pathfinder.load(points, pointZ);
}

void Area::initCameras(const glm::vec3 &entryPosition, float entryFacing) {
    glm::vec3 position(entryPosition);
    position.z += 1.7f;

    SceneGraph *sceneGraph = &_game->services().scene().graph();

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
    shared_ptr<SpatialObject> object(dynamic_pointer_cast<SpatialObject>(_game->services().objectFactory().getObjectById(objectId)));
    if (!object) return;
    {
        Room *room = object->room();
        if (room) {
            room->removeTenant(object.get());
        }
    }
    {
        auto sceneNode = object->sceneNode();
        if (sceneNode) {
            _game->services().scene().graph().removeRoot(sceneNode);
        }
    }
    {
        auto maybeObject = find_if(_objects.begin(), _objects.end(), [&object](auto &o) { return o.get() == object.get(); });
        if (maybeObject != _objects.end()) {
            _objects.erase(maybeObject);
        }
    }
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

ObjectList &Area::getObjectsByType(ObjectType type) {
    return _objectsByType.find(type)->second;
}

shared_ptr<SpatialObject> Area::getObjectByTag(const string &tag, int nth) const {
    auto objects = _objectsByTag.find(tag);
    if (objects == _objectsByTag.end()) return nullptr;
    if (nth >= objects->second.size()) return nullptr;

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
    Party &party = _game->services().party();

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
    Party &party = _game->services().party();

    for (int i = 0; i < party.getSize(); ++i) {
        doDestroyObject(party.getMember(i)->id());
    }
}

void Area::reloadParty() {
    shared_ptr<Creature> player(_game->services().party().player());
    loadParty(player->position(), player->getFacing());

    fill(_game->services().scene().graph());
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
    ss << boost::format(",model='%s'") % model->model()->name();

    debug("Selected object: " + ss.str());
}

void Area::update(float dt) {
    doDestroyObjects();
    updateVisibility();
    updateSounds();
    updateObjectSelection();

    if (!_game->isPaused()) {
        Object::update(dt);

        for (auto &room : _rooms) {
            room.second->update(dt);
        }
        for (auto &object : _objects) {
            object->update(dt);
        }

        updatePerception(dt);
        updateHeartbeat(dt);
    }
}

bool Area::moveCreature(const shared_ptr<Creature> &creature, const glm::vec2 &dir, bool run, float dt) {
    static glm::vec3 up { 0.0f, 0.0f, 1.0f };
    static glm::vec3 zOffset { 0.0f, 0.0f, 0.1f };

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
        if (getCreatureObstacle(creature->position() + zOffset, dest + zOffset, normal)) return false;
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

        if (creature == _game->services().party().getLeader()) {
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
    if (_onEnter.empty()) return;

    auto player = _game->services().party().player();
    if (!player) return;

    _game->services().scriptRunner().run(_onEnter, _id, player->id());
}

void Area::runOnExitScript() {
    if (_onExit.empty()) return;

    auto player = _game->services().party().player();
    if (!player) return;

    _game->services().scriptRunner().run(_onExit, _id, player->id());
}

void Area::destroyObject(const SpatialObject &object) {
    _objectsToDestroy.insert(object.id());
}

static inline float calculateTriangleArea(const vector<glm::vec3> &verts) {
    // Adapted from https://www.omnicalculator.com/math/herons-formula
    float a = glm::distance(verts[0], verts[1]);
    float b = glm::distance(verts[0], verts[2]);
    float c = glm::distance(verts[1], verts[2]);
    return 0.25f * glm::sqrt((a + b + c) * (-a + b + c) * (a - b + c) * (a + b - c));
}

/**
 * @return random barycentric position
 */
static inline glm::vec3 getRandomBarycentric() {
    // Adapted from https://math.stackexchange.com/q/18686
    float r1sqrt = glm::sqrt(random(0.0f, 1.0f));
    float r2 = random(0.0f, 1.0f);
    return glm::vec3(1.0f - r1sqrt, r1sqrt * (1.0f - r2), r2 * r1sqrt);
}

void Area::fill(SceneGraph &sceneGraph) {
    sceneGraph.clearRoots();

    // Area properties

    sceneGraph.setAmbientLightColor(_ambientColor);
    sceneGraph.setFogEnabled(_fogEnabled);
    sceneGraph.setFogNear(_fogNear);
    sceneGraph.setFogFar(_fogFar);
    sceneGraph.setFogColor(_fogColor);

    // Room models

    for (auto &room : _rooms) {
        shared_ptr<ModelSceneNode> sceneNode(room.second->model());
        if (sceneNode) {
            sceneGraph.addRoot(sceneNode);
        }
        shared_ptr<ModelNode> aabbNode(sceneNode->model()->getAABBNode());
        if (aabbNode && _grass.texture) {
            glm::mat4 aabbTransform(glm::translate(aabbNode->absoluteTransform(), room.second->position()));
            auto grass = make_shared<GrassSceneNode>(room.first, glm::vec2(_grass.quadSize), _grass.texture, aabbNode->mesh()->lightmap, &sceneGraph);
            for (auto &material : _game->services().surfaces().getGrassSurfaceIndices()) {
                for (auto &face : aabbNode->getFacesByMaterial(material)) {
                    vector<glm::vec3> vertices(aabbNode->mesh()->mesh->getTriangleCoords(face));
                    float area = calculateTriangleArea(vertices);
                    for (int i = 0; i < getNumGrassClusters(area); ++i) {
                        glm::vec3 baryPosition(getRandomBarycentric());
                        glm::vec3 position(aabbTransform * glm::vec4(barycentricToCartesian(vertices[0], vertices[1], vertices[2], baryPosition), 1.0f));
                        glm::vec2 lightmapUV(aabbNode->mesh()->mesh->getTriangleTexCoords2(face, baryPosition));
                        auto cluster = make_shared<GrassSceneNode::Cluster>();
                        cluster->parent = grass.get();
                        cluster->position = move(position);
                        cluster->variant = getRandomGrassVariant();
                        cluster->lightmapUV = move(lightmapUV);
                        grass->addCluster(move(cluster));
                    }
                }
            }
            sceneGraph.addRoot(grass);
        }
    }

    // Objects

    for (auto &object : _objects) {
        shared_ptr<SceneNode> sceneNode(object->sceneNode());
        if (sceneNode) {
            sceneGraph.addRoot(sceneNode);
        }
    }
}

int Area::getNumGrassClusters(float area) const {
    return static_cast<int>(glm::round(kGrassDensityFactor * _grass.density * area));
}

int Area::getRandomGrassVariant() const {
    float sum = _grass.probabilities[0] + _grass.probabilities[1] + _grass.probabilities[2] + _grass.probabilities[3];
    float val = random(0.0f, 1.0f) * sum;
    float upper = 0.0f;

    for (int i = 0; i < 3; ++i) {
        upper += _grass.probabilities[i];
        if (val < upper) return i;
    }

    return 3;
}

glm::vec3 Area::getSelectableScreenCoords(const shared_ptr<SpatialObject> &object, const glm::mat4 &projection, const glm::mat4 &view) const {
    static glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);

    glm::vec3 position(object->getSelectablePosition());

    return glm::project(position, view, projection, viewport);
}

void Area::update3rdPersonCameraFacing() {
    shared_ptr<SpatialObject> partyLeader(_game->services().party().getLeader());
    if (!partyLeader) return;

    _thirdPersonCamera->setFacing(partyLeader->getFacing());
}

void Area::startDialog(const shared_ptr<SpatialObject> &object, const string &resRef) {
    string finalResRef(resRef);
    if (resRef.empty()) finalResRef = object->conversation();
    if (resRef.empty()) return;

    _game->startDialog(object, finalResRef);
}

void Area::onPartyLeaderMoved(bool roomChanged) {
    shared_ptr<Creature> partyLeader(_game->services().party().getLeader());
    if (!partyLeader) return;

    if (roomChanged) {
        updateRoomVisibility();
    }
    update3rdPersonCameraTarget();
    selectNearestObject();
}

void Area::updateRoomVisibility() {
    shared_ptr<Creature> partyLeader(_game->services().party().getLeader());
    Room *leaderRoom = partyLeader ? partyLeader->room() : nullptr;
    bool allVisible = _game->cameraType() != CameraType::ThirdPerson || !leaderRoom;

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
    shared_ptr<SpatialObject> partyLeader(_game->services().party().getLeader());
    if (!partyLeader) return;

    glm::vec3 position(partyLeader->position());

    auto model = static_pointer_cast<ModelSceneNode>(partyLeader->sceneNode());
    shared_ptr<ModelNode> cameraHook(model->model()->getNodeByName("camerahook"));
    if (cameraHook) {
        position += glm::vec3(cameraHook->absoluteTransform()[3]);
    }

    _thirdPersonCamera->setTargetPosition(position);
}

void Area::updateVisibility() {
    if (_game->cameraType() != CameraType::ThirdPerson) {
        updateRoomVisibility();
    }
}

void Area::updateSounds() {
    glm::vec3 refPosition;
    if (_game->cameraType() == CameraType::ThirdPerson) {
        refPosition = _game->services().party().getLeader()->position();
    } else {
        refPosition = _game->getActiveCamera()->sceneNode()->absoluteTransform()[3];
    }

    vector<pair<Sound *, float>> soundDistances;

    for (auto &sound : _objectsByType[ObjectType::Sound]) {
        Sound *soundPtr = static_cast<Sound *>(sound.get());
        soundPtr->setAudible(false);

        if (!soundPtr->isActive()) continue;

        float maxDist2 = soundPtr->maxDistance();
        maxDist2 *= maxDist2;

        float dist2 = soundPtr->getDistanceTo2(refPosition);
        if (dist2 > maxDist2) continue;

        soundDistances.push_back(make_pair(soundPtr, dist2));
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

void Area::checkTriggersIntersection(const shared_ptr<SpatialObject> &triggerrer) {
    glm::vec2 position2d(triggerrer->position());

    for (auto &object : _objectsByType[ObjectType::Trigger]) {
        auto trigger = static_pointer_cast<Trigger>(object);
        if (trigger->getDistanceTo2(position2d) > kDefaultRaycastDistance * kDefaultRaycastDistance) continue;
        if (trigger->isTenant(triggerrer) || !trigger->isIn(position2d)) continue;

        debug(boost::format("Area: trigger '%s' triggerred by '%s'") % trigger->tag() % triggerrer->tag());
        trigger->addTenant(triggerrer);

        if (!trigger->linkedToModule().empty()) {
            _game->scheduleModuleTransition(trigger->linkedToModule(), trigger->linkedTo());
            return;
        }
        if (!trigger->getOnEnter().empty()) {
            _game->services().scriptRunner().run(trigger->getOnEnter(), trigger->id(), triggerrer->id());
        }
    }
}

void Area::updateHeartbeat(float dt) {
    if (_heartbeatTimer.advance(dt)) {
        if (!_onHeartbeat.empty()) {
            _game->services().scriptRunner().run(_onHeartbeat, _id);
        }
        for (auto &object : _objects) {
            string heartbeat(object->getOnHeartbeat());
            if (!heartbeat.empty()) {
                _game->services().scriptRunner().run(heartbeat, object->id());
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
    ensureNotNull(location, "location");

    shared_ptr<Object> object;

    switch (type) {
        case ObjectType::Item: {
            auto item = _game->services().objectFactory().newItem();
            item->loadFromBlueprint(blueprintResRef);
            object = move(item);
            break;
        }
        case ObjectType::Creature: {
            auto creature = _game->services().objectFactory().newCreature();
            creature->loadFromBlueprint(blueprintResRef);
            creature->setPosition(location->position());
            creature->setFacing(location->facing());
            object = move(creature);
            break;
        }
        case ObjectType::Placeable: {
            auto placeable = _game->services().objectFactory().newPlaceable();
            placeable->loadFromBlueprint(blueprintResRef);
            object = move(placeable);
            break;
        }
        default:
            warn("Area: createObject: unsupported object type: " + to_string(static_cast<int>(type)));
            break;
    }
    if (!object) return nullptr;

    auto spatial = dynamic_pointer_cast<SpatialObject>(object);
    if (spatial) {
        add(spatial);
        auto model = spatial->sceneNode();
        if (model) {
            _game->services().scene().graph().addRoot(model);
        }
        auto creature = ObjectConverter::toCreature(spatial);
        if (creature) {
            creature->runSpawnScript();
        }
    }

    return move(object);
}

} // namespace game

} // namespace reone
