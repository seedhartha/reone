#pragma once

#include "../audio/types.h"
#include "../gui/debug.h"
#include "../gui/gui.h"
#include "../gui/hud.h"
#include "../gui/mainmenu.h"
#include "../gui/modules.h"
#include "../render/window.h"
#include "../resources/types.h"

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
        InGame
    };

    boost::filesystem::path _path;
    std::string _startModule;
    std::shared_ptr<gui::MainMenu> _mainMenu;
    std::shared_ptr<gui::ModulesGui> _modulesGui;
    std::shared_ptr<gui::HUD> _hud;
    std::shared_ptr<gui::DebugGui> _debug;
    render::RenderWindow _renderWindow;
    uint32_t _ticks { 0 };
    bool _quit { false };
    Screen _screen { Screen::None };
    std::string _nextEntry;
    GameState _state;

    void startModuleSelection();
    void runMainLoop();
    std::shared_ptr<gui::GUI> currentGUI() const;
    float getDeltaTime();
    void renderWorld();
    void renderGUI();
};

} // namespace game

} // namespace reone
