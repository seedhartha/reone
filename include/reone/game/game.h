/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/audio/source.h"
#include "reone/graphics/cursor.h"
#include "reone/graphics/eventhandler.h"
#include "reone/movie/movie.h"
#include "reone/script/routines.h"
#include "reone/system/logutil.h"

#include "action.h"
#include "combat.h"
#include "di/services.h"
#include "effect.h"
#include "event.h"
#include "gui/chargen.h"
#include "gui/computer.h"
#include "gui/console.h"
#include "gui/container.h"
#include "gui/conversation.h"
#include "gui/dialog.h"
#include "gui/hud.h"
#include "gui/ingame.h"
#include "gui/loadscreen.h"
#include "gui/mainmenu.h"
#include "gui/map.h"
#include "gui/partyselect.h"
#include "gui/profileoverlay.h"
#include "gui/saveload.h"
#include "location.h"
#include "object/area.h"
#include "object/camera/animated.h"
#include "object/camera/dialog.h"
#include "object/camera/firstperson.h"
#include "object/camera/static.h"
#include "object/camera/thirdperson.h"
#include "object/creature.h"
#include "object/door.h"
#include "object/encounter.h"
#include "object/module.h"
#include "object/placeable.h"
#include "object/sound.h"
#include "object/store.h"
#include "object/trigger.h"
#include "object/waypoint.h"
#include "options.h"
#include "party.h"
#include "script/runner.h"
#include "talent.h"

namespace reone {

namespace gui {

class GUI;

}

namespace game {

class Game : public graphics::IEventHandler, boost::noncopyable {
public:
    enum class Screen {
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

    Game(
        GameID gameId,
        std::filesystem::path path,
        OptionsView &options,
        ServicesView &services) :
        _gameId(gameId),
        _path(std::move(path)),
        _options(options),
        _services(services),
        _party(*this),
        _combat(*this, services) {
    }

    ~Game() {
        deinit();
    }

    void init();

    void deinit() {
        if (_updateThread.joinable()) {
            _updateThread.join();
        }
    }

    /**
     * @return exit code
     */
    int run();

    void quit() {
        setState(State::Quitting);
    }

    void playVideo(const std::string &name);

    bool isPaused() const { return _paused; }
    bool isTSL() const { return _gameId == GameID::TSL; }

    Camera *getActiveCamera() const;

    OptionsView &options() { return _options; }
    const OptionsView &options() const { return _options; }
    Party &party() { return _party; }
    Combat &combat() { return _combat; }
    ScriptRunner &scriptRunner() { return *_scriptRunner; }
    Map &map() { return *_map; }
    script::IRoutines &routines() { return *_routines; }

    std::shared_ptr<Module> module() const { return _module; }
    CameraType cameraType() const { return _cameraType; }
    const std::set<std::string> &moduleNames() const { return _moduleNames; }

    void initLocalServices();
    void setSceneSurfaces();

    void setCursorType(CursorType type);
    void setPaused(bool paused);
    void setRelativeMouseMode(bool relative);

    void openMainMenu();
    void openInGame();
    void openInGameMenu(InGameMenuTab tab);
    void openLevelUp();
    void openContainer(const std::shared_ptr<Object> &container);
    void openPartySelection(const PartySelectionContext &ctx);
    void openSaveLoad(SaveLoadMode mode);

    void startCharacterGeneration();
    void startDialog(const std::shared_ptr<Object> &owner, const std::string &resRef);

    void pauseConversation();
    void resumeConversation();

    void setBarkBubbleText(std::string text, float durartion);

    Screen currentScreen() const {
        return _screen;
    }

    std::shared_ptr<movie::IMovie> movie() const {
        return _movie;
    }

    // Module loading

    /**
     * @param entry waypoint tag to spawn at, or empty string to spawn at default location
     */
    void loadModule(const std::string &name, std::string entry = "");

    void scheduleModuleTransition(const std::string &moduleName, const std::string &entry);

    // END Module loading

    // Objects

    std::shared_ptr<Object> getObjectById(uint32_t id) const;

    inline std::shared_ptr<Module> newModule() {
        return newObject<Module>(*this, _services);
    }
    inline std::shared_ptr<Item> newItem() {
        return newObject<Item>(*this, _services);
    }

    inline std::shared_ptr<Area> newArea(std::string sceneName = kSceneMain) {
        return newObject<Area>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Creature> newCreature(std::string sceneName = kSceneMain) {
        return newObject<Creature>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Placeable> newPlaceable(std::string sceneName = kSceneMain) {
        return newObject<Placeable>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Door> newDoor(std::string sceneName = kSceneMain) {
        return newObject<Door>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Waypoint> newWaypoint(std::string sceneName = kSceneMain) {
        return newObject<Waypoint>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Trigger> newTrigger(std::string sceneName = kSceneMain) {
        return newObject<Trigger>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Sound> newSound(std::string sceneName = kSceneMain) {
        return newObject<Sound>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<AnimatedCamera> newAnimatedCamera(float aspect, std::string sceneName = kSceneMain) {
        return newObject<AnimatedCamera>(aspect, std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<DialogCamera> newDialogCamera(CameraStyle style, float aspect, std::string sceneName = kSceneMain) {
        return newObject<DialogCamera>(std::move(style), aspect, std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<FirstPersonCamera> newFirstPersonCamera(float fovy, float aspect, std::string sceneName = kSceneMain) {
        return newObject<FirstPersonCamera>(fovy, aspect, std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<StaticCamera> newStaticCamera(float aspect, std::string sceneName = kSceneMain) {
        return newObject<StaticCamera>(aspect, std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<ThirdPersonCamera> newThirdPersonCamera(CameraStyle style, float aspect, std::string sceneName = kSceneMain) {
        return newObject<ThirdPersonCamera>(std::move(style), aspect, std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Encounter> newEncounter(std::string sceneName = kSceneMain) {
        return newObject<Encounter>(std::move(sceneName), *this, _services);
    }

    inline std::shared_ptr<Store> newStore(std::string sceneName = kSceneMain) {
        return newObject<Store>(std::move(sceneName), *this, _services);
    }

    template <class T>
    inline std::shared_ptr<T> getObjectById(uint32_t id) const {
        return std::dynamic_pointer_cast<T>(getObjectById(id));
    }

    template <class T, class... Args>
    inline std::shared_ptr<T> newObject(Args &&...args) {
        auto object = std::make_shared<T>(_nextObjectId++, std::forward<Args>(args)...);
        auto [inserted, _] = _objectById.insert(std::make_pair(object->id(), std::move(object)));
        return std::static_pointer_cast<T>(inserted->second);
    }

    template <class T, class... Args>
    inline std::shared_ptr<T> newAction(Args &&...args) {
        return std::make_shared<T>(*this, _services, std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    inline std::shared_ptr<T> newEffect(Args &&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <class... Args>
    inline std::shared_ptr<Event> newEvent(Args &&...args) {
        return std::make_shared<Event>(std::forward<Args>(args)...);
    }

    template <class... Args>
    inline std::shared_ptr<Location> newLocation(Args &&...args) {
        return std::make_shared<Location>(std::forward<Args>(args)...);
    }

    template <class... Args>
    inline std::shared_ptr<Talent> newTalent(Args &&...args) {
        return std::make_shared<Talent>(std::forward<Args>(args)...);
    }

    // END Objects

    // Global variables

    bool getGlobalBoolean(const std::string &name) const;
    int getGlobalNumber(const std::string &name) const;
    std::shared_ptr<Location> getGlobalLocation(const std::string &name) const;
    std::string getGlobalString(const std::string &name) const;

    const std::map<std::string, std::string> &globalStrings() const { return _globalStrings; }
    const std::map<std::string, bool> &globalBooleans() const { return _globalBooleans; }
    const std::map<std::string, int> &globalNumbers() const { return _globalNumbers; }
    const std::map<std::string, std::shared_ptr<Location>> &globalLocations() const { return _globalLocations; }

    void setGlobalBoolean(const std::string &name, bool value);
    void setGlobalLocation(const std::string &name, const std::shared_ptr<Location> &location);
    void setGlobalNumber(const std::string &name, int value);
    void setGlobalString(const std::string &name, const std::string &value);

    // END Global variables

    // IEventHandler

    bool handle(const SDL_Event &event) override;

    // END IEventHandler

private:
    enum class State {
        Created,
        Running,
        Paused,
        Quitting
    };

    GameID _gameId;
    std::filesystem::path _path;
    OptionsView &_options;
    ServicesView &_services;

    std::atomic<State> _state {State::Created};
    uint32_t _ticks {0};
    uint32_t _updateTicks {0};

    Screen _screen {Screen::None};

    std::shared_ptr<movie::IMovie> _movie;
    CursorType _cursorType {CursorType::None};
    std::shared_ptr<graphics::Cursor> _cursor;
    std::atomic<float> _gameSpeed {1.0f};
    CameraType _cameraType {CameraType::ThirdPerson};
    bool _paused {false};
    std::set<std::string> _moduleNames;

    uint32_t _nextObjectId {2}; // ids 0 and 1 are reserved
    std::map<uint32_t, std::shared_ptr<Object>> _objectById;

    // Services

    Party _party;
    Combat _combat;

    std::unique_ptr<script::IRoutines> _routines;
    std::unique_ptr<ScriptRunner> _scriptRunner;

    // END Services

    // GUI

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<CharacterGeneration> _charGen;
    std::unique_ptr<HUD> _hud;
    std::unique_ptr<InGameMenu> _inGame;
    std::unique_ptr<DialogGUI> _dialog;
    std::unique_ptr<ComputerGUI> _computer;
    std::unique_ptr<ContainerGUI> _container;
    std::unique_ptr<PartySelection> _partySelect;
    std::unique_ptr<SaveLoad> _saveLoad;

    std::unique_ptr<Map> _map;
    std::unique_ptr<Console> _console;
    std::unique_ptr<LoadingScreen> _loadScreen;
    std::unique_ptr<ProfileOverlay> _profileOverlay;

    Conversation *_conversation {nullptr}; /**< pointer to either DialogGUI or ComputerGUI  */

    // END GUI

    // Modules

    std::string _nextModule;
    std::string _nextEntry;
    std::shared_ptr<Module> _module;
    std::map<std::string, std::shared_ptr<Module>> _loadedModules;

    // END Modules

    // Audio

    std::string _musicResRef;
    std::shared_ptr<audio::AudioSource> _music;

    // END Audio

    // Global variables

    std::map<std::string, std::string> _globalStrings;
    std::map<std::string, bool> _globalBooleans;
    std::map<std::string, int> _globalNumbers;
    std::map<std::string, std::shared_ptr<Location>> _globalLocations;

    // END Global variables

    // Update thread

    std::thread _updateThread;
    std::mutex _updateMutex;
    std::condition_variable _updateCondVar;
    std::atomic_bool _updateFlushed {false};

    // END Update thread

    void setState(State state) {
        std::lock_guard<std::mutex> lock(_updateMutex);
        _state = state;
        _updateCondVar.notify_one();
    }

    void stopMovement();

    void mainLoopIteration(float dt);

    void loadDefaultParty();
    void loadNextModule();
    void playMusic(const std::string &resRef);
    void toggleInGameCameraType();

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);

    void onModuleSelected(const std::string &name);
    void drawHUD();

    GameGUI *getScreenGUI() const;
    CameraType getConversationCamera(int &cameraId) const;

    // Updates

    void update(float dt);

    void updateMovie(float dt);
    void updateMusic();
    void updateCamera(float dt);
    void updateSceneGraph(float dt);
    void updateCursor();

    void updateThreadFunc();

    // END Updates

    // Rendering

    void drawAll();

    void drawWorld();
    void drawGUI();

    // END Rendering

    // GUI

    void loadInGameMenus();

    void changeScreen(Screen screen);

    void withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block);

    template <class T>
    std::unique_ptr<T> tryLoadGUI() {
        auto gui = std::make_unique<T>(*this, _services);
        try {
            gui->init();
            return gui;
        } catch (const std::exception &e) {
            error(boost::format("Error loading GUI: %s") % std::string(e.what()));
            return nullptr;
        }
    }

    // END GUI
};

} // namespace game

} // namespace reone
