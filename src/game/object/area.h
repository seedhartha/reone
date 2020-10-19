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

#include <list>

#include "../../system/gui/types.h"
#include "../../system/net/types.h"
#include "../../system/scene/scenegraph.h"
#include "../../system/render/types.h"
#include "../../system/resource/types.h"
#include "../../system/script/variable.h"

#include "../actionexecutor.h"
#include "../camera/animatedcamera.h"
#include "../camera/dialogcamera.h"
#include "../camera/firstperson.h"
#include "../camera/thirdperson.h"
#include "../collisiondetect.h"
#include "../objectselect.h"
#include "../pathfinder.h"
#include "../room.h"

#include "creature.h"
#include "door.h"
#include "placeable.h"
#include "trigger.h"
#include "waypoint.h"

namespace reone {

namespace game {

typedef std::unordered_map<std::string, std::shared_ptr<Room>> RoomMap;
typedef std::vector<std::shared_ptr<SpatialObject>> ObjectList;

class ObjectFactory;

class Area : public Object {
public:
    Area(
        uint32_t id,
        resource::GameVersion version,
        ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph,
        const render::GraphicsOptions &opts);

    void load(const std::string &name, const resource::GffStruct &are, const resource::GffStruct &git);
    void loadParty(const PartyConfiguration &party, const glm::vec3 &position, float heading);
    void loadCameras(const glm::vec3 &entryPosition, float entryHeading);
    void runOnEnterScript();

    bool handle(const SDL_Event &event);
    void update(const UpdateContext &updateCtx);

    void fill(const UpdateContext &updateCtx, GuiContext &guiCtx);
    bool moveCreatureTowards(Creature &creature, const glm::vec2 &dest, bool run, float dt);
    void updateTriggers(const Creature &creature);
    void updateRoomVisibility();
    SpatialObject *getObjectAt(int x, int y) const;

    void update3rdPersonCameraTarget();
    void update3rdPersonCameraHeading();
    void switchTo3rdPersonCamera();
    void toggleCameraType();
    Camera *getCamera() const;
    void startDialog(Creature &creature, const std::string &resRef);

    std::shared_ptr<SpatialObject> find(uint32_t id) const;
    std::shared_ptr<SpatialObject> find(const std::string &tag, int nth = 0) const;

    const CameraStyle &cameraStyle() const;
    CameraType cameraType() const;
    const std::string &music() const;
    const RoomMap &rooms() const;
    const ObjectList &objects() const;
    const CollisionDetector &collisionDetector() const;
    const Pathfinder &pathfinder() const;
    ThirdPersonCamera *thirdPersonCamera();
    DialogCamera &dialogCamera();
    AnimatedCamera &animatedCamera();
    ObjectSelector &objectSelector();

    // Load/save

    void saveTo(GameState &state) const;
    void loadState(const GameState &state);

    // END Load/save

    // Party

    std::shared_ptr<SpatialObject> player() const;
    std::shared_ptr<SpatialObject> partyLeader() const;
    std::shared_ptr<SpatialObject> partyMember1() const;
    std::shared_ptr<SpatialObject> partyMember2() const;

    // END Party

    // Callbacks

    void setOnCameraChanged(const std::function<void(CameraType)> &fn);
    void setOnModuleTransition(const std::function<void(const std::string &, const std::string &)> &fn);
    void setOnPlayerChanged(const std::function<void()> &fn);
    void setOnStartDialog(const std::function<void(SpatialObject &, const std::string &)> &fn);

    // END Callbacks

protected:
    ObjectFactory *_objectFactory { nullptr };
    scene::SceneGraph *_sceneGraph { nullptr };
    bool _scriptsEnabled { true };
    float _cameraAspect { 0.0f };
    CameraType _cameraType { CameraType::FirstPerson };
    std::unique_ptr<FirstPersonCamera> _firstPersonCamera;
    std::unique_ptr<ThirdPersonCamera> _thirdPersonCamera;
    std::unique_ptr<DialogCamera> _dialogCamera;
    std::unique_ptr<AnimatedCamera> _animatedCamera;
    std::function<void()> _onPlayerChanged;

    ObjectList _objects;
    std::unordered_map<ObjectType, ObjectList> _objectsByType;
    std::unordered_map<uint32_t, std::shared_ptr<SpatialObject>> _objectById;
    std::unordered_map<std::string, ObjectList> _objectsByTag;

    // Party

    std::shared_ptr<SpatialObject> _player;
    std::shared_ptr<SpatialObject> _partyLeader;
    std::shared_ptr<SpatialObject> _partyMember1;
    std::shared_ptr<SpatialObject> _partyMember2;

    // END Party

    virtual void add(const std::shared_ptr<SpatialObject> &object);
    void determineObjectRoom(SpatialObject &object);
    void landObject(SpatialObject &object);

private:
    resource::GameVersion _version { resource::GameVersion::KotOR };
    render::GraphicsOptions _opts;
    CollisionDetector _collisionDetector;
    Pathfinder _pathfinder;
    ObjectSelector _objectSelector;
    ActionExecutor _actionExecutor;
    std::string _name;
    RoomMap _rooms;
    std::unique_ptr<resource::Visibility> _visibility;
    CameraStyle _cameraStyle;
    std::string _music;

    // Callbacks

    std::function<void(const std::string &, const std::string &)> _onModuleTransition;
    std::function<void(SpatialObject &, const std::string &)> _onStartDialog;
    std::function<void(CameraType)> _onCameraChanged;

    // END Callbacks

    std::shared_ptr<Creature> makeCharacter(const CreatureConfiguration &character, const std::string &tag, const glm::vec3 &position, float heading);
    bool getElevationAt(const glm::vec2 &position, Room *&room, float &z) const;
    void addPartyMemberPortrait(const std::shared_ptr<SpatialObject> &object, GuiContext &ctx);
    void addDebugInfo(const UpdateContext &updateCtx, GuiContext &guiCtx);
    glm::vec3 getSelectableScreenCoords(uint32_t objectId, const UpdateContext &ctx) const;
    bool findCameraObstacle(const glm::vec3 &origin, const glm::vec3 &dest, glm::vec3 &intersection) const;

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

    // Events

    bool handleKeyDown(const SDL_KeyboardEvent &event);

    // END Events
};

} // namespace game

} // namespace reone
