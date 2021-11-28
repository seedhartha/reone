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

#include "../../../game/portrait.h"
#include "../../../scene/node/model.h"

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace kotor {

class CharacterGeneration;

class PortraitSelection : public GameGUI {
public:
    PortraitSelection(
        CharacterGeneration &charGen,
        KotOR &game,
        game::Services &services);

    void load() override;

    void updatePortraits();
    void resetCurrentPortrait();

private:
    struct Binding {
        std::shared_ptr<gui::Label> lblHead;
        std::shared_ptr<gui::Label> lblPortrait;
        std::shared_ptr<gui::Button> btnArrL;
        std::shared_ptr<gui::Button> btnArrR;
        std::shared_ptr<gui::Button> btnAccept;
        std::shared_ptr<gui::Button> btnBack;
    } _binding;

    CharacterGeneration &_charGen;

    std::vector<game::Portrait> _filteredPortraits;
    int _currentPortrait {0};

    void bindControls();
    void loadCurrentPortrait();
    void loadHeadModel();

    int getAppearanceFromCurrentPortrait() const;
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(scene::SceneGraph &sceneGraph);

    void setButtonColors(gui::Control &control);
};

} // namespace kotor

} // namespace reone
