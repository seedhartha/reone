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

#include "../../../scene/node/model.h"

#include "../../portrait.h"

#include "../gui.h"

namespace reone {

namespace game {

class CharacterGeneration;

class PortraitSelection : public GameGUI {
public:
    PortraitSelection(CharacterGeneration *charGen, Game *game);

    void load() override;

    void updatePortraits();
    void resetCurrentPortrait();

private:
    CharacterGeneration *_charGen;

    std::vector<Portrait> _portraits;
    int _currentPortrait { 0 };

    void onClick(const std::string &control) override;

    void loadCurrentPortrait();
    void loadHeadModel();

    int getAppearanceFromCurrentPortrait() const;
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(scene::SceneGraph &sceneGraph);

    void setButtonColors(const std::string &tag);
};

} // namespace game

} // namespace reone
