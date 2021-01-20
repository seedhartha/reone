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

#include "gui.h"

#include "../../audio/player.h"

#include "colorutil.h"
#include "sounds.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

GameGUI::GameGUI(GameVersion version, const GraphicsOptions &options) : GUI(version, options) {
}

void GameGUI::onClick(const string &control) {
    AudioPlayer::instance().play(GUISounds::instance().onClick(), AudioType::Sound);
}

void GameGUI::onFocusChanged(const string &control, bool focus) {
    if (focus) {
        AudioPlayer::instance().play(GUISounds::instance().onEnter(), AudioType::Sound);
    }
}

void GameGUI::initForGame() {
    if (_version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
}

} // namespace game

} // namespace reone
