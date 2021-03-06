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

#include "../../../resource/types.h"

#include "../../character.h"
#include "../../portrait.h"
#include "../../types.h"

#include "../gui.h"

namespace reone {

namespace game {

class ClassSelection : public GameGUI {
public:
    ClassSelection(Game *game);

    void load() override;

private:
    struct ClassButton {
        gui::Control *control { nullptr };
        glm::ivec2 center { 0 };
        Character character;
    };

    glm::ivec2 _defaultButtonSize { 0 };
    glm::ivec2 _enlargedButtonSize { 0 };
    std::vector<ClassButton> _classButtons;

    void setupClassButtons();
    void setupClassButton(int index, Gender gender, ClassType clazz);


    int getClassButtonIndexByTag(const std::string &tag) const;
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(int appearance, scene::SceneGraph &sceneGraph);
    int getRandomCharacterAppearance(Gender gender, ClassType clazz);
    std::vector<Portrait> getPCPortraitsByGender(Gender gender);

    void setButtonColors(gui::Control &control);
    void setClassButtonEnlarged(int index, bool enlarged);

    void onFocusChanged(const std::string &control, bool focus) override;
    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
