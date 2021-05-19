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

#pragma once

#include <set>
#include <vector>

#include "glm/vec3.hpp"

#include "SDL2/SDL_events.h"

#include "../../common/timer.h"
#include "../../graphics/types.h"
#include "../../resource/format/gffreader.h"
#include "../../resource/types.h"
#include "../../scene/scenegraph.h"

#include "../actionexecutor.h"
#include "../camera/animatedcamera.h"
#include "../camera/dialogcamera.h"
#include "../camera/firstperson.h"
#include "../camera/staticcamera.h"
#include "../camera/thirdperson.h"
#include "../map.h"
#include "../pathfinder.h"
#include "../types.h"

#include "object.h"

namespace reone {

namespace game {

const float kHeartbeatInterval = 6.0f;

typedef std::unordered_map<std::string, std::shared_ptr<Room>> RoomMap;
typedef std::vector<std::shared_ptr<SpatialObject>> ObjectList;

class Game;

class Area : public Object {
public:
    typedef std::vector<std::pair<CreatureType, int>> SearchCriteriaList;

    Area(uint32_t id, Game *game);

    void load(const std::string &name, const resource::GffStruct &are, const resource::GffStruct &git);

    bool handle(const SDL_Event &event);
    void update(float dt);

    void destroyObject(const SpatialObject &object);
    void fill(scene::SceneGraph &sceneGraph);
    void initCameras(const glm::vec3 &entryPosition, float entryFacing);

    void onPartyLeaderMoved(bool roomChanged = false);

    void startDialog(const std::shared_ptr<SpatialObject> &object, const std::string &resRef);
    void update3rdPersonCameraFacing();
    void update3rdPersonCameraTarget();
    void landObject(SpatialObject &object);

    bool moveCreature(const std::shared_ptr<Creature> &creature, const glm::vec2 &dir, bool run, float dt);
    bool moveCreatureTowards(const std::shared_ptr<Creature> &creature, const glm::vec2 &dest, bool run, float dt);

    bool isUnescapable() const { return _unescapable; }

    std::shared_ptr<SpatialObject> getObjectAt(int x, int y) const;
    glm::vec3 getSelectableScreenCoords(const std::shared_ptr<SpatialObject> &object, const glm::mat4 &projection, const glm::mat4 &view) const;

    const CameraStyle &camStyleDefault() const { return _camStyleDefault; }
    const std::string &music() const { return _music; }
    const ObjectList &objects() const { return _objects; }
    const Pathfinder &pathfinder() const { return _pathfinder; }
    const std::string &localizedName() const { return _localizedName; }
    const RoomMap &rooms() const { return _rooms; }
    Map &map() { return _map; }

    void setUnescapable(bool value);

    // Objects

    std::shared_ptr<Object> createObject(ObjectType type, const std::string &blueprintResRef, const std::shared_ptr<Location> &location);

    bool isInLineOfSight(const Creature &subject, const SpatialObject &target) const;

    ObjectList &getObjectsByType(ObjectType type);
    std::shared_ptr<SpatialObject> getObjectByTag(const std::string &tag, int nth = 0) const;

    // END Objects

    // Object Search

    /**
     * Find the nth nearest object for which the specified predicate returns true.
     *
     * @param nth a 0-based object index
     */
    std::shared_ptr<SpatialObject> getNearestObject(const glm::vec3 &origin, int nth, const std::function<bool(const std::shared_ptr<SpatialObject> &)> &predicate);

    /**
     * @param nth 0-based index of the creature
     * @return nth nearest creature to the target object, that matches the specified criterias
     */
    std::shared_ptr<Creature> getNearestCreature(const std::shared_ptr<SpatialObject> &target, const SearchCriteriaList &criterias, int nth = 0);

    /**
     * @param nth 0-based index of the creature
     * @return nth nearest creature to the location, that matches the specified criterias
     */
    std::shared_ptr<Creature> getNearestCreatureToLocation(const Location &location, const SearchCriteriaList &criterias, int nth = 0);

    // END Object Search

    // Cameras

    Camera &getCamera(CameraType type);

    void setStaticCamera(int cameraId);
    void setThirdPartyCameraStyle(CameraStyleType type);

    template <class T>
    T &getCamera(CameraType type) {
        return static_cast<T &>(getCamera(type));
    };

    // END Cameras

    // Party

    void loadParty(const glm::vec3 &position, float facing);
    void unloadParty();
    void reloadParty();

    // END Party

    // Perception

    void updatePerception(float dt);

    // END Perception

    // Object Selection

    void selectNextObject(bool reverse = false);
    void selectNearestObject();
    void hilightObject(std::shared_ptr<SpatialObject> object);
    void selectObject(std::shared_ptr<SpatialObject> object);

    std::vector<std::shared_ptr<SpatialObject>> getSelectableObjects() const;

    std::shared_ptr<SpatialObject> hilightedObject() const { return _hilightedObject; }
    std::shared_ptr<SpatialObject> selectedObject() const { return _selectedObject; }

    // END Object Selection

    // Stealth

    bool isStealthXPEnabled() const { return _stealthXPEnabled; }

    int maxStealthXP() const { return _maxStealthXP; }
    int currentStealthXP() const { return _currentStealthXP; }
    int stealthXPDecrement() const { return _stealthXPDecrement; }

    void setStealthXPEnabled(bool value);
    void setMaxStealthXP(int value);
    void setCurrentStealthXP(int value);
    void setStealthXPDecrement(int value);

    // END Stealth

    // Scripts

    void runSpawnScripts();
    void runOnEnterScript();
    void runOnExitScript();

    // END Scripts

private:
    struct Grass {
        std::shared_ptr<graphics::Texture> texture;
        float density { 0.0f };
        float quadSize { 0.0f };
        int ambient { 0 };
        int diffuse { 0 };
        float probabilities[4];
    };

    Game *_game;

    ActionExecutor _actionExecutor;
    Pathfinder _pathfinder;
    std::string _localizedName;
    RoomMap _rooms;
    resource::Visibility _visibility;
    CameraStyle _camStyleDefault;
    CameraStyle _camStyleCombat;
    std::string _music;
    Timer _heartbeatTimer;
    Map _map;
    bool _unescapable { false };
    Grass _grass;
    glm::vec3 _ambientColor { 0.0f };
    Timer _perceptionTimer;
    std::shared_ptr<SpatialObject> _hilightedObject;
    std::shared_ptr<SpatialObject> _selectedObject;

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
    std::unordered_map<std::string, ObjectList> _objectsByTag;
    std::set<uint32_t> _objectsToDestroy;

    // END Objects

    // Stealth

    bool _stealthXPEnabled { false };
    int _maxStealthXP { 0 };
    int _currentStealthXP { 0 };
    int _stealthXPDecrement { 0 };

    // END Stealth

    // Fog

    bool _fogEnabled { false };
    float _fogNear { 0.0f };
    float _fogFar { 0.0f };
    glm::vec3 _fogColor { 0.0f };

    // END Fog

    void init();

    void loadLYT();
    void loadVIS();
    void loadPTH();

    void add(const std::shared_ptr<SpatialObject> &object);
    void doDestroyObject(uint32_t objectId);
    void doDestroyObjects();
    void updateVisibility();
    void updateSounds();
    void updateHeartbeat(float dt);

    void updateRoomVisibility();

    /**
     * Certain VIS files in the original game have a bug: room A is visible from
     * room B, but room B is not visible from room A. This function makes room
     * relations symmetric.
     */
    resource::Visibility fixVisibility(const resource::Visibility &visiblity);

    void printDebugInfo(const SpatialObject &object);

    bool doMoveCreature(const std::shared_ptr<Creature> &creature, const glm::vec3 &dest);

    int getNumGrassClusters(float area) const;
    int getRandomGrassVariant() const;

    // Loading ARE

    void loadARE(const resource::GffStruct &are);

    void loadCameraStyle(const resource::GffStruct &are);
    void loadAmbientColor(const resource::GffStruct &are);
    void loadScripts(const resource::GffStruct &are);
    void loadMap(const resource::GffStruct &are);
    void loadStealthXP(const resource::GffStruct &are);
    void loadGrass(const resource::GffStruct &are);
    void loadFog(const resource::GffStruct &are);

    // END Loading ARE

    // Loading GIT

    void loadGIT(const resource::GffStruct &gffs);

    void loadProperties(const resource::GffStruct &git);
    void loadCreatures(const resource::GffStruct &git);
    void loadDoors(const resource::GffStruct &git);
    void loadPlaceables(const resource::GffStruct &git);
    void loadWaypoints(const resource::GffStruct &git);
    void loadTriggers(const resource::GffStruct &git);
    void loadSounds(const resource::GffStruct &git);
    void loadCameras(const resource::GffStruct &git);
    void loadEncounters(const resource::GffStruct &git);

    // END Loading GIT

    // User input

    bool handleKeyDown(const SDL_KeyboardEvent &event);

    // END User input

    // Collision detection

    void determineObjectRoom(SpatialObject &object);
    void checkTriggersIntersection(const std::shared_ptr<SpatialObject> &triggerrer);

    /**
     * @param position 2D coordinates to test elevation at
     * @param[out] z elevation at the specified 2D coordinates
     * @param[out] room room which the walkable face belongs to, if any
     * @param[out] material material of the walkable face, if any
     *
     * @return true if there is a walkable face at the specified coordinates, false otherwise
     */
    bool testElevationAt(const glm::vec2 &point, float &z, int &material, Room *&room) const;

    /**
     * @param start start of the camera path
     * @param end end of the camera path
     * @param[out] intersection intersection point with a non-walkable face, if any
     *
     * @return true if a walkmesh or an object are blocking the camera path, false otherwise
     */
    bool getCameraObstacle(const glm::vec3 &start, const glm::vec3 &end, glm::vec3 &intersection) const;

    /**
     * @param start start of the creature path
     * @param end end of the creature path
     *
     * @return true if a walkmesh or an object are blocking the creature path, false otherwise
     */
    bool getCreatureObstacle(const glm::vec3 &start, const glm::vec3 &end, glm::vec3 &normal) const;

    // END Collision detection

    // Creature Search

    bool matchesCriterias(const Creature &creature, const SearchCriteriaList &criterias, std::shared_ptr<SpatialObject> target = nullptr) const;

    // END Creature Search

    // Perception

    void doUpdatePerception();

    // END Perception

    // Object Selection

    void updateObjectSelection();

    // END Object Selection
};

} // namespace game

} // namespace reone
