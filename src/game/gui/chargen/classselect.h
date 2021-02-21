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

#include "../../types.h"

#include "../gui.h"

namespace reone {

namespace game {

class Game;
class StaticCreatureBlueprint;

class ClassSelection : public GameGUI {
public:
    ClassSelection(Game *game);

    void load() override;

private:
    struct ClassButton {
        gui::Control *control { nullptr };
        glm::ivec2 center { 0 };
        std::shared_ptr<StaticCreatureBlueprint> character;
    };

    Game *_game { nullptr };
    glm::ivec2 _defaultButtonSize { 0 };
    glm::ivec2 _enlargedButtonSize { 0 };
    std::vector<ClassButton> _classButtons;

    void configureClassButtons();
    void configureClassModels();
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(const std::shared_ptr<StaticCreatureBlueprint> &character, scene::SceneGraph &sceneGraph);
    void configureClassModel(int index, Gender gender, ClassType clazz);
    void setButtonColors(gui::Control &control);
    void setClassButtonEnlarged(int index, bool enlarged);
    void onFocusChanged(const std::string &control, bool focus) override;
    int getClassButtonIndexByTag(const std::string &tag) const;
    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
