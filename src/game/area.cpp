#include "area.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

#include "SDL2/SDL.h"

#include "../core/jobs.h"
#include "../core/log.h"
#include "../core/streamutil.h"
#include "../render/mesh/aabb.h"
#include "../resources/lytfile.h"
#include "../resources/manager.h"
#include "../resources/visfile.h"

#include "object/door.h"
#include "object/placeable.h"
#include "object/trigger.h"
#include "object/waypoint.h"

using namespace reone::gui;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

static const float kDrawDebugDistance = 64.0f;
static const float kPartyMemberFollowDistance = 4.0f;

static const float kMaxDistanceToTestCollision = 64.0f;
static const float kElevationTestOffset = 0.1f;
static const float kElevationTestDistance = 1.0f;
static const float kKeepPathDuration = 1000.0f;

static const int kCarthAppearance = 6;
static const int kBastilaAppearance = 4;
static const int kAttonAppearance = 452;
static const int kKreiaAppearance = 455;

static const char kPartyLeaderTag[] = "party-leader";
static const char kPartyMember1Tag[] = "party-member-1";
static const char kPartyMember2Tag[] = "party-member-2";

Area::Area(resources::GameVersion version, const std::string &name) : _version(version), _name(name), _navMesh(new NavMesh()) {
    _objects.insert(std::make_pair(ObjectType::Creature, ObjectList()));
    _objects.insert(std::make_pair(ObjectType::Door, ObjectList()));
    _objects.insert(std::make_pair(ObjectType::Placeable, ObjectList()));
    _objects.insert(std::make_pair(ObjectType::Waypoint, ObjectList()));
    _objects.insert(std::make_pair(ObjectType::Trigger, ObjectList()));

    _renderLists.insert(std::make_pair(RenderListName::Opaque, RenderList()));
    _renderLists.insert(std::make_pair(RenderListName::Transparent, RenderList()));
}

void Area::load(const GffStruct &are, const GffStruct &git) {
    loadProperties(git.getStruct("AreaProperties"));
    loadVisibility();
    loadLayout();
    loadCameraStyle(are);

    for (auto &gffs : git.getList("Creature List")) {
        std::shared_ptr<Creature> creature(makeCreature());
        creature->load(gffs);
        landObject(*creature);
        _objects[ObjectType::Creature].push_back(std::move(creature));
    }
    for (auto &gffs : git.getList("Door List")) {
        std::shared_ptr<Door> door(makeDoor());
        door->load(gffs);
        _objects[ObjectType::Door].push_back(std::move(door));
    }
    for (auto &gffs : git.getList("Placeable List")) {
        std::shared_ptr<Placeable> placeable(makePlaceable());
        placeable->load(gffs);
        if (placeable->walkmesh()) {
            _navMesh->add(placeable->walkmesh(), placeable->transform());
        }
        _objects[ObjectType::Placeable].push_back(std::move(placeable));
    }
    for (auto &gffs : git.getList("WaypointList")) {
        std::shared_ptr<Waypoint> waypoint(makeWaypoint());
        waypoint->load(gffs);
        _objects[ObjectType::Waypoint].push_back(std::move(waypoint));
    }
    for (auto &gffs : git.getList("TriggerList")) {
        std::shared_ptr<Trigger> trigger(makeTrigger());
        trigger->load(gffs);
        _objects[ObjectType::Trigger].push_back(std::move(trigger));
    }

    JobExecutor::instance().enqueue([&](const std::atomic_bool &cancel) {
        info("Computing navigation mesh");
        _navMesh->compute(cancel);
        info("Finished computing NavMesh");
    });
}

std::shared_ptr<Creature> Area::makeCreature() {
    return std::make_unique<Creature>(_idCounter++);
}

std::shared_ptr<Door> Area::makeDoor() {
    return std::make_unique<Door>(_idCounter++);
}

std::shared_ptr<Placeable> Area::makePlaceable() {
    return std::make_unique<Placeable>(_idCounter++);
}

std::shared_ptr<Waypoint> Area::makeWaypoint() {
    return std::make_unique<Waypoint>(_idCounter++);
}

std::shared_ptr<Trigger> Area::makeTrigger() {
    return std::make_unique<Trigger>(_idCounter++);
}

void Area::loadProperties(const GffStruct &gffs) {
    ResourceManager &resources = ResourceManager::instance();
    std::shared_ptr<TwoDaTable> musicTable(resources.find2DA("ambientmusic"));

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
        std::unique_ptr<ModelInstance> model(new ModelInstance(resources.findModel(lytRoom.name)));
        model->animate("animloop1", kAnimationLoop);

        std::shared_ptr<Walkmesh> walkmesh(resources.findWalkmesh(lytRoom.name, ResourceType::Walkmesh));
        if (walkmesh) {
            _navMesh->add(walkmesh, glm::mat4(1.0f));
        }

        std::unique_ptr<Room> room(new Room(lytRoom.name, lytRoom.position, std::move(model), std::move(walkmesh)));
        _rooms.push_back(std::move(room));
    }
}

void Area::loadVisibility() {
    ResourceManager &resources = ResourceManager::instance();

    VisFile vis;
    vis.load(wrap(resources.find(_name, ResourceType::Vis)));

    _visibility = std::make_unique<Visibility>(vis.visibility());
}

void Area::loadCameraStyle(const GffStruct &are) {
    int styleIdx = are.getInt("CameraStyle");

    ResourceManager &resources = ResourceManager::instance();
    std::shared_ptr<TwoDaTable> styleTable(resources.find2DA("camerastyle"));

    _cameraStyle.distance = styleTable->getFloat(styleIdx, "distance", 0.0f);
    _cameraStyle.pitch = styleTable->getFloat(styleIdx, "pitch", 0.0f);
    _cameraStyle.viewAngle = styleTable->getFloat(styleIdx, "viewangle", 0.0f);
    _cameraStyle.height = styleTable->getFloat(styleIdx, "height", 0.0f);
}

void Area::landObject(Object &object) {
    glm::vec3 position(object.position());
    if (findElevationAt(position, position.z)) {
        object.setPosition(position);
    }
}

void Area::loadParty(const glm::vec3 &position, float heading) {
    std::shared_ptr<Creature> partyLeader(makeCreature());
    partyLeader->setTag(kPartyLeaderTag);
    partyLeader->equip("g_a_clothes01");

    std::shared_ptr<Creature> partyMember1(makeCreature());
    partyMember1->setTag(kPartyMember1Tag);
    partyMember1->equip("g_a_clothes01");

    switch (_version) {
        case GameVersion::KotOR:
            partyLeader->load(kCarthAppearance, position, heading);
            partyMember1->load(kBastilaAppearance, position, heading);
            break;
        case GameVersion::TheSithLords:
            partyLeader->load(kAttonAppearance, position, heading);
            partyMember1->load(kKreiaAppearance, position, heading);
            break;
    }

    _player = partyLeader;
    _partyLeader = partyLeader;
    _partyMember1 = partyMember1;

    _objects[ObjectType::Creature].push_back(_partyLeader);
    _objects[ObjectType::Creature].push_back(_partyMember1);

    landObject(*_partyLeader);
    landObject(*_partyMember1);

    Creature::Action action(Creature::ActionType::Follow, _partyLeader, kPartyMemberFollowDistance);
    static_cast<Creature &>(*_partyMember1).enqueue(std::move(action));

    // DEBUG: everyone follows player
    //for (auto &crit : _objects[ObjectType::Creature]) {
    //    if (crit->tag().empty()) continue;
    //    static_cast<Creature &>(*crit).enqueue(Creature::Action(Creature::ActionType::Follow, _player, kCompanionFollowDistance));
    //}
}

bool Area::handle(const SDL_Event &event) {
    return false;
}

void Area::update(const UpdateContext &updateCtx, GuiContext &guiCtx) {
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

    auto &opaque = _renderLists[RenderListName::Opaque];
    auto &transparent = _renderLists[RenderListName::Transparent];

    opaque.clear();
    transparent.clear();

    for (auto &room : _rooms) {
        std::shared_ptr<ModelInstance> model(room->model());
        if (!model) continue;

        model->update(updateCtx.deltaTime);

        glm::mat4 transform(glm::translate(glm::mat4(1.0f), room->position()));
        model->fillRenderLists(transform, opaque, transparent);
    }
    for (auto &pair : _objects) {
        for (auto &object : pair.second) {
            object->update(updateCtx);

            std::shared_ptr<ModelInstance> model(object->model());
            if (!model) continue;

            glm::mat4 transform(object->transform());
            model->fillRenderLists(transform, opaque, transparent);
        }
    }

    std::sort(
        transparent.begin(),
        transparent.end(),
        [&](const RenderListItem &left, const RenderListItem &right) { return glm::distance2(left.center, updateCtx.cameraPosition) > glm::distance2(right.center, updateCtx.cameraPosition); });

    switch (_debugMode) {
        case DebugMode::ModelNodes:
            for (auto &list : _renderLists) {
                for (auto &item : list.second) {
                    if (glm::distance2(item.center, updateCtx.cameraPosition) > kDrawDebugDistance) continue;

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
    }
}

void Area::updateCreature(Creature &creature, float dt) {
    const Creature::Action &action = creature.currentAction();
    if (action.type == Creature::ActionType::QueueEmpty) return;

    switch (action.type) {
        case Creature::ActionType::MoveToPoint:
        case Creature::ActionType::Follow:
            glm::vec3 dest(action.type == Creature::ActionType::Follow ? action.object->position() : action.point);
            navigateCreature(creature, dest, action.distance, dt);
            break;
    }
}

void Area::navigateCreature(Creature &creature, const glm::vec3 &dest, float distance, float dt) {
    glm::vec3 origin(creature.position());
    float distToDest = glm::distance2(glm::vec2(origin), glm::vec2(dest));

    if (distToDest <= distance) {
        creature.setMovementType(MovementType::None);
        return;
    }

    bool updatePath = true;
    std::shared_ptr<Creature::Path> path(creature.path());

    if (path) {
        uint32_t now = SDL_GetTicks();
        if (path->destination == dest || now - path->timeFound <= kKeepPathDuration) {
            advanceCreatureOnPath(creature, dt);
            updatePath = false;
        }
    }
    if (updatePath) {
        updateCreaturePath(creature, dest);
    }
}

void Area::advanceCreatureOnPath(Creature &creature, float dt) {
    glm::vec3 origin(creature.position());
    std::shared_ptr<Creature::Path> path(creature.path());
    int pointCount = path->points.size();
    glm::vec3 dest(path->pointIdx == pointCount ? path->destination : path->points[path->pointIdx]);

    if (glm::distance2(glm::vec2(origin), glm::vec2(dest)) <= 1.0f) {
        selectNextPathPoint(*path);
    } else if (moveCreatureTowards(creature, dest, dt)) {
        selectNextPathPoint(*path);
        creature.setMovementType(MovementType::Run);
    } else {
        creature.setMovementType(MovementType::None);
    }
}

void Area::selectNextPathPoint(Creature::Path &path) {;
    int pointCount = path.points.size();
    if (path.pointIdx < pointCount) path.pointIdx++;
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

    Object *obstacle = nullptr;
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

        const std::vector<glm::vec3> &geometry = trigger.geometry();
        bool triggered =
            geometry.size() >= 3 && glm::intersectRayTriangle(liftedPosition, down, geometry[0], geometry[1], geometry[2], intersection, distance) ||
            geometry.size() >= 4 && glm::intersectRayTriangle(liftedPosition, down, geometry[2], geometry[3], geometry[0], intersection, distance);

        if (triggered) {
            if (!trigger.linkedToModule().empty() && _onModuleTransition) {
                _onModuleTransition(trigger.linkedToModule(), trigger.linkedTo());
            }
            break;
        }
    }
}

void Area::updateCreaturePath(Creature &creature, const glm::vec3 &dest) {
    if (creature.isPathUpdating()) return;

    creature.setPathUpdating();

    JobExecutor::instance().enqueue([=, &creature](const std::atomic_bool &) {
        glm::vec3 origin(creature.position());
        std::vector<glm::vec3> points(_navMesh->findPath(origin, dest));
        uint32_t now = SDL_GetTicks();

#ifdef DEBUG_PATH
        std::vector<std::string> pointsStr;
        pointsStr.reserve(points.size());

        for (auto &point : points) {
            pointsStr.push_back(str(boost::format("%.0f %.0f") % point.x % point.y));
        }

        debug(boost::join(pointsStr, " | "));
#endif

        creature.setPath(dest, std::move(points), now);
    });
}

void Area::addToDebugContext(const RenderListItem &item, const UpdateContext &updateCtx, DebugContext &debugCtx) const {
    glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec3 position(item.transform[3]);

    DebugObject object;
    object.tag = item.node->name();
    object.text = str(boost::format("%s %s") % item.model->name() % item.node->name());
    object.screenCoords = glm::project(item.center, updateCtx.view, updateCtx.projection, viewport);

    debugCtx.objects.push_back(std::move(object));
}

void Area::addToDebugContext(const Object &object, const UpdateContext &updateCtx, DebugContext &debugCtx) const {
    glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec3 position(object.position());

    DebugObject debugObj;
    debugObj.tag = object.tag();
    debugObj.text = str(boost::format("%s %.2f %.2f %.2f") % object.tag() % position.x % position.y % position.z);
    debugObj.screenCoords = glm::project(position, updateCtx.view, updateCtx.projection, viewport);

    debugCtx.objects.push_back(std::move(debugObj));
}

void Area::setDebugMode(DebugMode mode) {
    _debugMode = mode;
}

void Area::initGL() {
    for (auto &room : _rooms) {
        std::shared_ptr<ModelInstance> model(room->model());
        if (model) model->initGL();
    }
    for (auto &pair : _objects) {
        for (auto &object : pair.second) {
            object->initGL();
        }
    }
}

void Area::render() const {
    for (auto &item : _renderLists.find(RenderListName::Opaque)->second) {
        item.model->render(*item.node, item.transform, _debugMode == DebugMode::ModelNodes);
    }
    for (auto &item : _renderLists.find(RenderListName::Transparent)->second) {
        item.model->render(*item.node, item.transform, _debugMode == DebugMode::ModelNodes);
    }

    if (_debugMode == DebugMode::GameObjects) {
        AABBMesh &aabb = AABBMesh::instance();
        for (auto &list : _objects) {
            if (list.first != ObjectType::Creature &&
                list.first != ObjectType::Door &&
                list.first != ObjectType::Placeable) continue;

            for (auto &object : list.second) {
                std::shared_ptr<ModelInstance> model(object->model());
                if (!model) return;

                aabb.render(model->model()->aabb(), object->transform());
            }
        }
    }
}

void Area::saveTo(GameState &state) const {
    AreaState areaState;

    for (auto &list : _objects) {
        if (list.first != ObjectType::Creature && list.first != ObjectType::Door) continue;

        for (auto &object : list.second) {
            object->saveTo(areaState);
        }
    }

    state.areas[_name] = std::move(areaState);
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

std::shared_ptr<Object> Area::find(uint32_t id) const {
    std::shared_ptr<Object> object;
    for (auto &list : _objects) {
        object = find(id, list.first);
        if (object) return object;
    }

    return nullptr;
}

std::shared_ptr<Object> Area::find(const std::string &tag) const {
    std::shared_ptr<Object> object;
    for (auto &list : _objects) {
        object = find(tag, list.first);
        if (object) return object;
    }

    return nullptr;
}

std::shared_ptr<Object> Area::find(uint32_t id, ObjectType type) const {
    const ObjectList &list = _objects.find(type)->second;

    auto it = std::find_if(
        list.begin(),
        list.end(),
        [&id](const std::shared_ptr<Object> &object) { return object->id() == id; });

    return it != list.end() ? *it : nullptr;
}

std::shared_ptr<Object> Area::find(const std::string &tag, ObjectType type) const {
    const ObjectList &list = _objects.find(type)->second;

    auto it = std::find_if(
        list.begin(),
        list.end(),
        [&tag](const std::shared_ptr<Object> &object) { return object->tag() == tag; });

    return it != list.end() ? *it : nullptr;
}

bool Area::findObstacleByWalkmesh(const glm::vec3 &from, const glm::vec3 &to, int mask, glm::vec3 &intersection, Object **obstacle) const {
    std::vector<std::pair<Object *, float>> candidates;

    for (auto &list : _objects) {
        if (list.first != ObjectType::Door && list.first != ObjectType::Placeable) continue;
        if (list.first == ObjectType::Door && (mask & kObstacleDoor) == 0) continue;
        if (list.first == ObjectType::Placeable && (mask & kObstaclePlaceable) == 0) continue;

        for (auto &object : list.second) {
            if (!object->walkmesh() ||
                object->type() == ObjectType::Door && static_cast<Door &>(*object).isOpen()) continue;

            float distToFrom = object->distanceTo(from);
            if (distToFrom > kMaxDistanceToTestCollision) continue;

            candidates.push_back(std::make_pair(object.get(), distToFrom));
        }
    }

    std::sort(
        candidates.begin(),
        candidates.end(),
        [](const std::pair<Object *, float> &left, const std::pair<Object *, float> &right) { return left.second < right.second; });

    for (auto &pair : candidates) {
        Object &object = *pair.first;

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
        std::shared_ptr<Walkmesh> walkmesh(room->walkmesh());
        if (!walkmesh) continue;

        AABB aabb(walkmesh->aabb());
        if (!aabb.contains(from) && !aabb.contains(to)) continue;

        if (walkmesh->findObstacle(from, to, intersection)) {
            return true;
        }
    }

    return false;
}

bool Area::findObstacleByAABB(const glm::vec3 &from, const glm::vec3 &to, int mask, const Object *except, Object **obstacle) const {
    std::vector<std::pair<Object *, float>> candidates;

    for (auto &list : _objects) {
        if (list.first != ObjectType::Creature &&
            list.first != ObjectType::Door &&
            list.first != ObjectType::Placeable) continue;

        if (list.first == ObjectType::Creature && (mask & kObstacleCreature) == 0) continue;
        if (list.first == ObjectType::Door && (mask & kObstacleDoor) == 0) continue;
        if (list.first == ObjectType::Placeable && (mask & kObstaclePlaceable) == 0) continue;

        for (auto &object : list.second) {
            if (!object->model() ||
                object->type() == ObjectType::Door && static_cast<Door &>(*object).isOpen() ||
                except && object.get() == except) continue;

            float distToFrom = object->distanceTo(from);
            if (distToFrom > kMaxDistanceToTestCollision) continue;

            candidates.push_back(std::make_pair(object.get(), distToFrom));
        }
    }

    std::sort(
        candidates.begin(),
        candidates.end(),
        [](const std::pair<Object *, float> &left, const std::pair<Object *, float> &right) { return left.second < right.second; });

    for (auto &pair : candidates) {
        Object *object = pair.first;
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
    Object *obstacle = nullptr;

    if (findObstacleByWalkmesh(from, to, kObstacleDoor | kObstaclePlaceable, intersection, &obstacle)) {
        return false;
    }
    for (auto &room : _rooms) {
        std::shared_ptr<Walkmesh> walkmesh(room->walkmesh());
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

const std::string &Area::music() const {
    return _music;
}

std::shared_ptr<Object> Area::player() const {
    return _player;
}

std::shared_ptr<Object> Area::partyLeader() const {
    return _partyLeader;
}

std::shared_ptr<Object> Area::partyMember1() const {
    return _partyMember1;
}

std::shared_ptr<Object> Area::partyMember2() const {
    return _partyMember2;
}

void Area::setOnModuleTransition(const std::function<void(const std::string &, const std::string &)> &fn) {
    _onModuleTransition = fn;
}

void Area::setOnPlayerChanged(const std::function<void()> &fn) {
    _onPlayerChanged = fn;
}

} // namespace game

} // namespace reone
