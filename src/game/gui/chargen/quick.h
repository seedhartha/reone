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

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace game {

class CharacterGeneration;

class QuickCharacterGeneration : public GameGUI {
public:
    QuickCharacterGeneration(
        CharacterGeneration &charGen,
        Game &game,
        ServicesView &services);

    void load() override;

    void goToNextStep();

    int step() const { return _step; }

    void setStep(int step);

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnCancel;
        std::shared_ptr<gui::Button> btnStepName1;
        std::shared_ptr<gui::Button> btnStepName2;
        std::shared_ptr<gui::Button> btnStepName3;
        std::shared_ptr<gui::Label> lbl1;
        std::shared_ptr<gui::Label> lbl2;
        std::shared_ptr<gui::Label> lbl3;
        std::shared_ptr<gui::Label> lblDecoration;
        std::shared_ptr<gui::Label> lblNum1;
        std::shared_ptr<gui::Label> lblNum2;
        std::shared_ptr<gui::Label> lblNum3;
    } _binding;

    CharacterGeneration &_charGen;
    int _step {0};

    void bindControls();
    void doSetStep(int step);
};

} // namespace game

} // namespace reone
