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

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

} // namespace gui

namespace game {

class CharacterGeneration;

class QuickOrCustom : public GameGUI {
public:
    QuickOrCustom(
        CharacterGeneration &charGen,
        Game &game,
        ServicesView &services) :
        GameGUI(game, services),
        _charGen(charGen) {
        _resRef = guiResRef("qorcpnl");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> CUST_CHAR_BTN;
        std::shared_ptr<gui::Label> LBL_DECORATION;
        std::shared_ptr<gui::Label> LBL_RBG;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::Button> QUICK_CHAR_BTN;
    };

    Controls _controls;

    CharacterGeneration &_charGen;

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.CUST_CHAR_BTN = findControl<gui::Button>("CUST_CHAR_BTN");
        _controls.LBL_DECORATION = findControl<gui::Label>("LBL_DECORATION");
        _controls.LBL_RBG = findControl<gui::Label>("LBL_RBG");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.QUICK_CHAR_BTN = findControl<gui::Button>("QUICK_CHAR_BTN");
    }
};

} // namespace game

} // namespace reone
