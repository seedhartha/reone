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

#pragma once

#include <memory>
#include <set>
#include <vector>

#include "glm/vec3.hpp"

#include "SDL2/SDL_events.h"

#include "../../system/render/types.h"
#include "../../system/resource/gfffile.h"
#include "../../system/resource/types.h"

#include "../actionexecutor.h"
#include "../camera/animatedcamera.h"
#include "../camera/dialogcamera.h"
#include "../camera/firstperson.h"
#include "../camera/thirdperson.h"
#include "../collisiondetect.h"
#include "../objectselect.h"
#include "../pathfinder.h"

#include "object.h"

namespace reone {

namespace game {

typedef std::unordered_map<std::string, std::shared_ptr<Room>> RoomMap;
typedef std::vector<std::shared_ptr<SpatialObject>> ObjectList;

class Game;
class ObjectFactory;

class Area : public Object {
public:
    Area(
        uint32_t id,
        resource::GameVersion version,
        Game *game,
        ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph,
        const render::GraphicsOptions &opts);

    void load(const std::string &name, const resource::GffStruct &are, const resource::GffStruct &git);
    void loadParty(const PartyConfiguration &party, const glm::vec3 &position, float heading);
    void loadCameras(const glm::vec3 &entryPosition, float entryHeading);

    bool handle(const SDL_Event &event);
    void update(const UpdateContext &updateCtx);

    void destroyObject(const std::shared_ptr<SpatialObject> &object);
    void fill(const UpdateContext &updateCtx, GuiContext &guiCtx);
    bool moveCreatureTowards(Creature &creature, const glm::vec2 &dest, bool run, float dt);
    void runOnEnterScript();
    void startDialog(Creature &creature, const std::string &resRef);
    void switchTo3rdPersonCamera();
    void toggleCameraType();
    void update3rdPersonCameraTarget();
    void update3rdPersonCameraHeading();

    void onPlayerMoved();

    Camera *getCamera() const;
    SpatialObject *getObjectAt(int x, int y) const;

    const CameraStyle &cameraStyle() const;
    CameraType cameraType() const;
    const std::string &music() const;
    const RoomMap &rooms() const;
    const ObjectList &objects() const;
    const CollisionDetector &collisionDetector() const;
    ObjectSelector &objectSelector();
    const Pathfinder &pathfinder() const;
    ThirdPersonCamera *thirdPersonCamera();
    DialogCamera &dialogCamera();
    AnimatedCamera &animatedCamera();

    // Objects

    std::shared_ptr<SpatialObject> find(uint32_t id) const;
    std::shared_ptr<SpatialObject> find(const std::string &tag, int nth = 0) const;

    // END Objects

    // Party

    std::shared_ptr<SpatialObject> player() const;
    std::shared_ptr<SpatialObject> partyLeader() const;
    std::shared_ptr<SpatialObject> partyMember1() const;
    std::shared_ptr<SpatialObject> partyMember2() const;

    // END Party

private:
    Game *_game { nullptr };
    resource::GameVersion _version { resource::GameVersion::KotOR };
    ObjectFactory *_objectFactory { nullptr };
    scene::SceneGraph *_sceneGraph { nullptr };
    render::GraphicsOptions _opts;
    CollisionDetector _collisionDetector;
    ObjectSelector _objectSelector;
    ActionExecutor _actionExecutor;
    Pathfinder _pathfinder;
    std::string _name;
    RoomMap _rooms;
    std::unique_ptr<resource::Visibility> _visibility;
    CameraStyle _cameraStyle;
    std::string _music;

    // Cameras

    float _cameraAspect { 0.0f };
    CameraType _cameraType { CameraType::FirstPerson };
    std::unique_ptr<FirstPersonCamera> _firstPersonCamera;
    std::unique_ptr<ThirdPersonCamera> _thirdPersonCamera;
    std::unique_ptr<DialogCamera> _dialogCamera;
    std::unique_ptr<AnimatedCamera> _animatedCamera;

    // END Cameras

    // Objects

    ObjectList _objects;
    std::unordered_map<ObjectType, ObjectList> _objectsByType;
    std::unordered_map<uint32_t, std::shared_ptr<SpatialObject>> _objectById;
    std::unordered_map<std::string, ObjectList> _objectsByTag;
    std::set<uint32_t> _objectsToDestroy;

    // END Objects

    // Party

    std::shared_ptr<SpatialObject> _player;
    std::shared_ptr<SpatialObject> _partyLeader;
    std::shared_ptr<SpatialObject> _partyMember1;
    std::shared_ptr<SpatialObject> _partyMember2;

    // END Party

    void add(const std::shared_ptr<SpatialObject> &object);
    void addDebugInfo(const UpdateContext &updateCtx, GuiContext &guiCtx);
    void addPartyMemberPortrait(const std::shared_ptr<SpatialObject> &object, GuiContext &ctx);
    void determineObjectRoom(SpatialObject &object);
    void doDestroyObject(uint32_t objectId);
    void doDestroyObjects();
    void landObject(SpatialObject &object);
    std::shared_ptr<Creature> makeCharacter(const CreatureConfiguration &character, const std::string &tag, const glm::vec3 &position, float heading);
    void updateRoomVisibility();
    void updateTriggers(const Creature &creature);

    bool findCameraObstacle(const glm::vec3 &origin, const glm::vec3 &dest, glm::vec3 &intersection) const;
    bool findCreatureObstacle(const Creature &creature, const glm::vec3 &dest) const;
    bool getElevationAt(const glm::vec2 &position, Room *&room, float &z) const;
    glm::vec3 getSelectableScreenCoords(uint32_t objectId, const UpdateContext &ctx) const;

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

    // END Loading

    // User input

    bool handleKeyDown(const SDL_KeyboardEvent &event);

    // END User input
};

} // namespace game

} // namespace reone
