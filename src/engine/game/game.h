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

#include "../di/services/game.h"
#include "../graphics/eventhandler.h"
#include "../video/video.h"

#include "camera/camera.h"
#include "console.h"
#include "gui/chargen/chargen.h"
#include "gui/computer.h"
#include "gui/container.h"
#include "gui/conversation.h"
#include "gui/dialog.h"
#include "gui/hud.h"
#include "gui/ingame/ingame.h"
#include "gui/loadscreen.h"
#include "gui/mainmenu.h"
#include "gui/partyselect.h"
#include "gui/profileoverlay.h"
#include "gui/saveload.h"
#include "object/module.h"
#include "options.h"

namespace reone {

namespace resource {

class Resources;
class Strings;

}

namespace graphics {

class Context;
class Features;
class Fonts;
class Lips;
class Materials;
class Meshes;
class Models;
class PBRIBL;
class Shaders;
class Textures;
class Walkmeshes;
class Window;

}

namespace audio {

class AudioFiles;
class AudioPlayer;

}

namespace scene {

class SceneGraph;

}

namespace di {

class AudioServices;
class GraphicsServices;
class ResourceServices;
class SceneServices;
class ScriptServices;

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

/**
 * Abstract game.
 */
class Game : public graphics::IEventHandler, boost::noncopyable {
public:
    Game(
        GameID gameId,
        boost::filesystem::path path,
        Options options,
        di::ResourceServices &resource,
        di::GraphicsServices &graphics,
        di::AudioServices &audio,
        di::SceneServices &scene,
        di::ScriptServices &script);

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

    bool isLoadFromSaveGame() const;
    bool isPaused() const { return _paused; }
    bool isInConversation() const { return _screen == GameScreen::Conversation; }

    Camera *getActiveCamera() const;
    std::shared_ptr<Object> getObjectById(uint32_t id) const;

    GameID id() const { return _gameId; }
    const Options &options() const { return _options; }
    std::shared_ptr<Module> module() const { return _module; }
    HUD &hud() const { return *_hud; }
    CharacterGeneration &characterGeneration() { return *_charGen; }
    CameraType cameraType() const { return _cameraType; }
    Conversation &conversation() { return *_conversation; }
    const std::set<std::string> &moduleNames() const { return _moduleNames; }

    void setCursorType(CursorType type);
    void setLoadFromSaveGame(bool load);
    void setPaused(bool paused);
    void setRelativeMouseMode(bool relative);

    // Services

    ActionFactory &actionFactory() { return _game->actionFactory(); }
    Classes &classes() { return _game->classes(); }
    Combat &combat() { return _game->combat(); }
    EffectFactory &effectFactory() { return _game->effectFactory(); }
    Feats &feats() { return _game->feats(); }
    FootstepSounds &footstepSounds() { return _game->footstepSounds(); }
    GUISounds &guiSounds() { return _game->guiSounds(); }
    ObjectFactory &objectFactory() { return _game->objectFactory(); }
    Party &party() { return _game->party(); }
    Portraits &portraits() { return _game->portraits(); }
    Reputes &reputes() { return _game->reputes(); }
    ScriptRunner &scriptRunner() { return _game->scriptRunner(); }
    Skills &skills() { return _game->skills(); }
    SoundSets &soundSets() { return _game->soundSets(); }
    Surfaces &surfaces() { return _game->surfaces(); }

    audio::AudioFiles &audioFiles();
    audio::AudioPlayer &audioPlayer();
    graphics::Context &context();
    graphics::Features &features();
    graphics::Fonts &fonts();
    graphics::Lips &lips();
    graphics::Materials &materials();
    graphics::Meshes &meshes();
    graphics::Models &models();
    graphics::PBRIBL &pbrIbl();
    graphics::Shaders &shaders();
    graphics::Textures &textures();
    graphics::Walkmeshes &walkmeshes();
    graphics::Window &window();
    scene::SceneGraph &sceneGraph();
    resource::Resources &resources();
    resource::Strings &strings();

    // END Services

    // Game ID

    bool isKotOR() const;
    bool isTSL() const;

    // END Game ID

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

    // Game screens

    void openMainMenu();
    void openSaveLoad(SaveLoad::Mode mode);
    void openInGame();
    void openInGameMenu(InGameMenu::Tab tab);
    void openContainer(const std::shared_ptr<SpatialObject> &container);
    void openPartySelection(const PartySelection::Context &ctx);
    void openLevelUp();

    void startCharacterGeneration();
    void startDialog(const std::shared_ptr<SpatialObject> &owner, const std::string &resRef);

    // END Game screens

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
    const std::string kDataDirectoryName { "data" };

    GameID _gameId;
    boost::filesystem::path _path;
    di::ResourceServices &_resource;
    std::string _mainMenuMusicResRef;
    std::string _charGenMusicResRef;
    std::string _charGenLoadScreenResRef;

    // GUI colors

    glm::vec3 _guiColorBase { 0.0f };
    glm::vec3 _guiColorHilight { 0.0f };
    glm::vec3 _guiColorDisabled { 0.0f };

    // END GUI colors

    virtual void initResourceProviders() = 0;

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

    Options _options;

    GameScreen _screen { GameScreen::MainMenu };
    uint32_t _ticks { 0 };
    bool _quit { false };
    std::shared_ptr<video::Video> _video;
    CursorType _cursorType { CursorType::None };
    float _gameSpeed { 1.0f };
    bool _loadFromSaveGame { false };
    CameraType _cameraType { CameraType::ThirdPerson };
    bool _paused { false };
    Conversation *_conversation { nullptr }; /**< pointer to either DialogGUI or ComputerGUI  */
    std::set<std::string> _moduleNames;

    // Services

    di::GraphicsServices &_graphics;
    di::AudioServices &_audio;
    di::SceneServices &_scene;
    di::ScriptServices &_script;

    std::unique_ptr<di::GameServices> _game;

    // END Services

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

    std::unique_ptr<Console> _console;
    std::unique_ptr<ProfileOverlay> _profileOverlay;

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

    void init();
    void deinit();

    void update();

    void loadNextModule();
    float measureFrameTime();
    void playMusic(const std::string &resRef);
    void runMainLoop();
    void toggleInGameCameraType();
    void stopMovement();
    void changeScreen(GameScreen screen);

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);

    void updateCamera(float dt);
    void updateVideo(float dt);
    void updateMusic();
    void updateSceneGraph(float dt);

    gui::GUI *getScreenGUI() const;

    // Resource management

    void loadModuleNames();
    void loadModuleResources(const std::string &moduleName);

    // END Resource management

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

    // Save games

    std::shared_ptr<resource::GffStruct> getPartyMemberNFOStruct(int index) const;

    // END Save games

    // Helper methods

    void withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block);

    // END Helper methods
};

} // namespace game

} // namespace reone
