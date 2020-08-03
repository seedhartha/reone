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

#include "../gui/types.h"
#include "../net/types.h"
#include "../render/camera/camera.h"
#include "../render/types.h"
#include "../resources/types.h"

#include "navmesh.h"
#include "multiplayer/command.h"
#include "object/creature.h"
#include "object/door.h"
#include "object/placeable.h"
#include "object/trigger.h"
#include "object/waypoint.h"
#include "room.h"
#include "types.h"

namespace reone {

namespace game {

typedef std::vector<std::shared_ptr<Object>> ObjectList;

/*
 * Game object container. Building block of a module.
 *
 * @see reone::game::Module
 * @see reone::game::Object
 */
class Area {
public:
    Area(resources::GameVersion version, const std::string &name);

    void load(const resources::GffStruct &are, const resources::GffStruct &git);
    void loadParty(const glm::vec3 &position, float heading);
    bool handle(const SDL_Event &event);
    void update(const UpdateContext &updateCtx, gui::GuiContext &guiCtx);
    bool moveCreatureTowards(Creature &creature, const glm::vec3 &point, float dt);
    void updateTriggers(const Creature &creature);
    void initGL();
    void render() const;
    void saveTo(GameState &state) const;
    void loadState(const GameState &state);

    // Object search
    std::shared_ptr<Object> find(uint32_t id) const;
    std::shared_ptr<Object> find(const std::string &tag) const;
    std::shared_ptr<Object> find(uint32_t id, ObjectType type) const;
    std::shared_ptr<Object> find(const std::string &tag, ObjectType type) const;
    bool findObstacleByWalkmesh(const glm::vec3 &from, const glm::vec3 &to, int mask, glm::vec3 &intersection, Object **obstacle) const;
    bool findObstacleByAABB(const glm::vec3 &from, const glm::vec3 &to, int mask, const Object *except, Object **obstacle) const;

    // Setters
    void setDebugMode(gui::DebugMode mode);

    // General getters
    const render::CameraStyle &cameraStyle() const;
    const std::string &music() const;

    // Party getters
    std::shared_ptr<Object> player() const;
    std::shared_ptr<Object> partyLeader() const;
    std::shared_ptr<Object> partyMember1() const;
    std::shared_ptr<Object> partyMember2() const;

    // Callbacks
    void setOnModuleTransition(const std::function<void(const std::string &, const std::string &)> &fn);
    void setOnPlayerChanged(const std::function<void()> &fn);

protected:
    uint32_t _idCounter { 0 };
    std::map<ObjectType, ObjectList> _objects;
    std::function<void()> _onPlayerChanged;

    // Party
    std::shared_ptr<Object> _player;
    std::shared_ptr<Object> _partyLeader;
    std::shared_ptr<Object> _partyMember1;
    std::shared_ptr<Object> _partyMember2;

    void landObject(Object &object);

    virtual std::shared_ptr<Creature> makeCreature();
    virtual std::shared_ptr<Door> makeDoor();
    virtual std::shared_ptr<Placeable> makePlaceable();
    virtual std::shared_ptr<Waypoint> makeWaypoint();
    virtual std::shared_ptr<Trigger> makeTrigger();
    virtual void updateCreature(Creature &creature, float dt);

private:
    enum class RenderListName {
        Opaque,
        Transparent
    };

    resources::GameVersion _version { resources::GameVersion::KotOR };
    std::string _name;
    std::vector<std::shared_ptr<Room>> _rooms;
    std::unique_ptr<resources::Visibility> _visibility;
    render::CameraStyle _cameraStyle;
    std::string _music;
    std::map<RenderListName, render::RenderList> _renderLists;
    std::unique_ptr<NavMesh> _navMesh;
    gui::DebugMode _debugMode { gui::DebugMode::None };

    // Callbacks
    std::function<void(const std::string &, const std::string &)> _onModuleTransition;

    void navigateCreature(Creature &creature, const glm::vec3 &dest, float distance, float dt);
    void advanceCreatureOnPath(Creature &creature, float dt);
    void selectNextPathPoint(Creature::Path &path);
    void updateCreaturePath(Creature &creature, const glm::vec3 &dest);
    void addToDebugContext(const render::RenderListItem &item, const UpdateContext &updateCtx, gui::DebugContext &debugCtx) const;
    void addToDebugContext(const Object &object, const UpdateContext &updateCtx, gui::DebugContext &debugCtx) const;
    bool findElevationAt(const glm::vec3 &position, float &z) const;

    // Loading
    void loadProperties(const resources::GffStruct &gffs);
    void loadLayout();
    void loadVisibility();
    void loadCameraStyle(const resources::GffStruct &are);
};

} // namespace game

} // namespace reone
