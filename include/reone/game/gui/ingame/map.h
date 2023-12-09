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

#pragma once

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace game {

class Waypoint;

class MapMenu : public GameGUI {
public:
    MapMenu(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("map");
    }

    void render() override;

    void refreshControls();

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_DOWN;
        std::shared_ptr<gui::Button> BTN_EXIT;
        std::shared_ptr<gui::Button> BTN_PRTYSLCT;
        std::shared_ptr<gui::Button> BTN_RETURN;
        std::shared_ptr<gui::Button> BTN_UP;
        std::shared_ptr<gui::Label> LBL_Area;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_COMPASS;
        std::shared_ptr<gui::Label> LBL_Map;
        std::shared_ptr<gui::Label> LBL_MapNote;
        std::shared_ptr<gui::Label> LBL_TITLE;
    };

    Controls _controls;

    std::vector<std::shared_ptr<Waypoint>> _notes;
    int _selectedNoteIdx {0};

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_DOWN = findControl<gui::Button>("BTN_DOWN");
        _controls.BTN_EXIT = findControl<gui::Button>("BTN_EXIT");
        _controls.BTN_PRTYSLCT = findControl<gui::Button>("BTN_PRTYSLCT");
        _controls.BTN_RETURN = findControl<gui::Button>("BTN_RETURN");
        _controls.BTN_UP = findControl<gui::Button>("BTN_UP");
        _controls.LBL_Area = findControl<gui::Label>("LBL_Area");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_COMPASS = findControl<gui::Label>("LBL_COMPASS");
        _controls.LBL_Map = findControl<gui::Label>("LBL_Map");
        _controls.LBL_MapNote = findControl<gui::Label>("LBL_MapNote");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
    }

    void refreshSelectedNote();
};

} // namespace game

} // namespace reone
