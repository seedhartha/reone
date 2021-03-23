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

#include "loadscreen.h"

#include "../../gui/control/progressbar.h"
#include "../../resource/resources.h"

#include "../game.h"
#include "../gameidutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

LoadingScreen::LoadingScreen(Game *game) :
    GameGUI(game->gameId(), game->options().graphics) {

    _resRef = getResRef("loadscreen");

    if (isTSL(_gameId)) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        loadBackground(BackgroundType::Load);
    }
}

void LoadingScreen::load() {
    GUI::load();
    setControlText("LBL_HINT", "");
}

void LoadingScreen::setImage(const string &resRef) {
    configureRootContol([&resRef](Control &ctrl) { ctrl.setBorderFill(resRef); });
}

void LoadingScreen::setProgress(int progress) {
    auto &progressBar = getControl<ProgressBar>("PB_PROGRESS");
    progressBar.setValue(progress);
}

} // namespace game

} // namespace reone
