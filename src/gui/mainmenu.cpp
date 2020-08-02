#include "mainmenu.h"

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

MainMenu::MainMenu(const GraphicsOptions &opts) : GUI(opts) {
}

void MainMenu::load(GameVersion version) {
    GUI::load(getResRef(version), version == GameVersion::KotOR ? BackgroundType::Menu : BackgroundType::None);

    hideControl("BTN_WARP");
    hideControl("LBL_NEWCONTENT");
    hideControl("LBL_BW");
    hideControl("LBL_LUCAS");
}

std::string MainMenu::getResRef(GameVersion version) const {
    std::string resRef("mainmenu");
    switch (version) {
        case GameVersion::KotOR:
            resRef += "16x12";
            break;
        case GameVersion::TheSithLords:
            resRef += "8x6_p";
            break;
    }

    return resRef;
}

void MainMenu::onClick(const std::string &control) {
    if (control == "BTN_NEWGAME") {
        if (_onNewGame) _onNewGame();
    } else if (control == "BTN_EXIT") {
        if (_onExit) _onExit();
    }
}

void MainMenu::setOnNewGame(const std::function<void()> &fn) {
    _onNewGame = fn;
}

void MainMenu::setOnExit(const std::function<void()> &fn) {
    _onExit = fn;
}

} // namespace gui

} // namespace reone
