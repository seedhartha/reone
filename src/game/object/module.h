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

#include <functional>

#include "SDL2/SDL_events.h"

#include "../../system/net/types.h"

#include "../player.h"

#include "area.h"

namespace reone {

namespace game {

struct ModuleInfo {
    std::string entryArea;
    glm::vec3 entryPosition { 0.0f };
    float entryHeading { 0.0f };
};

class ObjectFactory;

class Module : public Object {
public:
    Module(uint32_t id, resource::GameVersion version, ObjectFactory *objectFactory, scene::SceneGraph *sceneGraph, const render::GraphicsOptions &opts);

    // Loading
    void load(const std::string &name, const resource::GffStruct &ifo);
    void loadParty(const PartyConfiguration &party, const std::string &entry = "");

    bool handle(const SDL_Event &event);
    void update(float dt, GuiContext &guiCtx);
    void saveTo(GameState &state) const;

    // Getters
    const std::string &name() const;
    bool loaded() const;
    const ModuleInfo &info() const;
    std::shared_ptr<Area> area() const;

    // Callbacks
    void setOnModuleTransition(const std::function<void(const std::string &, const std::string &)> &fn);
    void setStartDialog(const std::function<void(SpatialObject &, const std::string &)> &fn);
    void setOpenContainer(const std::function<void(SpatialObject *)> &fn);

protected:
    resource::GameVersion _version { resource::GameVersion::KotOR };
    ModuleInfo _info;

private:
    ObjectFactory *_objectFactory { nullptr };
    scene::SceneGraph *_sceneGraph { nullptr };
    std::string _name;
    bool _loaded { false };
    render::GraphicsOptions _opts;
    std::shared_ptr<Area> _area;
    PartyConfiguration _party;
    std::unique_ptr<Player> _player;

    // Callbacks
    std::function<void(const std::string &, const std::string &)> _onModuleTransition;
    std::function<void(SpatialObject &, const std::string &)> _startDialog;
    std::function<void(SpatialObject *)> _openContainer;

    void cycleDebugMode(bool forward);
    void getEntryPoint(const std::string &waypoint, glm::vec3 &position, float &heading) const;

    // Loading

    void loadInfo(const resource::GffStruct &ifo);
    void loadArea(const resource::GffStruct &ifo);
    void loadPlayer();

    // END Loading

    // Events

    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    bool handleMouseButtonUp(const SDL_MouseButtonEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);

    // END Events
};

} // namespace game

} // namespace reone
