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

#pragma once

#include "../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace game {

class Map;
class Waypoint;

} // namespace game

namespace kotor {

class MapMenu : public GameGUI {
public:
    MapMenu(KotOR &game, game::Services &services);

    void load() override;
    void draw() override;

    void refreshControls();

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnExit;
        std::shared_ptr<gui::Button> btnUp;
        std::shared_ptr<gui::Button> btnDown;
        std::shared_ptr<gui::Button> btnReturn;
        std::shared_ptr<gui::Label> lblArea;
        std::shared_ptr<gui::Label> lblMap;
        std::shared_ptr<gui::Label> lblMapNote;

        // KotOR only
        std::shared_ptr<gui::Button> btnPrtySlct;
        // END KotOR only
    } _binding;

    std::vector<std::shared_ptr<game::Waypoint>> _notes;
    int _selectedNoteIdx {0};

    void bindControls();
    void refreshSelectedNote();
};

} // namespace kotor

} // namespace reone
