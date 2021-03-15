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

#include <cstdint>
#include <functional>
#include <map>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include "SDL2/SDL_events.h"

#include "../resource/types.h"
#include "../scene/pipeline/world.h"
#include "../scene/scenegraph.h"

#include "console.h"
#include "gui/chargen/chargen.h"
#include "gui/container.h"
#include "gui/computer.h"
#include "gui/dialog.h"
#include "gui/hud.h"
#include "gui/ingame/ingame.h"
#include "gui/loadscreen.h"
#include "gui/mainmenu.h"
#include "gui/partyselect.h"
#include "gui/profileoverlay.h"
#include "gui/saveload.h"
#include "object/module.h"
#include "object/objectfactory.h"
#include "object/spatial.h"
#include "options.h"
#include "party.h"
#include "script/runner.h"
#include "types.h"

namespace reone {

namespace audio {

class SoundHandle;

}

namespace video {

class Video;

}

namespace game {

/**
 * Entry point for the game logic: contains the main game loop and delegates
 * work to the instances of Module and GUI. Serves as a Service Locator.
 *
 * @see game::Module
 * @see gui::GUI
 */
class Game : public render::IEventHandler, boost::noncopyable {
public:
    Game(const boost::filesystem::path &path, const Options &opts);
    virtual ~Game() = default;

    /**
     * Initialize the engine, run the main game loop and clean up on exit.
     *
     * @return the exit code
     */
    int run();

    /**
     * Request termination of the main game loop.
     */
    void quit();

    void playVideo(const std::string &name);

    bool isLoadFromSaveGame() const;
    bool isPaused() const { return _paused; }
    bool isInConversation() const { return _screen == GameScreen::Conversation; }

    Camera *getActiveCamera() const;
    int getRunScriptVar() const;
    std::shared_ptr<Object> getObjectById(uint32_t id) const;

    resource::GameID gameId() const { return _gameId; }
    const Options &options() const { return _options; }
    scene::SceneGraph &sceneGraph() { return _sceneGraph; }
    ObjectFactory &objectFactory() { return *_objectFactory; }
    std::shared_ptr<Module> module() const { return _module; }
    HUD &hud() const { return *_hud; }
    Party &party() { return _party; }
    CharacterGeneration &characterGeneration() { return *_charGen; }
    CameraType cameraType() const { return _cameraType; }
    ScriptRunner &scriptRunner() { return _scriptRunner; }
    Conversation &conversation() { return *_conversation; }

    void setCursorType(CursorType type);
    void setLoadFromSaveGame(bool load);
    void setRunScriptVar(int var);
    void setPaused(bool paused);
    void setRelativeMouseMode(bool relative);

    // Module Loading

    /**
     * Load a module with the specified name and entry point.
     *
     * @param name name of the module to load
     * @param entry tag of the waypoint to spawn at, or empty string to use the default entry point
     */
    void loadModule(const std::string &name, std::string entry = "");

    /**
     * Schedule transition to the specified module with the specified entry point.
     *
     * @param name name of the module to load
     * @param entry tag of the waypoint to spawn at
     */
    void scheduleModuleTransition(const std::string &moduleName, const std::string &entry);

    // END Module Loading

    // Game Screens

    void openMainMenu();
    void openSaveLoad(SaveLoad::Mode mode);
    void openInGame();
    void openInGameMenu(InGameMenu::Tab tab);
    void openContainer(const std::shared_ptr<SpatialObject> &container);
    void openPartySelection(const PartySelection::Context &ctx);
    void openLevelUp();

    void startCharacterGeneration();
    void startDialog(const std::shared_ptr<SpatialObject> &owner, const std::string &resRef);

    // END Game Screens

    // Globals/locals

    bool getGlobalBoolean(const std::string &name) const;
    bool getLocalBoolean(uint32_t objectId, int index) const;
    int getGlobalNumber(const std::string &name) const;
    int getLocalNumber(uint32_t objectId, int index) const;
    std::shared_ptr<Location> getGlobalLocation(const std::string &name) const;
    std::string getGlobalString(const std::string &name) const;

    void setGlobalBoolean(const std::string &name, bool value);
    void setGlobalLocation(const std::string &name, const std::shared_ptr<Location> &location);
    void setGlobalNumber(const std::string &name, int value);
    void setGlobalString(const std::string &name, const std::string &value);
    void setLocalBoolean(uint32_t objectId, int index, bool value);
    void setLocalNumber(uint32_t objectId, int index, int value);

    // END Globals/locals

    // IEventHandler

    bool handle(const SDL_Event &event) override;

    // END IEventHandler

protected:
    Options _options;

    /**
     * Initializes the engine subsytems.
     */
    virtual void init();

    virtual void update();

private:
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
    scene::SceneGraph _sceneGraph;
    scene::WorldRenderPipeline _worldPipeline;
    Console _console;
    resource::GameID _gameId { resource::GameID::KotOR };
    std::unique_ptr<ObjectFactory> _objectFactory;
    GameScreen _screen { GameScreen::MainMenu };
    Party _party;
    uint32_t _ticks { 0 };
    bool _quit { false };
    std::shared_ptr<video::Video> _video;
    CursorType _cursorType { CursorType::None };
    float _gameSpeed { 1.0f };
    bool _loadFromSaveGame { false };
    CameraType _cameraType { CameraType::ThirdPerson };
    int _runScriptVar { -1 };
    ScriptRunner _scriptRunner;
    bool _paused { false };
    Conversation *_conversation { nullptr }; /**< pointer to either DialogGUI or ComputerGUI  */
    ProfileOverlay _profileOverlay;

    // Modules

    std::string _nextModule;
    std::string _nextEntry;
    std::shared_ptr<Module> _module;
    std::map<std::string, std::shared_ptr<Module>> _loadedModules;

    // END Modules

    // GUI

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<LoadingScreen> _loadScreen;
    std::unique_ptr<CharacterGeneration> _charGen;
    std::unique_ptr<HUD> _hud;
    std::unique_ptr<InGameMenu> _inGame;
    std::unique_ptr<DialogGUI> _dialog;
    std::unique_ptr<ComputerGUI> _computer;
    std::unique_ptr<Container> _container;
    std::unique_ptr<PartySelection> _partySelect;
    std::unique_ptr<SaveLoad> _saveLoad;

    // END GUI

    // Audio

    std::string _musicResRef;
    std::shared_ptr<audio::SoundHandle> _music;
    std::shared_ptr<audio::SoundHandle> _movieAudio;

    // END Audio

    // Globals/locals

    std::map<std::string, std::string> _globalStrings;
    std::map<std::string, bool> _globalBooleans;
    std::map<std::string, int> _globalNumbers;
    std::map<std::string, std::shared_ptr<Location>> _globalLocations;
    std::map<uint32_t, std::map<int, bool>> _localBooleans;
    std::map<uint32_t, std::map<int, int>> _localNumbers;

    // END Globals/locals

    /**
    * Releases the engine subsystems.
    */
    void deinit();

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);
    void loadNextModule();
    float measureFrameTime();
    void playMusic(const std::string &resRef);
    void runMainLoop();
    void toggleInGameCameraType();
    void updateCamera(float dt);
    void stopMovement();
    void changeScreen(GameScreen screen);
    void updateVideo(float dt);
    void updateMusic();
    void registerModelLoaders();

    std::string getMainMenuMusic() const;
    std::string getCharacterGenerationMusic() const;
    gui::GUI *getScreenGUI() const;

    // Loading

    void loadCharacterGeneration();
    void loadContainer();
    void loadDialog();
    void loadComputer();
    void loadHUD();
    void loadInGame();
    void loadLoadingScreen();
    void loadMainMenu();
    void loadPartySelection();
    void loadSaveLoad();

    // END Loading

    // Rendering

    void drawAll();
    void drawWorld();
    void drawGUI();

    // END Rendering

    // Helper methods

    void withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block);

    // END Helper methods
};

} // namespace game

} // namespace reone
