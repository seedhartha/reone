/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/texture.h"
#include "reone/graphics/types.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/types.h"
#include "reone/system/timer.h"

#include "../generated/are.h"
#include "../generated/git.h"
#include "../object.h"
#include "../object/camera/animated.h"
#include "../object/camera/dialog.h"
#include "../object/camera/firstperson.h"
#include "../object/camera/static.h"
#include "../object/camera/thirdperson.h"
#include "../pathfinder.h"
#include "../types.h"

namespace reone {

namespace graphics {

class Window;

}

namespace game {

const float kHeartbeatInterval = 6.0f;

class Creature;
class Location;
class Object;
class Room;

using RoomMap = std::unordered_map<std::string, std::shared_ptr<Room>>;
using ObjectList = std::vector<std::shared_ptr<Object>>;

class Area : public Object {
public:
    struct Grass {
        std::shared_ptr<graphics::Texture> texture;
        float density {0.0f};
        float quadSize {0.0f};
        int ambient {0};
        int diffuse {0};
        glm::vec4 probabilities {0.0f};
    };

    using SearchCriteriaList = std::vector<std::pair<CreatureType, int>>;

    Area(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services);

    void load(std::string name, const resource::Gff &are, const resource::Gff &git, bool fromSave = false);

    bool handle(const SDL_Event &event);
    void update(float dt);

    void destroyObject(const Object &object);
    void initCameras(const glm::vec3 &entryPosition, float entryFacing);

    void onPartyLeaderMoved(bool roomChanged = false);
    void updateRoomVisibility();

    void startDialog(const std::shared_ptr<Object> &object, const std::string &resRef);
    void update3rdPersonCameraFacing();
    void update3rdPersonCameraTarget();
    void landObject(Object &object);

    bool moveCreature(const std::shared_ptr<Creature> &creature, const glm::vec2 &dir, bool run, float dt);
    bool moveCreatureTowards(const std::shared_ptr<Creature> &creature, const glm::vec2 &dest, bool run, float dt);

    bool isUnescapable() const { return _unescapable; }

    Object *getObjectAt(int x, int y) const;
    glm::vec3 getSelectableScreenCoords(const std::shared_ptr<Object> &object, const glm::mat4 &projection, const glm::mat4 &view) const;

    const CameraStyle &camStyleDefault() const { return _camStyleDefault; }
    const std::string &music() const { return _music; }
    const ObjectList &objects() const { return _objects; }
    const Pathfinder &pathfinder() const { return _pathfinder; }
    const std::string &localizedName() const { return _localizedName; }
    const RoomMap &rooms() const { return _rooms; }
    const Grass &grass() const { return _grass; }
    const glm::vec3 &ambientColor() const { return _ambientColor; }

    void setUnescapable(bool value);

    // Objects

    std::shared_ptr<Object> createObject(ObjectType type, const std::string &blueprintResRef, const std::shared_ptr<Location> &location);

    bool isObjectSeen(const Creature &subject, const Object &object) const;

    ObjectList &getObjectsByType(ObjectType type);
    std::shared_ptr<Object> getObjectByTag(const std::string &tag, int nth = 0) const;

    // END Objects

    // Object Search

    /**
     * Find the nth nearest object for which the specified predicate returns true.
     *
     * @param nth a 0-based object index
     */
    std::shared_ptr<Object> getNearestObject(const glm::vec3 &origin, int nth, const std::function<bool(const std::shared_ptr<Object> &)> &predicate);

    /**
     * @param nth 0-based index of the creature
     * @return nth nearest creature to the target object, that matches the specified criterias
     */
    std::shared_ptr<Creature> getNearestCreature(const std::shared_ptr<Object> &target, const SearchCriteriaList &criterias, int nth = 0);

    /**
     * @param nth 0-based index of the creature
     * @return nth nearest creature to the location, that matches the specified criterias
     */
    std::shared_ptr<Creature> getNearestCreatureToLocation(const Location &location, const SearchCriteriaList &criterias, int nth = 0);

    // END Object Search

    // Cameras

    Camera *getCamera(CameraType type);

    void setStaticCamera(int cameraId);
    void setThirdPartyCameraStyle(CameraStyleType type);

    template <class T>
    T *getCamera(CameraType type) {
        return static_cast<T *>(getCamera(type));
    };

    // END Cameras

    // Party

    void loadParty(const glm::vec3 &position, float facing, bool fromSave = false);
    void unloadParty();
    void reloadParty();

    // END Party

    // Perception

    void updatePerception(float dt);

    // END Perception

    // Object Selection

    void hilightObject(std::shared_ptr<Object> object);
    void selectObject(std::shared_ptr<Object> object);

    std::shared_ptr<Object> hilightedObject() const { return _hilightedObject; }
    std::shared_ptr<Object> selectedObject() const { return _selectedObject; }

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

    // Fog

    bool fogEnabled() const { return _fogEnabled; }
    float fogNear() const { return _fogNear; }
    float fogFar() const { return _fogFar; }
    const glm::vec3 &fogColor() const { return _fogColor; }

    // END Fog

    // Scripts

    void runSpawnScripts();
    void runOnEnterScript();
    void runOnExitScript();

    // END Scripts

private:
    std::string _sceneName;

    Pathfinder _pathfinder;
    std::string _localizedName;
    RoomMap _rooms;
    Visibility _visibility;
    CameraStyle _camStyleDefault;
    CameraStyle _camStyleCombat;
    std::string _music;
    Timer _heartbeatTimer;
    bool _unescapable {false};
    Grass _grass;
    glm::vec3 _ambientColor {0.0f};
    Timer _perceptionTimer;
    std::shared_ptr<Object> _hilightedObject;
    std::shared_ptr<Object> _selectedObject;

    // Scripts

    std::string _onEnter;
    std::string _onExit;
    std::string _onHeartbeat;

    // END Scripts

    // Cameras

    float _cameraAspect {0.0f};
    std::shared_ptr<FirstPersonCamera> _firstPersonCamera;
    std::shared_ptr<ThirdPersonCamera> _thirdPersonCamera;
    std::shared_ptr<DialogCamera> _dialogCamera;
    std::shared_ptr<AnimatedCamera> _animatedCamera;
    StaticCamera *_staticCamera {nullptr};

    // END Cameras

    // Objects

    ObjectList _objects;
    std::unordered_map<ObjectType, ObjectList> _objectsByType;
    std::unordered_map<std::string, ObjectList> _objectsByTag;
    std::set<uint32_t> _objectsToDestroy;

    // END Objects

    // Stealth

    bool _stealthXPEnabled {false};
    int _maxStealthXP {0};
    int _currentStealthXP {0};
    int _stealthXPDecrement {0};

    // END Stealth

    // Fog

    bool _fogEnabled {false};
    float _fogNear {0.0f};
    float _fogFar {0.0f};
    glm::vec3 _fogColor {0.0f};

    // END Fog

    void init();

    void loadLYT();
    void loadVIS();
    void loadPTH();

    void add(const std::shared_ptr<Object> &object);
    void doDestroyObject(uint32_t objectId);
    void doDestroyObjects();
    void updateVisibility();
    void updateHeartbeat(float dt);

    void doUpdatePerception();
    void updateObjectSelection();

    bool matchesCriterias(const Creature &creature, const SearchCriteriaList &criterias, std::shared_ptr<Object> target = nullptr) const;

    /**
     * Certain VIS files in the original game have a bug: room A is visible from
     * room B, but room B is not visible from room A. This function makes room
     * relations symmetric.
     */
    Visibility fixVisibility(const Visibility &visiblity);

    void determineObjectRoom(Object &object);
    void checkTriggersIntersection(const std::shared_ptr<Object> &triggerrer);

    // Loading ARE

    void loadARE(const generated::ARE &are);

    void loadCameraStyle(const generated::ARE &are);
    void loadAmbientColor(const generated::ARE &are);
    void loadScripts(const generated::ARE &are);
    void loadMap(const generated::ARE &are);
    void loadStealthXP(const generated::ARE &are);
    void loadGrass(const generated::ARE &are);
    void loadFog(const generated::ARE &are);

    // END Loading ARE

    // Loading GIT

    void loadGIT(const generated::GIT &git);

    void loadProperties(const generated::GIT &git);
    void loadCreatures(const generated::GIT &git);
    void loadDoors(const generated::GIT &git);
    void loadPlaceables(const generated::GIT &git);
    void loadWaypoints(const generated::GIT &git);
    void loadTriggers(const generated::GIT &git);
    void loadSounds(const generated::GIT &git);
    void loadCameras(const generated::GIT &git);
    void loadEncounters(const generated::GIT &git);
    void loadStores(const generated::GIT &git);

    // END Loading GIT

    // User input

    bool handleKeyDown(const SDL_KeyboardEvent &event);

    // END User input
};

} // namespace game

} // namespace reone
