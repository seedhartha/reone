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

#include "camera/camera.h"
#include "console.h"
#include "gui/loadscreen.h"
#include "object/module.h"
#include "options.h"
#include "profileoverlay.h"

namespace reone {

namespace resource {

class Resources;
class Strings;

} // namespace resource

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

} // namespace graphics

namespace audio {

class AudioFiles;
class AudioPlayer;

} // namespace audio

namespace scene {

class SceneGraph;
class WorldRenderPipeline;

} // namespace scene

namespace gui {

class GUI;

}

namespace script {

class Scripts;

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

class ActionFactory;
class Classes;
class Combat;
class Cursors;
class EffectFactory;
class Feats;
class FootstepSounds;
class GUISounds;
class ObjectFactory;
class Party;
class Portraits;
class Reputes;
class ScriptRunner;
class Skills;
class SoundSets;
class Surfaces;

/**
 * Abstract game.
 */
class Game : public graphics::IEventHandler, boost::noncopyable {
public:
    Game(
        bool tsl,
        boost::filesystem::path path,
        Options options,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        Cursors &cursors,
        EffectFactory &effectFactory,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        Skills &skills,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Fonts &fonts,
        graphics::Lips &lips,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::PBRIBL &pbrIbl,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        graphics::Window &window,
        scene::SceneGraph &sceneGraph,
        scene::WorldRenderPipeline &worldRenderPipeline,
        script::Scripts &scripts,
        resource::Resources &resources,
        resource::Strings &strings);

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

    std::string _mainMenuMusicResRef;
    std::string _charGenMusicResRef;
    std::string _charGenLoadScreenResRef;

    GameScreen _screen {GameScreen::MainMenu};

    // Services

    ActionFactory &_actionFactory;
    Classes &_classes;
    Combat &_combat;
    Cursors &_cursors;
    EffectFactory &_effectFactory;
    Feats &_feats;
    FootstepSounds &_footstepSounds;
    GUISounds &_guiSounds;
    ObjectFactory &_objectFactory;
    Party &_party;
    Portraits &_portraits;
    Reputes &_reputes;
    ScriptRunner &_scriptRunner;
    Skills &_skills;
    SoundSets &_soundSets;
    Surfaces &_surfaces;

    audio::AudioFiles &_audioFiles;
    audio::AudioPlayer &_audioPlayer;
    graphics::Context &_context;
    graphics::Features &_features;
    graphics::Fonts &_fonts;
    graphics::Lips &_lips;
    graphics::Materials &_materials;
    graphics::Meshes &_meshes;
    graphics::Models &_models;
    graphics::PBRIBL &_pbrIbl;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;
    graphics::Walkmeshes &_walkmeshes;
    graphics::Window &_window;
    scene::SceneGraph &_sceneGraph;
    scene::WorldRenderPipeline &_worldRenderPipeline;
    script::Scripts &_scripts;
    resource::Resources &_resources;
    resource::Strings &_strings;

    // END Services

    // GUI colors

    glm::vec3 _guiColorBase {0.0f};
    glm::vec3 _guiColorHilight {0.0f};
    glm::vec3 _guiColorDisabled {0.0f};

    // END GUI colors

    virtual void start() = 0;

    void stopMovement();

    // Rendering

    void drawAll();
    void drawWorld();
    void drawGUI();

    // END Rendering

    Options _options;

    uint32_t _ticks {0};
    bool _quit {false};
    std::shared_ptr<video::Video> _video;
    CursorType _cursorType {CursorType::None};
    float _gameSpeed {1.0f};
    bool _loadFromSaveGame {false};
    CameraType _cameraType {CameraType::ThirdPerson};
    bool _paused {false};
    std::set<std::string> _moduleNames;

    // Modules

    std::string _nextModule;
    std::string _nextEntry;
    std::shared_ptr<Module> _module;
    std::map<std::string, std::shared_ptr<Module>> _loadedModules;

    // END Modules

    // GUI

    std::unique_ptr<ILoadingScreen> _loadScreen;

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

    void deinit();

    virtual void loadModuleNames() = 0;
    virtual void loadModuleResources(const std::string &moduleName) = 0;

    void update();

    void loadNextModule();
    float measureFrameTime();
    void playMusic(const std::string &resRef);
    void runMainLoop();
    void toggleInGameCameraType();

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);

    void updateCamera(float dt);
    void updateVideo(float dt);
    void updateMusic();
    void updateSceneGraph(float dt);

    virtual gui::GUI *getScreenGUI() const = 0;

    // GUI

    virtual void loadInGameMenus() = 0;
    virtual void loadLoadingScreen() = 0;

    virtual void openInGame() = 0;
    virtual void changeScreen(GameScreen screen) = 0;

    void withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block);

    // END GUI

    // Save games

    std::shared_ptr<resource::GffStruct> getPartyMemberNFOStruct(int index) const;

    // END Save games

    virtual void onModuleSelected(const std::string &name) = 0;
    virtual void drawHUD() = 0;
    virtual CameraType getConversationCamera(int &cameraId) const = 0;
};

} // namespace game

} // namespace reone
