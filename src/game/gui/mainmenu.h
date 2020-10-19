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

#include "../../system/gui/gui.h"
#include "../../system/scene/modelscenenode.h"
#include "../../system/scene/scenegraph.h"
#include "../../system/resource/types.h"

#include "../types.h"

namespace reone {

namespace game {

class MainMenu : public gui::GUI {
public:
    MainMenu(resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;
    void onClick(const std::string &control) override;

    void setOnNewGame(const std::function<void()> &fn);
    void setOnExit(const std::function<void()> &fn);
    void setOnModuleSelected(const std::function<void(const std::string &)> &fn);

private:
    std::function<void()> _onNewGame;
    std::function<void()> _onExit;
    std::function<void(const std::string &)> _onModuleSelected;

    void configureButtons();
    void setButtonColors(const std::string &tag);
    std::shared_ptr<scene::ModelSceneNode> getKotorModel(scene::SceneGraph &sceneGraph);
    void startModuleSelection();
};

} // namespace game

} // namespace reone
