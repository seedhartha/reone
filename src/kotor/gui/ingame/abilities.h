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

#include "../../../game/types.h"

#include "../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

} // namespace gui

namespace kotor {

class AbilitiesMenu : public GameGUI {
public:
    AbilitiesMenu(KotOR &game, game::Services &services);

    void load() override;

    void refreshControls();

private:
    struct SkillInfo {
        game::SkillType skill;
        std::string name;
        std::string description;
        std::shared_ptr<graphics::Texture> icon;
    };

    struct Binding {
        std::shared_ptr<gui::Button> btnExit;
        std::shared_ptr<gui::Button> btnFeats;
        std::shared_ptr<gui::Button> btnPowers;
        std::shared_ptr<gui::Label> btnSkills;
        std::shared_ptr<gui::Label> lblBonus;
        std::shared_ptr<gui::Label> lblBonusVal;
        std::shared_ptr<gui::Label> lblInfoBg;
        std::shared_ptr<gui::Label> lblName;
        std::shared_ptr<gui::Label> lblRankVal;
        std::shared_ptr<gui::Label> lblSkillRank;
        std::shared_ptr<gui::Label> lblTotal;
        std::shared_ptr<gui::Label> lblTotalVal;
        std::shared_ptr<gui::ListBox> lbAbility;
        std::shared_ptr<gui::ListBox> lbDesc;

        // KotOR only

        std::shared_ptr<gui::Button> btnChange1;
        std::shared_ptr<gui::Button> btnChange2;
        std::shared_ptr<gui::Label> lblPortrait;

        // END KotOR only

        // TSL only

        std::shared_ptr<gui::Label> lblAbilities;
        std::shared_ptr<gui::Label> lblBar1;
        std::shared_ptr<gui::Label> lblBar2;
        std::shared_ptr<gui::Label> lblBar3;
        std::shared_ptr<gui::Label> lblBar4;
        std::shared_ptr<gui::Label> lblBar5;
        std::shared_ptr<gui::Label> lblBar6;
        std::shared_ptr<gui::Label> lblFilter;
        std::shared_ptr<gui::ListBox> lbDescFeats;

        // END TSL only
    } _binding;

    std::unordered_map<game::SkillType, SkillInfo> _skills;

    void bindControls();
    void loadSkills();
    void refreshPortraits();

    std::shared_ptr<graphics::Texture> getFrameTexture() const;
};

} // namespace kotor

} // namespace reone
