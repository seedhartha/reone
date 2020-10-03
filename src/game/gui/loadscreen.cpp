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

#include "loadscreen.h"

#include "../../resource/resources.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

LoadingScreen::LoadingScreen(GameVersion version, const GraphicsOptions &opts) : GUI(version, opts) {
    _resRef = getResRef("loadscreen");
    _backgroundType = BackgroundType::Load;

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void LoadingScreen::load() {
    GUI::load();

    configureRootContol([this](Control &control) {
        string resRef(_version == GameVersion::TheSithLords ? "load_default" : "load_chargen");
        control.setBorderFill(resRef);
    });
}

} // namespace game

} // namespace reone
