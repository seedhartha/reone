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

#include "../../audio/soundhandle.h"
#include "../../graphics/eventhandler.h"
#include "../../video/video.h"

#include "action/factory.h"
#include "camera/camera.h"
#include "combat.h"
#include "console.h"
#include "effect/factory.h"
#include "gui/loadscreen.h"
#include "object/factory.h"
#include "object/module.h"
#include "options.h"
#include "party.h"
#include "profileoverlay.h"
#include "scenemanager.h"
#include "script/routine/routines.h"
#include "script/runner.h"

namespace reone {

namespace gui {

class GUI;

}

namespace game {

constexpr char kKeyFilename[] = "chitin.key";
constexpr char kTexturePackDirectoryName[] = "texturepacks";
constexpr char kGUITexturePackFilename[] = "swpc_tex_gui.erf";
constexpr char kTexturePackFilename[] = "swpc_tex_tpa.erf";
constexpr char kMusicDirectoryName[] = "streammusic";
constexpr char kSoundsDirectoryName[] = "streamsounds";
constexpr char kLipsDirectoryName[] = "lips";
constexpr char kOverrideDirectoryName[] = "override";

struct Services;

/**
 * Abstract game.
 */
class Game : public graphics::IEventHandler, boost::noncopyable {
public:
    Game(
        bool tsl,
        boost::filesystem::path path,
        Options options,
        Services &services);

    virtual ~Game() {
        deinit();
    }

    virtual void initResourceProviders() = 0;

    void init();

    /**
     * Initializes the engine, run the main game loop and clean up on exit.
     *
     * @return the exit code
     */
    int run();

    /**
     * Requests termination of the main game loop.
     */
    void quit();

    void playVideo(const std::string &name);

    bool isTSL() const { return _tsl; }
    bool isLoadFromSaveGame() const;
    bool isPaused() const { return _paused; }

    Camera *getActiveCamera() const;
    std::shared_ptr<Object> getObjectById(uint32_t id) const;

    const Options &options() const { return _options; }
    Party &party() { return _party; }
    Combat &combat() { return _combat; }
    ActionFactory &actionFactory() { return _actionFactory; }
    EffectFactory &effectFactory() { return _effectFactory; }
    ObjectFactory &objectFactory() { return _objectFactory; }
    SceneManager &sceneManager() { return _sceneManager; }
    ScriptRunner &scriptRunner() { return _scriptRunner; }

    std::shared_ptr<Module> module() const { return _module; }
    CameraType cameraType() const { return _cameraType; }
    const std::set<std::string> &moduleNames() const { return _moduleNames; }

    void setCursorType(CursorType type);
    void setLoadFromSaveGame(bool load);
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
     * Loads a module with the specified name and entry point.
     *
     * @param name name of the module to load
     * @param entry tag of the waypoint to spawn at, or empty string to use the default entry point
     */
    void loadModule(const std::string &name, std::string entry = "");

    /**
     * Schedules transition to the specified module with the specified entry point.
     *
     * @param name name of the module to load
     * @param entry tag of the waypoint to spawn at
     */
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

    // Saved games

    void saveToFile(const boost::filesystem::path &path);
    void loadFromFile(const boost::filesystem::path &path);

    // END Saved games

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

    bool _tsl;
    boost::filesystem::path _path;
    Options _options;
    Services &_services;

    GameScreen _screen {GameScreen::None};
    std::string _mainMenuMusicResRef;
    std::string _charGenMusicResRef;
    std::string _charGenLoadScreenResRef;

    uint32_t _ticks {0};
    bool _quit {false};
    std::shared_ptr<video::Video> _video;
    CursorType _cursorType {CursorType::None};
    float _gameSpeed {1.0f};
    bool _loadFromSaveGame {false};
    CameraType _cameraType {CameraType::ThirdPerson};
    bool _paused {false};
    std::set<std::string> _moduleNames;

    // Services

    Party _party;
    Combat _combat;
    ActionFactory _actionFactory;
    EffectFactory _effectFactory;
    ObjectFactory _objectFactory;
    Routines _routines;
    SceneManager _sceneManager;
    ScriptRunner _scriptRunner;

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

    std::unique_ptr<ILoadingScreen> _loadScreen;

    Console _console;
    ProfileOverlay _profileOverlay;

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

    // END Globals/locals

    void deinit();

    virtual void start() = 0;

    void stopMovement();

    virtual void loadModuleNames() = 0;
    virtual void loadModuleResources(const std::string &moduleName) = 0;

    void runMainLoop();
    void update();

    float measureFrameTime();

    void updateMusic();
    void updateVideo(float dt);
    void updateCamera(float dt);
    void updateSceneGraph(float dt);

    void loadDefaultParty();
    void loadNextModule();
    void playMusic(const std::string &resRef);
    void toggleInGameCameraType();

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);

    virtual void initRoutines() = 0;
    virtual void onModuleSelected(const std::string &name) = 0;
    virtual void drawHUD() = 0;

    virtual void getDefaultPartyMembers(std::string &member1, std::string &member2, std::string &member3) const;
    virtual gui::GUI *getScreenGUI() const = 0;
    virtual CameraType getConversationCamera(int &cameraId) const = 0;

    std::shared_ptr<resource::GffStruct> getPartyMemberNFOStruct(int index) const;

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
