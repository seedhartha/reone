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

#include "map.h"

#include <stdexcept>

#include "../../game.h"
#include "../../map.h"

#include "../colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

MapMenu::MapMenu(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("map");
    _backgroundType = BackgroundType::Menu;

    if (game->version() == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
}

void MapMenu::load() {
    GUI::load();

    disableControl("BTN_PRTYSLCT");
    disableControl("BTN_RETURN");
}

void MapMenu::render() const {
    GUI::render();

    Control &label = getControl("LBL_Map");
    const Control::Extent &extent = label.extent();

    glm::vec4 bounds(
        _controlOffset.x + extent.left,
        _controlOffset.y + extent.top,
        extent.width,
        extent.height);

    shared_ptr<Area> area(_game->module()->area());
    area->map().render(Map::Mode::Default, bounds);
}

void MapMenu::onClick(const string &control) {
    if (control == "BTN_EXIT") {
        _game->openInGame();
    }
}

} // namespace game

} // namespace reone
