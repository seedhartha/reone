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

#include "classselect.h"
#include "name.h"
#include "portraitselect.h"
#include "quick.h"
#include "quickorcustom.h"

namespace reone {

namespace game {

enum class CharGenScreen {
    ClassSelection,
    QuickOrCustom,
    Quick,
    PortraitSelection,
    Name
};

class CharacterGeneration : public gui::GUI {
public:
    CharacterGeneration(resource::GameVersion, const render::GraphicsOptions &opts);

    void load() override;

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void render() const override;
    void render3D() const override;

    void setOnPlay(const std::function<void(const CreatureConfiguration &)> &fn);
    void setOnCancel(const std::function<void()> &fn);

private:
    CharGenScreen _screen { CharGenScreen::ClassSelection };
    CreatureConfiguration _character;

    // Sub GUI

    std::unique_ptr<ClassSelection> _classSelection;
    std::unique_ptr<QuickOrCustom> _quickOrCustom;
    std::unique_ptr<QuickCharacterGeneration> _quick;
    std::unique_ptr<PortraitSelection> _portraitSelection;
    std::unique_ptr<NameGui> _nameGui;

    // END Sub GUI

    std::function<void(const CreatureConfiguration &)> _onPlay;
    std::function<void()> _onCancel;

    gui::GUI *getSubGUI() const;

    // Loading

    void loadClassSelection();
    void loadQuickOrCustom();
    void loadQuickCharacterGeneration();
    void loadPortraitSelection();
    void loadNameGui();

    // END Loading
};

} // namespace game

} // namespace reone
