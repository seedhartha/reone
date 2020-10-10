/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../../gui/gui.h"
#include "../../../resource/types.h"

#include "../../object/objectfactory.h"
#include "../../types.h"

namespace reone {

namespace game {

class ClassSelection : public gui::GUI {
public:
    ClassSelection(resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;

    void setOnClassSelected(const std::function<void(const CreatureConfiguration &)> &fn);
    void setOnCancel(const std::function<void()> &fn);

private:
    struct ClassButton {
        gui::Control *control { nullptr };
        glm::ivec2 center { 0 };
        Gender gender { Gender::Male };
        ClassType clazz { ClassType::Soldier };
    };

    resource::GameVersion _version { resource::GameVersion::KotOR };
    glm::ivec2 _defaultButtonSize { 0 };
    glm::ivec2 _enlargedButtonSize { 0 };
    std::vector<ClassButton> _classButtons;
    std::function<void(const CreatureConfiguration &)> _onClassSelected;
    std::function<void()> _onCancel;

    void configureClassButtons();
    void configureClassModels();
    void configureClassModel(int index, Gender gender, ClassType clazz);
    void setButtonColors(gui::Control &control);
    void setClassButtonEnlarged(int index, bool enlarged);
    void onFocusChanged(const std::string &control, bool focus) override;
    int getClassButtonIndexByTag(const std::string &tag) const;
    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
