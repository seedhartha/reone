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

#include "../system/audio/soundinstance.h"
#include "../system/audio/types.h"
#include "../system/gui/gui.h"
#include "../system/render/pipeline/world.h"
#include "../system/render/scene/scenegraph.h"
#include "../system/render/window.h"
#include "../system/resource/types.h"

#include "console.h"
#include "gui/chargen/chargen.h"
#include "gui/container.h"
#include "gui/dialog.h"
#include "gui/equip.h"
#include "gui/hud.h"
#include "gui/loadscreen.h"
#include "gui/mainmenu.h"
#include "object/area.h"
#include "object/module.h"
#include "types.h"

namespace reone {

namespace game {

/**
 * Entry point for the game logic. Delegates work to instances of `Module` and
 * `GUI`. Contains the main game loop.
 *
 * @see game::Module
 * @see gui::GUI
 */
class Game : public render::IEventHandler {
public:
    Game(const boost::filesystem::path &path, const Options &opts);
    virtual ~Game() = default;

    int run();
    void loadModule(const std::string &name, const PartyConfiguration &party, std::string entry = "");
    bool handle(const SDL_Event &event) override;

    std::shared_ptr<Module> module() const;

    // Events

    int eventUserDefined(int eventNumber);
    int getUserDefinedEventNumber(int eventId);

    // END Events

    // Globals/locals

    bool getGlobalBoolean(const std::string &name) const;
    int getGlobalNumber(const std::string &name) const;
    bool getLocalBoolean(uint32_t objectId, int idx) const;
    int getLocalNumber(uint32_t objectId, int idx) const;

    void setGlobalBoolean(const std::string &name, bool value);
    void setGlobalNumber(const std::string &name, int value);
    void setLocalBoolean(uint32_t objectId, int idx, bool value);
    void setLocalNumber(uint32_t objectId, int idx, int value);

    // END Globals/locals

protected:
    Options _options;
    resource::GameVersion _version { resource::GameVersion::KotOR };
    std::unique_ptr<ObjectFactory> _objectFactory;
    render::SceneGraph _sceneGraph;
    render::WorldRenderPipeline _worldPipeline;
    std::shared_ptr<Module> _module;
    std::string _nextModule;
    GameScreen _screen { GameScreen::MainMenu };
    std::unique_ptr<DialogGui> _dialogGui;
    bool _pickDialogReplyEnabled { true };

    virtual void initObjectFactory();
    virtual void configure();
    virtual void configureModule();
    virtual void update();
    virtual void loadNextModule();
    virtual void startDialog(SpatialObject &owner, const std::string &resRef);

    // Event handlers

    virtual void onDialogReplyPicked(uint32_t index);
    virtual void onDialogFinished();

    // END Event handlers

private:
    struct UserDefinedEvent {
        int eventNumber { 0 };
    };

    boost::filesystem::path _path;
    render::RenderWindow _window;
    uint32_t _ticks { 0 };
    bool _quit { false };
    std::string _nextEntry;
    GameState _state;
    std::shared_ptr<audio::SoundInstance> _music;
    Console _console;
    std::map<int, UserDefinedEvent> _events;
    int _eventCounter { 0 };

    // GUI

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<LoadingScreen> _loadScreen;
    std::unique_ptr<CharacterGeneration> _charGen;
    std::unique_ptr<HUD> _hud;
    std::unique_ptr<ContainerGui> _containerGui;
    std::unique_ptr<EquipmentGui> _equipmentGui;

    // END GUI

    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;

    void openMainMenu();
    void runMainLoop();

    float getDeltaTime();
    gui::GUI *getScreenGUI() const;

    // Initialization

    void initGameVersion();
    void initSubsystems();
    void deinitSubsystems();

    // END Initialization

    // Loading

    void loadResources();
    void loadCursor();
    void loadMainMenu();
    void loadLoadingScreen();
    void loadCharacterGeneration();
    void loadHUD();
    void loadDialogGui();
    void loadContainerGui();
    void loadEquipmentGui();

    // END Loading

    // Rendering

    void drawAll();
    void drawWorld();
    void drawGUI();
    void drawCursor();

    // END Rendering

    // Event handlers

    void onNewGame();
    void onModuleSelected(const std::string &name);
    void onPlay(const CreatureConfiguration &config);
    void onEquipmentClick();
    void onGetItems();

    // END Event handlers

    // Helper methods

    void withLoadingScreen(const std::function<void()> &block);

    // END Helper methods
};

} // namespace game

} // namespace reone
