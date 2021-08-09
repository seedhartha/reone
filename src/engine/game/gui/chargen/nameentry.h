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

#include "../../../gui/textinput.h"
#include "../../../resource/format/ltrreader.h"

#include "../gui.h"

namespace reone {

namespace game {

class CharacterGeneration;

class NameEntry : public GameGUI {
public:
    NameEntry(CharacterGeneration *charGen, Game *game);

    void load() override;
    bool handle(const SDL_Event &event) override;

    void loadRandomName();

private:
    struct Binding {
        std::shared_ptr<gui::Control> nameBoxEdit;
    } _binding;

    CharacterGeneration *_charGen { nullptr };
    gui::TextInput _input;
    resource::LtrReader _maleLtr;
    resource::LtrReader _femaleLtr;
    resource::LtrReader _lastNameLtr;

    void bindControls();
    void loadLtrFile(const std::string &resRef, resource::LtrReader &ltr);

    std::string getRandomName() const;

    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
