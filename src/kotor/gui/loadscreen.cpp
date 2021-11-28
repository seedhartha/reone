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

#include "../../resource/resources.h"

#include "../kotor.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

LoadingScreen::LoadingScreen(KotOR &game, Services &services) :
    GameGUI(game, services) {
    _resRef = getResRef("loadscreen");

    if (_game.isTSL()) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        loadBackground(BackgroundType::Load);
    }
}

void LoadingScreen::load() {
    GUI::load();
    bindControls();

    _binding.lblHint->setTextMessage("");
}

void LoadingScreen::bindControls() {
    _binding.pbProgress = getControl<ProgressBar>("PB_PROGRESS");
    _binding.lblHint = getControl<Label>("LBL_HINT");
    _binding.lblLogo = getControl<Label>("LBL_LOGO");
    _binding.lblLoading = getControl<Label>("LBL_LOADING");
}

void LoadingScreen::setImage(const string &resRef) {
    _rootControl->setBorderFill(resRef);
}

void LoadingScreen::setProgress(int progress) {
    _binding.pbProgress->setValue(progress);
}

} // namespace game

} // namespace reone
