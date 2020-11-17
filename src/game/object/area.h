/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#pragma once

#include <memory>
#include <set>
#include <vector>

#include "glm/vec3.hpp"

#include "SDL2/SDL_events.h"

#include "../../render/types.h"
#include "../../resource/gfffile.h"
#include "../../resource/types.h"

#include "../combat.h"
#include "../actionexecutor.h"
#include "../camera/animatedcamera.h"
#include "../camera/dialogcamera.h"
#include "../camera/firstperson.h"
#include "../camera/staticcamera.h"
#include "../camera/thirdperson.h"
#include "../camera/types.h"
#include "../collisiondetect.h"
#include "../objectselect.h"
#include "../pathfinder.h"

#include "object.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace game {

const float kRoundDuration = 6.0f;

typedef std::unordered_map<std::string, std::shared_ptr<Room>> RoomMap;
typedef std::vector<std::shared_ptr<SpatialObject>> ObjectList;

class Game;

class Area : public Object {
public:
    Area(uint32_t id, Game *game);

    void load(const std::string &name, const resource::GffStruct &are, const resource::GffStruct &git);

    bool handle(const SDL_Event &event);
    void update(float dt);

    void destroyObject(const SpatialObject &object);
    void fill(scene::SceneGraph &sceneGraph);
    void initCameras(const glm::vec3 &entryPosition, float entryHeading);
    bool moveCreatureTowards(Creature &creature, const glm::vec2 &dest, bool run, float dt);
    void onPartyLeaderMoved();
    void runOnEnterScript();
    void startDialog(SpatialObject &object, const std::string &resRef);
    void update3rdPersonCameraHeading();
    void update3rdPersonCameraTarget();

    std::shared_ptr<SpatialObject> getObjectAt(int x, int y) const;
    glm::vec3 getSelectableScreenCoords(const std::shared_ptr<SpatialObject> &object, const glm::mat4 &projection, const glm::mat4 &view) const;

    const CameraStyle &cameraStyle() const;
    const CollisionDetector &collisionDetector() const;
    const std::string &music() const;
    const ObjectList &objects() const;
    ObjectSelector &objectSelector();
    const Pathfinder &pathfinder() const;
    const RoomMap &rooms() const;
    Combat& combat() { return _combat;  }
    std::unordered_map<ObjectType, ObjectList> &objectsByType() { return _objectsByType; }

    // Objects

    std::shared_ptr<SpatialObject> find(uint32_t id) const;
    std::shared_ptr<SpatialObject> find(const std::string &tag, int nth = 0) const;

    // END Objects

    // Cameras

    Camera &getCamera(CameraType type);

    void setStaticCamera(int cameraId);

    // END Cameras

    // Party

    void loadParty(const glm::vec3 &position, float heading);
    void unloadParty();

    // END Party

private:
    Game *_game { nullptr };
    CollisionDetector _collisionDetector;
    ObjectSelector _objectSelector;
    ActionExecutor _actionExecutor;
    Combat _combat;
    Pathfinder _pathfinder;
    std::string _name;
    RoomMap _rooms;
    std::unique_ptr<resource::Visibility> _visibility;
    CameraStyle _cameraStyle;
    std::string _music;
    float _heartbeatTimeout { kRoundDuration };

    // Scripts

    std::string _onEnter;
    std::string _onExit;
    std::string _onHeartbeat;

    // END Scripts

    // Cameras

    float _cameraAspect { 0.0f };
    std::unique_ptr<FirstPersonCamera> _firstPersonCamera;
    std::unique_ptr<ThirdPersonCamera> _thirdPersonCamera;
    std::unique_ptr<DialogCamera> _dialogCamera;
    std::unique_ptr<AnimatedCamera> _animatedCamera;
    std::unique_ptr<StaticCamera> _staticCamera;

    // END Cameras

    // Objects

    ObjectList _objects;
    std::unordered_map<ObjectType, ObjectList> _objectsByType;
    std::unordered_map<uint32_t, std::shared_ptr<SpatialObject>> _objectById;
    std::unordered_map<std::string, ObjectList> _objectsByTag;
    std::set<uint32_t> _objectsToDestroy;

    // END Objects

    void add(const std::shared_ptr<SpatialObject> &object);
    void determineObjectRoom(SpatialObject &object);
    void doDestroyObject(uint32_t objectId);
    void doDestroyObjects();
    void landObject(SpatialObject &object);
    void checkTriggersIntersection(SpatialObject &triggerrer);
    void updateVisibility();
    void updateSounds();
    void updateHeartbeat(float dt);

    void printDebugInfo(const SpatialObject &object);

    bool findCameraObstacle(const glm::vec3 &origin, const glm::vec3 &dest, glm::vec3 &intersection) const;
    bool findCreatureObstacle(const Creature &creature, const glm::vec3 &dest) const;
    bool getElevationAt(const glm::vec2 &position, const SpatialObject *except, Room *&room, float &z) const;

    // Loading

    void loadLYT();
    void loadVIS();
    void loadPTH();
    void loadARE(const resource::GffStruct &are);
    void loadCameraStyle(const resource::GffStruct &are);
    void loadAmbientColor(const resource::GffStruct &are);
    void loadScripts(const resource::GffStruct &are);
    void loadGIT(const resource::GffStruct &gffs);
    void loadProperties(const resource::GffStruct &git);
    void loadCreatures(const resource::GffStruct &git);
    void loadDoors(const resource::GffStruct &git);
    void loadPlaceables(const resource::GffStruct &git);
    void loadWaypoints(const resource::GffStruct &git);
    void loadTriggers(const resource::GffStruct &git);
    void loadSounds(const resource::GffStruct &git);
    void loadCameras(const resource::GffStruct &git);

    // END Loading

    // User input

    bool handleKeyDown(const SDL_KeyboardEvent &event);

    // END User input
};

} // namespace game

} // namespace reone
