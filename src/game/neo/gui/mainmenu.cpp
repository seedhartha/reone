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

#include "mainmenu.h"

#include "game.h"

using namespace reone::gui::neo;

namespace reone {

namespace game {

namespace neo {

void MainMenu::init() {
    load("mainmenu16x12");
    bindControls();
    disableControl("BTN_WARP");
    disableControl("LB_MODULES");
    disableControl("LBL_BW");
    disableControl("LBL_LUCAS");
    disableControl("LBL_NEWCONTENT");
}

void MainMenu::bindControls() {
}

bool MainMenu::handleClick(const Control &control) {
    if (control.tag() == "BTN_NEWGAME") {
        _game.startNewGame();
        return true;
    } else if (control.tag() == "BTN_EXIT") {
        _game.quit();
        return true;
    }
    return false;
}

} // namespace neo

} // namespace game

} // namespace reone
