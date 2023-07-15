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
#include "../../types.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

} // namespace gui

namespace game {

class AbilitiesMenu : public GameGUI {
public:
    AbilitiesMenu(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("abilities");
    }

    void refreshControls();

private:
    struct SkillInfo {
        SkillType skill;
        std::string name;
        std::string description;
        std::shared_ptr<graphics::Texture> icon;
    };

    struct Controls {
        std::shared_ptr<gui::Button> BTN_CHANGE1;
        std::shared_ptr<gui::Button> BTN_CHANGE2;
        std::shared_ptr<gui::Button> BTN_EXIT;
        std::shared_ptr<gui::Button> BTN_FEATS;
        std::shared_ptr<gui::Button> BTN_POWERS;
        std::shared_ptr<gui::Button> BTN_SKILLS;
        std::shared_ptr<gui::Label> LBL_ABILITIES;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_BAR6;
        std::shared_ptr<gui::Label> LBL_BONUS;
        std::shared_ptr<gui::Label> LBL_BONUSVAL;
        std::shared_ptr<gui::Label> LBL_FILTER;
        std::shared_ptr<gui::Label> LBL_INFOBG;
        std::shared_ptr<gui::Label> LBL_NAME;
        std::shared_ptr<gui::Label> LBL_PORTRAIT;
        std::shared_ptr<gui::Label> LBL_RANKVAL;
        std::shared_ptr<gui::Label> LBL_SKILLRANK;
        std::shared_ptr<gui::Label> LBL_TOTAL;
        std::shared_ptr<gui::Label> LBL_TOTALVAL;
        std::shared_ptr<gui::ListBox> LB_ABILITY;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::ListBox> LB_DESC_FEATS;
    };

    Controls _controls;

    std::unordered_map<SkillType, SkillInfo> _skills;

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_CHANGE1 = findControl<gui::Button>("BTN_CHANGE1");
        _controls.BTN_CHANGE2 = findControl<gui::Button>("BTN_CHANGE2");
        _controls.BTN_EXIT = findControl<gui::Button>("BTN_EXIT");
        _controls.BTN_FEATS = findControl<gui::Button>("BTN_FEATS");
        _controls.BTN_POWERS = findControl<gui::Button>("BTN_POWERS");
        _controls.BTN_SKILLS = findControl<gui::Button>("BTN_SKILLS");
        _controls.LBL_ABILITIES = findControl<gui::Label>("LBL_ABILITIES");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_BAR6 = findControl<gui::Label>("LBL_BAR6");
        _controls.LBL_BONUS = findControl<gui::Label>("LBL_BONUS");
        _controls.LBL_BONUSVAL = findControl<gui::Label>("LBL_BONUSVAL");
        _controls.LBL_FILTER = findControl<gui::Label>("LBL_FILTER");
        _controls.LBL_INFOBG = findControl<gui::Label>("LBL_INFOBG");
        _controls.LBL_NAME = findControl<gui::Label>("LBL_NAME");
        _controls.LBL_PORTRAIT = findControl<gui::Label>("LBL_PORTRAIT");
        _controls.LBL_RANKVAL = findControl<gui::Label>("LBL_RANKVAL");
        _controls.LBL_SKILLRANK = findControl<gui::Label>("LBL_SKILLRANK");
        _controls.LBL_TOTAL = findControl<gui::Label>("LBL_TOTAL");
        _controls.LBL_TOTALVAL = findControl<gui::Label>("LBL_TOTALVAL");
        _controls.LB_ABILITY = findControl<gui::ListBox>("LB_ABILITY");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.LB_DESC_FEATS = findControl<gui::ListBox>("LB_DESC_FEATS");
    }

    void loadSkills();
    void refreshPortraits();

    std::shared_ptr<graphics::Texture> getFrameTexture() const;
};

} // namespace game

} // namespace reone
