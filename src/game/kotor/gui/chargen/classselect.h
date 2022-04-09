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

#include "../../../../resource/types.h"

#include "../../../character.h"
#include "../../../portrait.h"
#include "../../../types.h"

#include "../../gui.h"

namespace reone {

namespace scene {

class ModelSceneNode;
class SceneGraph;

} // namespace scene

namespace gui {

class Button;
class Label;

} // namespace gui

namespace kotor {

class CharacterGeneration;

class ClassSelection : public GameGUI {
public:
    ClassSelection(
        CharacterGeneration &charGen,
        KotOR &game,
        game::Services &services);

    void load() override;

private:
    struct ClassButton {
        gui::Control *control {nullptr};
        glm::ivec2 center {0};
        game::Character character;
    };

    struct Binding {
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnSel1;
        std::shared_ptr<gui::Button> btnSel2;
        std::shared_ptr<gui::Button> btnSel3;
        std::shared_ptr<gui::Button> btnSel4;
        std::shared_ptr<gui::Button> btnSel5;
        std::shared_ptr<gui::Button> btnSel6;
        std::shared_ptr<gui::Label> lblClass;
        std::shared_ptr<gui::Label> lblDesc;
        std::shared_ptr<gui::Label> threeDModel1;
        std::shared_ptr<gui::Label> threeDModel2;
        std::shared_ptr<gui::Label> threeDModel3;
        std::shared_ptr<gui::Label> threeDModel4;
        std::shared_ptr<gui::Label> threeDModel5;
        std::shared_ptr<gui::Label> threeDModel6;
    } _binding;

    CharacterGeneration &_charGen;

    glm::ivec2 _defaultButtonSize {0};
    glm::ivec2 _enlargedButtonSize {0};
    std::vector<ClassButton> _classButtons;

    void bindControls();
    void setupClassButtons();
    void setupClassButton(int index, game::Gender gender, game::ClassType clazz);

    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(int appearance, scene::SceneGraph &sceneGraph);
    int getRandomCharacterAppearance(game::Gender gender, game::ClassType clazz);
    std::vector<game::Portrait> getPCPortraitsByGender(game::Gender gender);

    void setButtonColors(gui::Control &control);
    void setClassButtonEnlarged(int index, bool enlarged);

    void onClassButtonFocusChanged(int index, bool focus);
    void onClassButtonClick(int index);
};

} // namespace kotor

} // namespace reone
