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

#include "reone/scene/node/model.h"

#include "../../gui.h"
#include "../../portrait.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace game {

class CharacterGeneration;

class PortraitSelection : public GameGUI {
public:
    PortraitSelection(
        CharacterGeneration &charGen,
        Game &game,
        ServicesView &services) :
        GameGUI(game, services),
        _charGen(charGen) {
        _resRef = guiResRef("portcust");
    }

    void updatePortraits();
    void resetCurrentPortrait();

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ACCEPT;
        std::shared_ptr<gui::Button> BTN_ARRL;
        std::shared_ptr<gui::Button> BTN_ARRR;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_BAR6;
        std::shared_ptr<gui::Label> LBL_BAR7;
        std::shared_ptr<gui::Label> LBL_BAR8;
        std::shared_ptr<gui::Label> LBL_BEVEL_B;
        std::shared_ptr<gui::Label> LBL_BEVEL_L;
        std::shared_ptr<gui::Label> LBL_BEVEL_M;
        std::shared_ptr<gui::Label> LBL_BEVEL_R;
        std::shared_ptr<gui::Label> LBL_BEVEL_T;
        std::shared_ptr<gui::Label> LBL_HEAD;
        std::shared_ptr<gui::Label> LBL_PORTRAIT;
        std::shared_ptr<gui::Label> MAIN_TITLE_LBL;
        std::shared_ptr<gui::Label> SUB_TITLE_LBL;
    };

    Controls _controls;

    CharacterGeneration &_charGen;

    std::vector<Portrait> _filteredPortraits;
    int _currentPortrait {0};

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_ACCEPT = findControl<gui::Button>("BTN_ACCEPT");
        _controls.BTN_ARRL = findControl<gui::Button>("BTN_ARRL");
        _controls.BTN_ARRR = findControl<gui::Button>("BTN_ARRR");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_BAR6 = findControl<gui::Label>("LBL_BAR6");
        _controls.LBL_BAR7 = findControl<gui::Label>("LBL_BAR7");
        _controls.LBL_BAR8 = findControl<gui::Label>("LBL_BAR8");
        _controls.LBL_BEVEL_B = findControl<gui::Label>("LBL_BEVEL_B");
        _controls.LBL_BEVEL_L = findControl<gui::Label>("LBL_BEVEL_L");
        _controls.LBL_BEVEL_M = findControl<gui::Label>("LBL_BEVEL_M");
        _controls.LBL_BEVEL_R = findControl<gui::Label>("LBL_BEVEL_R");
        _controls.LBL_BEVEL_T = findControl<gui::Label>("LBL_BEVEL_T");
        _controls.LBL_HEAD = findControl<gui::Label>("LBL_HEAD");
        _controls.LBL_PORTRAIT = findControl<gui::Label>("LBL_PORTRAIT");
        _controls.MAIN_TITLE_LBL = findControl<gui::Label>("MAIN_TITLE_LBL");
        _controls.SUB_TITLE_LBL = findControl<gui::Label>("SUB_TITLE_LBL");
    }

    void loadCurrentPortrait();
    void loadHeadModel();

    int getAppearanceFromCurrentPortrait() const;
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(scene::ISceneGraph &sceneGraph);

    void setButtonColors(gui::Control &control);
};

} // namespace game

} // namespace reone
