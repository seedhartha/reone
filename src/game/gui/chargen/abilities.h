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

#include "../../rp/abilities.h"

#include "../gui.h"

namespace reone {

namespace game {

class CharacterGeneration;

class CharGenAbilities : public GameGUI {
public:
    CharGenAbilities(CharacterGeneration *charGen, resource::GameID gameId, const render::GraphicsOptions &opts);

    void load() override;

    void reset(bool newGame);

private:
    CharacterGeneration *_charGen;
    CreatureAbilities _abilities;
    int _points { 0 };

    void onClick(const std::string &control) override;
    void onFocusChanged(const std::string &control, bool focus) override;

    void refreshControls();
    void updateCharacter();

    int getPointCost(Ability ability) const;
};

} // namespace game

} // namespace reone
