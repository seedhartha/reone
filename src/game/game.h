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

#include "../audio/soundinstance.h"
#include "../audio/types.h"
#include "../gui/gui.h"
#include "../render/scene/scenegraph.h"
#include "../render/window.h"
#include "../resources/types.h"

#include "console.h"
#include "gui/classselect.h"
#include "gui/container.h"
#include "gui/debug.h"
#include "gui/dialog.h"
#include "gui/equip.h"
#include "gui/hud.h"
#include "gui/mainmenu.h"
#include "gui/portraitselect.h"
#include "gui/target.h"
#include "object/area.h"
#include "object/module.h"

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
    enum class Screen {
        None,
        MainMenu,
        ClassSelection,
        PortraitSelection,
        InGame,
        Dialog,
        Container,
        Equipment
    };

    Options _options;
    resources::GameVersion _version { resources::GameVersion::KotOR };
    std::unique_ptr<ObjectFactory> _objectFactory;
    render::SceneGraph _sceneGraph;
    std::shared_ptr<Module> _module;
    std::string _nextModule;
    Screen _screen { Screen::MainMenu };
    std::shared_ptr<DialogGui> _dialogGui;
    bool _pickDialogReplyEnabled { true };

    virtual void initObjectFactory();
    virtual void configure();
    virtual void configureModule();
    virtual void update();
    virtual void loadNextModule();
    virtual void startDialog(uint32_t ownerId, const std::string &resRef);
    virtual void onDialogReplyPicked(uint32_t index);
    virtual void onDialogFinished();

private:
    boost::filesystem::path _path;
    render::RenderWindow _window;
    uint32_t _ticks { 0 };
    bool _quit { false };
    std::string _nextEntry;
    GameState _state;
    std::shared_ptr<audio::SoundInstance> _music;
    Console _console;

    // GUI

    std::shared_ptr<MainMenu> _mainMenu;
    std::shared_ptr<ClassSelectionGui> _classesGui;
    std::shared_ptr<PortraitSelectionGui> _portraitsGui;
    std::shared_ptr<HUD> _hud;
    std::shared_ptr<DebugOverlay> _debugOverlay;
    std::shared_ptr<ContainerGui> _containerGui;
    std::shared_ptr<EquipmentGui> _equipmentGui;
    std::shared_ptr<TargetOverlay> _targetOverlay;

    // END GUI

    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;

    void runMainLoop();
    std::shared_ptr<gui::GUI> currentGUI() const;
    float getDeltaTime();
    void onDialogSpeakerChanged(uint32_t from, uint32_t to);

    // Initialization

    void initGameVersion();
    void initSubsystems();
    void deinitSubsystems();

    // END Initialization

    // Loading

    void loadMainMenu();
    void loadClassSelectionGui();
    void loadPortraitsGui();
    void loadHUD();
    void loadDebugOverlay();
    void loadDialogGui();
    void loadContainerGui();
    void loadCursor();
    void loadEquipmentGui();
    void loadTargetOverlay();

    // END Loading

    // Rendering

    void drawAll();
    void drawWorld();
    void drawGUI();
    void drawGUI3D();
    void drawCursor();

    // END Rendering
};

} // namespace game

} // namespace reone
