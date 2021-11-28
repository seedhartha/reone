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

#include "../../../game/d20/attributes.h"

#include "../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

} // namespace gui

namespace kotor {

class CharacterGeneration;

class CharGenAbilities : public GameGUI {
public:
    CharGenAbilities(
        CharacterGeneration &charGen,
        KotOR &game,
        game::Services &services);

    void load() override;

    void reset(bool newGame);

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnAccept;
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnRecommended;
        std::shared_ptr<gui::Label> costPointsLbl;
        std::shared_ptr<gui::Label> remainingSelectionsLbl;
        std::shared_ptr<gui::ListBox> lbDesc;

        std::shared_ptr<gui::Label> strLbl;
        std::shared_ptr<gui::Label> dexLbl;
        std::shared_ptr<gui::Label> conLbl;
        std::shared_ptr<gui::Label> intLbl;
        std::shared_ptr<gui::Label> wisLbl;
        std::shared_ptr<gui::Label> chaLbl;

        std::shared_ptr<gui::Button> strPointsBtn;
        std::shared_ptr<gui::Button> dexPointsBtn;
        std::shared_ptr<gui::Button> conPointsBtn;
        std::shared_ptr<gui::Button> intPointsBtn;
        std::shared_ptr<gui::Button> wisPointsBtn;
        std::shared_ptr<gui::Button> chaPointsBtn;

        std::shared_ptr<gui::Button> strMinusBtn;
        std::shared_ptr<gui::Button> dexMinusBtn;
        std::shared_ptr<gui::Button> conMinusBtn;
        std::shared_ptr<gui::Button> intMinusBtn;
        std::shared_ptr<gui::Button> wisMinusBtn;
        std::shared_ptr<gui::Button> chaMinusBtn;

        std::shared_ptr<gui::Button> strPlusBtn;
        std::shared_ptr<gui::Button> dexPlusBtn;
        std::shared_ptr<gui::Button> conPlusBtn;
        std::shared_ptr<gui::Button> intPlusBtn;
        std::shared_ptr<gui::Button> wisPlusBtn;
        std::shared_ptr<gui::Button> chaPlusBtn;

        // KotOR only
        std::shared_ptr<gui::Label> lblAbilityMod;
        // END KotOR only
    } _binding;

    CharacterGeneration &_charGen;
    game::CreatureAttributes _attributes;
    int _points {0};

    void bindControls();
    void refreshControls();
    void updateCharacter();

    int getPointCost(game::Ability ability) const;

    void onAbilityLabelFocusChanged(game::Ability ability, bool focus);
    void onMinusButtonClick(game::Ability ability);
    void onPlusButtonClick(game::Ability ability);
};

} // namespace kotor

} // namespace reone
