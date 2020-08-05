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

#include "../audio/types.h"
#include "../gui/gui.h"
#include "../render/window.h"
#include "../resources/types.h"

#include "gui/debug.h"
#include "gui/dialog.h"
#include "gui/hud.h"
#include "gui/mainmenu.h"
#include "gui/modules.h"

#include "module.h"

namespace reone {

namespace game {

/**
 * Entry point for the game logic. Contains the main game loop.
 *
 * @see reone::game::Module
 * @see reone::gui::GUI
 * @see reone::render::RenderWindow
 */
class Game : public render::IEventHandler {
public:
    Game(
        resources::GameVersion version,
        const boost::filesystem::path &path,
        const std::string &startModule,
        const Options &opts);

    int run();

    void loadModule(const std::string &name, std::string entry = "");

    bool handle(const SDL_Event &event) override;

protected:
    resources::GameVersion _version { resources::GameVersion::KotOR };
    Options _opts;
    std::shared_ptr<Module> _module;
    std::string _nextModule;

    virtual void configure();
    virtual const std::shared_ptr<Module> makeModule(const std::string &name);
    virtual void configureModule();
    virtual void update();
    virtual void loadNextModule();

private:
    enum class Screen {
        None,
        MainMenu,
        ModuleSelection,
        InGame,
        Dialog
    };

    boost::filesystem::path _path;
    std::string _startModule;
    render::RenderWindow _renderWindow;
    uint32_t _ticks { 0 };
    bool _quit { false };
    Screen _screen { Screen::None };
    std::string _nextEntry;
    GameState _state;

    // GUI
    std::shared_ptr<MainMenu> _mainMenu;
    std::shared_ptr<ModulesGui> _modulesGui;
    std::shared_ptr<HUD> _hud;
    std::shared_ptr<DialogGui> _dialog;
    std::shared_ptr<DebugGui> _debug;

    void startModuleSelection();
    void runMainLoop();
    std::shared_ptr<gui::GUI> currentGUI() const;
    float getDeltaTime();
    void renderWorld();
    void renderGUI();
};

} // namespace game

} // namespace reone
