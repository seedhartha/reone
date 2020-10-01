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

#include "../net/types.h"

#include "area.h"
#include "camera/firstperson.h"
#include "camera/thirdperson.h"
#include "player.h"

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
    Module(uint32_t id, resources::GameVersion version, ObjectFactory *objectFactory, render::SceneGraph *sceneGraph, const render::GraphicsOptions &opts);

    // Loading
    void load(const std::string &name, const resources::GffStruct &ifo);
    void loadParty(const PartyConfiguration &party, const std::string &entry = "");

    bool handle(const SDL_Event &event);
    void update(float dt, GuiContext &guiCtx);
    void update3rdPersonCameraTarget();
    void update3rdPersonCameraHeading();
    void saveTo(GameState &state) const;

    // Getters
    const std::string &name() const;
    bool loaded() const;
    std::shared_ptr<Camera> getCamera() const;
    const ModuleInfo &info() const;
    std::shared_ptr<Area> area() const;
    CameraType cameraType() const;

    // Callbacks
    void setOnCameraChanged(const std::function<void(CameraType)> &fn);
    void setOnModuleTransition(const std::function<void(const std::string &, const std::string &)> &fn);
    void setStartDialog(const std::function<void(const Object &, const std::string &)> &fn);
    void setOpenContainer(const std::function<void(SpatialObject *)> &fn);

protected:
    resources::GameVersion _version { resources::GameVersion::KotOR };
    ModuleInfo _info;

private:
    ObjectFactory *_objectFactory { nullptr };
    render::SceneGraph *_sceneGraph { nullptr };
    std::string _name;
    bool _loaded { false };
    render::GraphicsOptions _opts;
    float _cameraAspect { 0.0f };
    std::shared_ptr<Area> _area;
    PartyConfiguration _party;
    CameraType _cameraType { CameraType::FirstPerson };
    std::shared_ptr<FirstPersonCamera> _firstPersonCamera;
    std::shared_ptr<ThirdPersonCamera> _thirdPersonCamera;
    std::unique_ptr<Player> _player;

    // Callbacks
    std::function<void(CameraType)> _onCameraChanged;
    std::function<void(const std::string &, const std::string &)> _onModuleTransition;
    std::function<void(const Object &, const std::string &)> _startDialog;
    std::function<void(SpatialObject *)> _openContainer;

    void toggleCameraType();
    void cycleDebugMode(bool forward);
    bool findObstacle(const glm::vec3 &from, const glm::vec3 &to, glm::vec3 &intersection) const;
    void getEntryPoint(const std::string &waypoint, glm::vec3 &position, float &heading) const;
    void switchTo3rdPersonCamera();
    SpatialObject *getObjectAt(int x, int y) const;

    // Loading

    void loadInfo(const resources::GffStruct &ifo);
    void loadArea(const resources::GffStruct &ifo);
    void loadCameras();
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
