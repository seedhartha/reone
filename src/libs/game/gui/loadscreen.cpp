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

#include "reone/game/gui/loadscreen.h"

#include "reone/resource/resources.h"

#include "reone/game/game.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void LoadingScreen::preload(IGUI &gui) {
    if (_game.isTSL()) {
        gui.setResolution(800, 600);
    }
}

void LoadingScreen::onGUILoaded() {
    if (!_game.isTSL()) {
        loadBackground(BackgroundType::Load);
    }

    bindControls();

    _binding.lblHint->setTextMessage("");
}

void LoadingScreen::bindControls() {
    _binding.pbProgress = findControl<ProgressBar>("PB_PROGRESS");
    _binding.lblHint = findControl<Label>("LBL_HINT");
    _binding.lblLogo = findControl<Label>("LBL_LOGO");
    _binding.lblLoading = findControl<Label>("LBL_LOADING");
}

void LoadingScreen::setImage(const std::string &resRef) {
    _gui->rootControl().setBorderFill(resRef);
}

void LoadingScreen::setProgress(int progress) {
    _binding.pbProgress->setValue(progress);
}

} // namespace game

} // namespace reone
