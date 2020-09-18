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

#include "../gui/types.h"
#include "../net/types.h"
#include "../render/camera/camera.h"
#include "../render/scene/scenegraph.h"
#include "../render/types.h"
#include "../resources/types.h"
#include "../script/variable.h"

#include "navmesh.h"
#include "object/creature.h"
#include "object/door.h"
#include "object/placeable.h"
#include "object/trigger.h"
#include "object/waypoint.h"
#include "objectcontainer.h"
#include "room.h"
#include "types.h"

namespace reone {

namespace game {

typedef std::vector<std::shared_ptr<SpatialObject>> ObjectList;

class ObjectFactory;

class Area : public Object, public ObjectContainer {
public:
    Area(uint32_t id, resources::GameVersion version, ObjectFactory *objectFactory);

    void load(const std::string &name, const resources::GffStruct &are, const resources::GffStruct &git);
    void loadParty(const PartyConfiguration &party, const glm::vec3 &position, float heading);
    void runOnEnterScript();

    bool handle(const SDL_Event &event);
    void update(const UpdateContext &updateCtx, GuiContext &guiCtx);
    bool moveCreatureTowards(Creature &creature, const glm::vec3 &point, float dt);
    void updateTriggers(const Creature &creature);
    void render() const;

    void delayCommand(uint32_t timestamp, const script::ExecutionContext &ctx);
    int eventUserDefined(int eventNumber);
    void signalEvent(int eventId);

    // Load/save
    void saveTo(GameState &state) const;
    void loadState(const GameState &state);

    // Object search
    bool findObstacleByWalkmesh(const glm::vec3 &from, const glm::vec3 &to, int mask, glm::vec3 &intersection, SpatialObject **obstacle) const;
    bool findObstacleByAABB(const glm::vec3 &from, const glm::vec3 &to, int mask, const SpatialObject *except, SpatialObject **obstacle) const;

    // General getters
    const render::CameraStyle &cameraStyle() const;
    const std::string &music() const;

    // Party getters
    std::shared_ptr<SpatialObject> player() const;
    std::shared_ptr<SpatialObject> partyLeader() const;
    std::shared_ptr<SpatialObject> partyMember1() const;
    std::shared_ptr<SpatialObject> partyMember2() const;

    // Setters
    void setDebugMode(DebugMode mode);

    // Callbacks
    void setOnModuleTransition(const std::function<void(const std::string &, const std::string &)> &fn);
    void setOnPlayerChanged(const std::function<void()> &fn);
    void setOnStartDialog(const std::function<void(const Object &, const std::string &)> &fn);

protected:
    ObjectFactory *_objectFactory { nullptr };
    bool _scriptsEnabled { true };
    std::function<void()> _onPlayerChanged;

    // Party
    std::shared_ptr<SpatialObject> _player;
    std::shared_ptr<SpatialObject> _partyLeader;
    std::shared_ptr<SpatialObject> _partyMember1;
    std::shared_ptr<SpatialObject> _partyMember2;

    void add(const std::shared_ptr<SpatialObject> &object) override;
    void landObject(SpatialObject &object);

    virtual void updateCreature(Creature &creature, float dt);

private:
    enum class ScriptType {
        OnEnter,
        OnExit,
        OnHeartbeat,
        OnUserDefined
    };

    struct DelayedCommand {
        uint32_t timestamp { 0 };
        script::ExecutionContext context;
        bool executed { false };
    };

    struct UserDefinedEvent {
        int eventNumber { 0 };
    };

    resources::GameVersion _version { resources::GameVersion::KotOR };
    std::string _name;
    std::vector<std::shared_ptr<Room>> _rooms;
    std::unique_ptr<resources::Visibility> _visibility;
    render::CameraStyle _cameraStyle;
    std::string _music;
    std::unique_ptr<NavMesh> _navMesh;
    DebugMode _debugMode { DebugMode::None };
    std::map<ScriptType, std::string> _scripts;
    std::list<DelayedCommand> _delayed;
    std::map<int, UserDefinedEvent> _events;
    int _eventCounter { 0 };
    render::SceneGraph _sceneGraph;

    // Callbacks
    std::function<void(const std::string &, const std::string &)> _onModuleTransition;
    std::function<void(const Object &, const std::string &)> _onStartDialog;

    std::shared_ptr<Creature> makeCharacter(const CreatureConfiguration &character, const std::string &tag, const glm::vec3 &position, float heading);
    void updateDelayedCommands();
    bool navigateCreature(Creature &creature, const glm::vec3 &dest, float distance, float dt);
    void advanceCreatureOnPath(Creature &creature, float dt);
    void selectNextPathPoint(Creature::Path &path);
    void updateCreaturePath(Creature &creature, const glm::vec3 &dest);
    bool findElevationAt(const glm::vec3 &position, float &z) const;

    // Loading
    void loadProperties(const resources::GffStruct &gffs);
    void loadLayout();
    void loadVisibility();
    void loadCameraStyle(const resources::GffStruct &are);
    void loadScripts(const resources::GffStruct &are);
};

} // namespace game

} // namespace reone
