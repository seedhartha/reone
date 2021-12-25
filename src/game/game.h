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

#include "../../audio/source.h"
#include "../../graphics/eventhandler.h"
#include "../../movie/movie.h"
#include "../../script/routines.h"

#include "action/factory.h"
#include "camera.h"
#include "combat.h"
#include "effect/factory.h"
#include "gui/console.h"
#include "gui/loadscreen.h"
#include "gui/map.h"
#include "gui/profileoverlay.h"
#include "object/factory.h"
#include "object/module.h"
#include "options.h"
#include "party.h"
#include "script/runner.h"
#include "services.h"

namespace reone {

namespace gui {

class GUI;

}

namespace game {

class Game : public graphics::IEventHandler, boost::noncopyable {
public:
    Game(
        boost::filesystem::path path,
        Options options,
        Services &services) :
        _path(std::move(path)),
        _options(std::move(options)),
        _services(services),
        _party(*this),
        _combat(*this, services),
        _actionFactory(*this, services),
        _objectFactory(*this, services) {
    }

    virtual ~Game() = default;

    virtual void initResourceProviders() = 0;

    virtual void init();

    /**
     * @return exit code
     */
    int run();

    void quit();

    void playVideo(const std::string &name);

    bool isPaused() const { return _paused; }

    Camera *getActiveCamera() const;
    std::shared_ptr<Object> getObjectById(uint32_t id) const;

    const Options &options() const { return _options; }
    Party &party() { return _party; }
    Combat &combat() { return _combat; }
    ActionFactory &actionFactory() { return _actionFactory; }
    EffectFactory &effectFactory() { return _effectFactory; }
    ObjectFactory &objectFactory() { return _objectFactory; }
    ScriptRunner &scriptRunner() { return *_scriptRunner; }
    IMap &map() { return *_map; }

    std::shared_ptr<Module> module() const { return _module; }
    CameraType cameraType() const { return _cameraType; }
    const std::set<std::string> &moduleNames() const { return _moduleNames; }

    void setCursorType(CursorType type);
    void setPaused(bool paused);
    void setRelativeMouseMode(bool relative);

    virtual void openMainMenu() = 0;
    virtual void openContainer(const std::shared_ptr<SpatialObject> &container) = 0;
    virtual void openPartySelection(const PartySelectionContext &ctx) = 0;
    virtual void openSaveLoad(SaveLoadMode mode) = 0;
    virtual void startDialog(const std::shared_ptr<SpatialObject> &owner, const std::string &resRef) = 0;
    virtual void pauseConversation() = 0;
    virtual void resumeConversation() = 0;

    // Module loading

    /**
     * @param entry waypoint tag to spawn at, or empty string to spawn at default location
     */
    void loadModule(const std::string &name, std::string entry = "");

    void scheduleModuleTransition(const std::string &moduleName, const std::string &entry);

    // END Module loading

    // Globals/locals

    bool getGlobalBoolean(const std::string &name) const;
    int getGlobalNumber(const std::string &name) const;
    std::shared_ptr<Location> getGlobalLocation(const std::string &name) const;
    std::string getGlobalString(const std::string &name) const;

    void setGlobalBoolean(const std::string &name, bool value);
    void setGlobalLocation(const std::string &name, const std::shared_ptr<Location> &location);
    void setGlobalNumber(const std::string &name, int value);
    void setGlobalString(const std::string &name, const std::string &value);

    // END Globals/locals

    // GUI colors

    const glm::vec3 &getGUIColorBase() const { return _guiColorBase; }
    const glm::vec3 &getGUIColorHilight() const { return _guiColorHilight; }
    const glm::vec3 &getGUIColorDisabled() const { return _guiColorDisabled; }

    // END GUI colors

    // IEventHandler

    bool handle(const SDL_Event &event) override;

    // END IEventHandler

protected:
    enum class GameScreen {
        None,
        MainMenu,
        Loading,
        CharacterGeneration,
        InGame,
        InGameMenu,
        Conversation,
        Container,
        PartySelection,
        SaveLoad
    };

    boost::filesystem::path _path;
    Options _options;
    Services &_services;

    GameScreen _screen {GameScreen::None};
    std::string _mainMenuMusicResRef;
    std::string _charGenMusicResRef;
    std::string _charGenLoadScreenResRef;

    uint32_t _ticks {0};
    bool _quit {false};
    std::shared_ptr<movie::Movie> _movie;
    CursorType _cursorType {CursorType::None};
    float _gameSpeed {1.0f};
    CameraType _cameraType {CameraType::ThirdPerson};
    bool _paused {false};
    std::set<std::string> _moduleNames;

    // Services

    Party _party;
    Combat _combat;
    ActionFactory _actionFactory;
    EffectFactory _effectFactory;
    ObjectFactory _objectFactory;

    std::unique_ptr<script::IRoutines> _routines;
    std::unique_ptr<ScriptRunner> _scriptRunner;

    // END Services

    // GUI colors

    glm::vec3 _guiColorBase {0.0f};
    glm::vec3 _guiColorHilight {0.0f};
    glm::vec3 _guiColorDisabled {0.0f};

    // END GUI colors

    // Modules

    std::string _nextModule;
    std::string _nextEntry;
    std::shared_ptr<Module> _module;
    std::map<std::string, std::shared_ptr<Module>> _loadedModules;

    // END Modules

    // GUI

    std::unique_ptr<IMap> _map;
    std::unique_ptr<IConsole> _console;
    std::unique_ptr<ILoadingScreen> _loadScreen;
    std::unique_ptr<IProfileOverlay> _profileOverlay;

    // END GUI

    // Audio

    std::string _musicResRef;
    std::shared_ptr<audio::AudioSource> _music;

    // END Audio

    // Globals/locals

    std::map<std::string, std::string> _globalStrings;
    std::map<std::string, bool> _globalBooleans;
    std::map<std::string, int> _globalNumbers;
    std::map<std::string, std::shared_ptr<Location>> _globalLocations;

    // END Globals/locals

    virtual void start() = 0;

    void stopMovement();

    virtual void loadModuleNames() = 0;
    virtual void loadModuleResources(const std::string &moduleName) = 0;

    void runMainLoop();
    void update();

    float measureFrameTime();

    void updateMusic();
    void updateMovie(float dt);
    void updateCamera(float dt);
    void updateSceneGraph(float dt);

    void loadDefaultParty();
    void loadNextModule();
    void playMusic(const std::string &resRef);
    void toggleInGameCameraType();

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);

    virtual void onModuleSelected(const std::string &name) = 0;
    virtual void drawHUD() = 0;

    virtual void getDefaultPartyMembers(std::string &member1, std::string &member2, std::string &member3) const = 0;
    virtual gui::GUI *getScreenGUI() const = 0;
    virtual CameraType getConversationCamera(int &cameraId) const = 0;

    // Rendering

    void drawAll();
    void drawWorld();
    void drawGUI();

    // END Rendering

    // GUI

    virtual void loadInGameMenus() = 0;
    virtual void loadLoadingScreen() = 0;

    virtual void openInGame() = 0;
    virtual void changeScreen(GameScreen screen) = 0;

    void withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block);

    // END GUI
};

} // namespace game

} // namespace reone
