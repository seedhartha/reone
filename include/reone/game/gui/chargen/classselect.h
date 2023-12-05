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

#include "reone/resource/types.h"
#include "reone/scene/graph.h"

#include "../../character.h"
#include "../../gui.h"
#include "../../portrait.h"
#include "../../types.h"

namespace reone {

namespace scene {

class ModelSceneNode;
class SceneGraph;

} // namespace scene

namespace gui {

class Button;
class Label;

} // namespace gui

namespace game {

class CharacterGeneration;

class ClassSelection : public GameGUI {
public:
    ClassSelection(
        CharacterGeneration &charGen,
        Game &game,
        ServicesView &services) :
        GameGUI(game, services),
        _charGen(charGen) {
        _resRef = guiResRef("classsel");
    }

private:
    struct ClassButton {
        gui::Control *control {nullptr};
        glm::ivec2 center {0};
        Character character;
    };

    struct Controls {
        std::shared_ptr<gui::Label> THREE_D_MODEL1;
        std::shared_ptr<gui::Label> THREE_D_MODEL2;
        std::shared_ptr<gui::Label> THREE_D_MODEL3;
        std::shared_ptr<gui::Label> THREE_D_MODEL4;
        std::shared_ptr<gui::Label> THREE_D_MODEL5;
        std::shared_ptr<gui::Label> THREE_D_MODEL6;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_SEL1;
        std::shared_ptr<gui::Button> BTN_SEL2;
        std::shared_ptr<gui::Button> BTN_SEL3;
        std::shared_ptr<gui::Button> BTN_SEL4;
        std::shared_ptr<gui::Button> BTN_SEL5;
        std::shared_ptr<gui::Button> BTN_SEL6;
        std::shared_ptr<gui::Label> LBL_CHAR_GEN;
        std::shared_ptr<gui::Label> LBL_CLASS;
        std::shared_ptr<gui::Label> LBL_DESC;
        std::shared_ptr<gui::Label> LBL_INSTRUCTION;
    };

    Controls _controls;

    CharacterGeneration &_charGen;

    glm::ivec2 _defaultButtonSize {0};
    glm::ivec2 _enlargedButtonSize {0};
    std::vector<ClassButton> _classButtons;

    void onGUILoaded() override;

    void bindControls() {
        _controls.THREE_D_MODEL1 = findControl<gui::Label>("3D_MODEL1");
        _controls.THREE_D_MODEL2 = findControl<gui::Label>("3D_MODEL2");
        _controls.THREE_D_MODEL3 = findControl<gui::Label>("3D_MODEL3");
        _controls.THREE_D_MODEL4 = findControl<gui::Label>("3D_MODEL4");
        _controls.THREE_D_MODEL5 = findControl<gui::Label>("3D_MODEL5");
        _controls.THREE_D_MODEL6 = findControl<gui::Label>("3D_MODEL6");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_SEL1 = findControl<gui::Button>("BTN_SEL1");
        _controls.BTN_SEL2 = findControl<gui::Button>("BTN_SEL2");
        _controls.BTN_SEL3 = findControl<gui::Button>("BTN_SEL3");
        _controls.BTN_SEL4 = findControl<gui::Button>("BTN_SEL4");
        _controls.BTN_SEL5 = findControl<gui::Button>("BTN_SEL5");
        _controls.BTN_SEL6 = findControl<gui::Button>("BTN_SEL6");
        _controls.LBL_CHAR_GEN = findControl<gui::Label>("LBL_CHAR_GEN");
        _controls.LBL_CLASS = findControl<gui::Label>("LBL_CLASS");
        _controls.LBL_DESC = findControl<gui::Label>("LBL_DESC");
        _controls.LBL_INSTRUCTION = findControl<gui::Label>("LBL_INSTRUCTION");
    }

    void setupClassButtons();
    void setupClassButton(int index, Gender gender, ClassType clazz);

    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(int appearance, scene::ISceneGraph &sceneGraph);
    int getRandomCharacterAppearance(Gender gender, ClassType clazz);
    std::vector<Portrait> getPCPortraitsByGender(Gender gender);

    void setButtonColors(gui::Control &control);
    void setClassButtonEnlarged(int index, bool enlarged);

    void onClassButtonSelectionChanged(int index, bool selected);
};

} // namespace game

} // namespace reone
