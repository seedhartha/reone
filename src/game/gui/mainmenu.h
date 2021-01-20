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

#include "../../scene/node/modelscenenode.h"
#include "../../scene/scenegraph.h"
#include "../../resource/types.h"

#include "../types.h"

#include "gui.h"

namespace reone {

namespace game {

class Game;

class MainMenu : public GameGUI {
public:
    MainMenu(Game *game);

    void load() override;

    void onClick(const std::string &control) override;

    void onModuleSelected(const std::string &name);

private:
    Game *_game { nullptr };

    void onListBoxItemClick(const std::string &control, const std::string &item) override;

    void configureButtons();
    void setButtonColors(const std::string &tag);
    std::shared_ptr<scene::ModelSceneNode> getKotorModel(scene::SceneGraph &sceneGraph);
    void startModuleSelection();
};

} // namespace game

} // namespace reone
