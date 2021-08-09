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

#include "../../d20/attributes.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

}

namespace game {

class CharacterGeneration;

class CharGenSkills : public GameGUI {
public:
    CharGenSkills(CharacterGeneration *charGen, Game *game);

    void load() override;

    void reset(bool newGame);

private:
    struct Binding {
        std::shared_ptr<gui::Button> awaMinusBtn;
        std::shared_ptr<gui::Button> awaPlusBtn;
        std::shared_ptr<gui::Button> awarenessPointsBtn;
        std::shared_ptr<gui::Button> btnRecommended;
        std::shared_ptr<gui::Button> comMinusBtn;
        std::shared_ptr<gui::Button> comPlusBtn;
        std::shared_ptr<gui::Button> computerUsePointsBtn;
        std::shared_ptr<gui::Button> demMinusBtn;
        std::shared_ptr<gui::Button> demolitionsPointsBtn;
        std::shared_ptr<gui::Button> demPlusBtn;
        std::shared_ptr<gui::Button> perMinusBtn;
        std::shared_ptr<gui::Button> perPlusBtn;
        std::shared_ptr<gui::Button> persuadePointsBtn;
        std::shared_ptr<gui::Button> repairPointsBtn;
        std::shared_ptr<gui::Button> repMinusBtn;
        std::shared_ptr<gui::Button> repPlusBtn;
        std::shared_ptr<gui::Button> secMinusBtn;
        std::shared_ptr<gui::Button> secPlusBtn;
        std::shared_ptr<gui::Button> securityPointsBtn;
        std::shared_ptr<gui::Button> stealthPointsBtn;
        std::shared_ptr<gui::Button> steMinusBtn;
        std::shared_ptr<gui::Button> stePlusBtn;
        std::shared_ptr<gui::Button> treatInjuryPointsBtn;
        std::shared_ptr<gui::Button> treMinusBtn;
        std::shared_ptr<gui::Button> trePlusBtn;
        std::shared_ptr<gui::Label> awarenessLbl;
        std::shared_ptr<gui::Label> computerUseLbl;
        std::shared_ptr<gui::Label> costPointsLbl;
        std::shared_ptr<gui::Label> demolitionsLbl;
        std::shared_ptr<gui::Label> persuadeLbl;
        std::shared_ptr<gui::Label> remainingSelectionsLbl;
        std::shared_ptr<gui::Label> repairLbl;
        std::shared_ptr<gui::Label> securityLbl;
        std::shared_ptr<gui::Label> stealthLbl;
        std::shared_ptr<gui::Label> treatInjuryLbl;
        std::shared_ptr<gui::ListBox> lbDesc;
    } _binding;

    CharacterGeneration *_charGen;
    CreatureAttributes _attributes;
    int _points { 0 };

    void bindControls();
    void refreshControls();
    void updateCharacter();

    bool canIncreaseSkill(SkillType skill) const;

    int getPointCost(SkillType skill) const;

    void onClick(const std::string &control) override;
    void onFocusChanged(const std::string &control, bool focus) override;
};

} // namespace game

} // namespace reone
