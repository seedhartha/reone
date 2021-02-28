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

#include "../gui.h"

namespace reone {

namespace game {

class CharacterGeneration;

class CustomCharacterGeneration : public GameGUI {
public:
    CustomCharacterGeneration(CharacterGeneration *charGen, resource::GameID gameId, const render::GraphicsOptions &opts);

    void load() override;

    void goToNextStep();

    int step() const { return _step; }

    void setStep(int step);

private:
    CharacterGeneration *_charGen { nullptr };
    int _step { 0 };

    void onClick(const std::string &control) override;

    void doSetStep(int step);
};

} // namespace game

} // namespace reone
