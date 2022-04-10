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

#include "../../game/types.h"
#include "../../gui/control/button.h"
#include "../../gui/control/label.h"
#include "../../gui/control/listbox.h"
#include "../../resource/types.h"
#include "../../scene/graph.h"
#include "../../scene/node/model.h"

#include "../gui.h"

namespace reone {

namespace game {

class MainMenu : public GameGUI {
public:
    MainMenu(Game &game, GameServices &services);

    void load() override;

    void onModuleSelected(const std::string &name);

private:
    struct Binding {
        std::shared_ptr<gui::ListBox> lbModules;
        std::shared_ptr<gui::Label> lbl3dView;
        std::shared_ptr<gui::Label> lblGameLogo;
        std::shared_ptr<gui::Label> lblBw;
        std::shared_ptr<gui::Label> lblLucas;
        std::shared_ptr<gui::Button> btnLoadGame;
        std::shared_ptr<gui::Button> btnNewGame;
        std::shared_ptr<gui::Button> btnMovies;
        std::shared_ptr<gui::Button> btnOptions;
        std::shared_ptr<gui::Label> lblNewContent;
        std::shared_ptr<gui::Button> btnExit;
        std::shared_ptr<gui::Button> btnWarp;

        // TSL only
        std::shared_ptr<gui::Label> lblMenuBg;
        std::shared_ptr<gui::Button> btnMusic;
        std::shared_ptr<gui::Button> btnMoreGames;
        std::shared_ptr<gui::Button> btnTslrcm;
        // END TSL only
    } _binding;

    void bindControls();
    void configureButtons();
    void setup3DView();
    void setButtonColors(gui::Control &control);
    void startModuleSelection();
    void loadModuleNames();

    std::shared_ptr<scene::ModelSceneNode> getKotorModel(scene::SceneGraph &sceneGraph);
};

} // namespace game

} // namespace reone
