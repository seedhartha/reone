/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../../gui/gui.h"
#include "../../scene/modelscenenode.h"
#include "../../scene/scenegraph.h"
#include "../../resource/types.h"

#include "../types.h"

namespace reone {

namespace game {

class Game;

class MainMenu : public gui::GUI {
public:
    MainMenu(Game *game);

    void load() override;

private:
    Game *_game { nullptr };

    void configureButtons();
    void setButtonColors(const std::string &tag);
    std::shared_ptr<scene::ModelSceneNode> getKotorModel(scene::SceneGraph &sceneGraph);
    void startModuleSelection();

    // Event handlers

    void onClick(const std::string &control) override;
    void onListBoxItemClick(const std::string &control, const std::string &item) override;

    // END Event handlers
};

} // namespace game

} // namespace reone
