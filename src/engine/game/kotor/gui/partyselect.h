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

#include "../../../gui/control/button.h"
#include "../../../gui/control/label.h"
#include "../../../gui/control/togglebutton.h"

#include "gui.h"

namespace reone {

namespace game {

constexpr int kNpcCount = 9;

class PartySelection : public GameGUI {
public:
    PartySelection(KotOR &game, Services &services);

    void load() override;

    void prepare(const PartySelectionContext &ctx);

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnAccept;
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnDone;
        std::shared_ptr<gui::ToggleButton> btnNpc0;
        std::shared_ptr<gui::ToggleButton> btnNpc1;
        std::shared_ptr<gui::ToggleButton> btnNpc2;
        std::shared_ptr<gui::ToggleButton> btnNpc3;
        std::shared_ptr<gui::ToggleButton> btnNpc4;
        std::shared_ptr<gui::ToggleButton> btnNpc5;
        std::shared_ptr<gui::ToggleButton> btnNpc6;
        std::shared_ptr<gui::ToggleButton> btnNpc7;
        std::shared_ptr<gui::ToggleButton> btnNpc8;
        std::shared_ptr<gui::Label> lbl3d;
        std::shared_ptr<gui::Label> lblBevelL;
        std::shared_ptr<gui::Label> lblBevelM;
        std::shared_ptr<gui::Label> lblChar0;
        std::shared_ptr<gui::Label> lblChar1;
        std::shared_ptr<gui::Label> lblChar2;
        std::shared_ptr<gui::Label> lblChar3;
        std::shared_ptr<gui::Label> lblChar4;
        std::shared_ptr<gui::Label> lblChar5;
        std::shared_ptr<gui::Label> lblChar6;
        std::shared_ptr<gui::Label> lblChar7;
        std::shared_ptr<gui::Label> lblChar8;
        std::shared_ptr<gui::Label> lblCount;
        std::shared_ptr<gui::Label> lblNa0;
        std::shared_ptr<gui::Label> lblNa1;
        std::shared_ptr<gui::Label> lblNa2;
        std::shared_ptr<gui::Label> lblNa3;
        std::shared_ptr<gui::Label> lblNa4;
        std::shared_ptr<gui::Label> lblNa5;
        std::shared_ptr<gui::Label> lblNa6;
        std::shared_ptr<gui::Label> lblNa7;
        std::shared_ptr<gui::Label> lblNa8;
        std::shared_ptr<gui::Label> lblNpcLevel;
        std::shared_ptr<gui::Label> lblNpcName;
        std::shared_ptr<gui::Label> lblTitle;

        // KotOR only
        std::shared_ptr<gui::Label> lblAvailable;
        std::shared_ptr<gui::Label> lblBevelR;
        // END KotOR only

        // TSL only
        std::shared_ptr<gui::ToggleButton> btnNpc9;
        std::shared_ptr<gui::ToggleButton> btnNpc10;
        std::shared_ptr<gui::ToggleButton> btnNpc11;
        std::shared_ptr<gui::Label> lblChar9;
        std::shared_ptr<gui::Label> lblChar10;
        std::shared_ptr<gui::Label> lblChar11;
        std::shared_ptr<gui::Label> lblNa9;
        std::shared_ptr<gui::Label> lblNa10;
        std::shared_ptr<gui::Label> lblNa11;
        std::shared_ptr<gui::Label> lblNameBack;
        // END TSL only
    } _binding;

    PartySelectionContext _context;
    int _selectedNpc {-1};
    bool _added[kNpcCount] {false};
    int _availableCount {0};

    void bindControls();
    void addNpc(int npc);
    void changeParty();
    void refreshAcceptButton();
    void refreshAvailableCount();
    void refreshNpcButtons();
    void removeNpc(int npc);

    gui::ToggleButton &getNpcButton(int npc);

    void onAcceptButtonClick();
    void onNpcButtonClick(int npc);
};

} // namespace game

} // namespace reone
