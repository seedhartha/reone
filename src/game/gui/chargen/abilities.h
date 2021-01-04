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

#include "../../../gui/gui.h"

#include "../../rp/attributes.h"

namespace reone {

namespace game {

class CharacterGeneration;

class CharGenAbilities : public gui::GUI {
public:
    CharGenAbilities(CharacterGeneration *charGen, resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;

    void reset();

private:
    CharacterGeneration *_charGen;
    CreatureAttributes _attributes;
    int _points { 0 };

    void onClick(const std::string &control) override;

    void refreshControls();
    void updateCharacter();

    int getPointCost(Ability ability) const;
};

} // namespace game

} // namespace reone
