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
#include "../../../resource/ltrfile.h"

#include "../gui.h"

namespace reone {

namespace game {

class CharacterGeneration;

class NameEntry : public GameGUI {
public:
    NameEntry(CharacterGeneration *charGen, resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;
    bool handle(const SDL_Event &event) override;

    void loadRandomName();

private:
    CharacterGeneration *_charGen { nullptr };
    gui::TextInput _input;
    gui::Control *_nameBoxEdit { nullptr };
    resource::LtrFile _maleLtr;
    resource::LtrFile _femaleLtr;
    resource::LtrFile _lastNameLtr;

    void onClick(const std::string &control) override;

    void loadLtrFile(const std::string &resRef, resource::LtrFile &ltr);

    std::string getRandomName() const;
};

} // namespace game

} // namespace reone
