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

#include "mainmenu.h"

#include "../../gui/control/listbox.h"
#include "../../resources/manager.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

MainMenu::MainMenu(const Options &opts) : GUI(opts.graphics), _opts(opts) {
    _resolutionX = 800;
    _resolutionY = 600;
}

void MainMenu::load(GameVersion version) {
    GUI::load(getResRef(version), version == GameVersion::KotOR ? BackgroundType::Menu : BackgroundType::None);

    hideControl("LB_MODULES");
    hideControl("LBL_NEWCONTENT");
    hideControl("LBL_BW");
    hideControl("LBL_LUCAS");

    if (!_opts.debug) {
        hideControl("BTN_WARP");
    }
}

string MainMenu::getResRef(GameVersion version) const {
    string resRef("mainmenu");
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

void MainMenu::onClick(const string &control) {
    if (control == "BTN_NEWGAME") {
        if (_onNewGame) _onNewGame();
    } else if (control == "BTN_EXIT") {
        if (_onExit) _onExit();
    } else if (control == "BTN_WARP") {
        startModuleSelection();
    }
}

void MainMenu::startModuleSelection() {
    hideControl("BTN_EXIT");
    hideControl("BTN_LOADGAME");
    hideControl("BTN_MOVIES");
    hideControl("BTN_MUSIC");
    hideControl("BTN_NEWGAME");
    hideControl("BTN_OPTIONS");
    hideControl("BTN_WARP");
    showControl("LB_MODULES");
    hideControl("LBL_3DVIEW");
    hideControl("LBL_GAMELOGO");
    hideControl("LBL_MENUBG");

    ListBox &modules = static_cast<ListBox &>(getControl("LB_MODULES"));
    modules.setOnItemClicked([this](const string &ctrl, const string &item) {
        if (_onModuleSelected) {
            _onModuleSelected(item);
        }
    });
    for (auto &module : ResMan.moduleNames()) {
        modules.add({ module, module });
    }
}

void MainMenu::setOnNewGame(const function<void()> &fn) {
    _onNewGame = fn;
}

void MainMenu::setOnExit(const function<void()> &fn) {
    _onExit = fn;
}

void MainMenu::setOnModuleSelected(const function<void(const string &)> &fn) {
    _onModuleSelected = fn;
}

} // namespace game

} // namespace reone
