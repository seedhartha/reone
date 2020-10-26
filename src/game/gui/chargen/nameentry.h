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
#include "../../../gui/textinput.h"

namespace reone {

namespace game {

class CharacterGeneration;

class NameEntry : public gui::GUI {
public:
    NameEntry(CharacterGeneration *charGen, resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;
    bool handle(const SDL_Event &event) override;

private:
    CharacterGeneration *_charGen { nullptr };
    gui::TextInput _input;

    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
