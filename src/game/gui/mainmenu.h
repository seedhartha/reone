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

#include "../../gui/gui.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace scene {

struct SceneServices;

}

namespace graphics {

struct GraphicsServices;
struct GraphicsOptions;

} // namespace graphics

namespace resource {

struct ResourceServices;

}

namespace game {

class IGuiGame;

class MainMenu : public gui::Gui {
public:
    MainMenu(
        IGuiGame &game,
        scene::SceneServices &sceneSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        gui::Gui(
            graphicsOpt,
            graphicsSvc,
            resourceSvc),
        _game(game),
        _sceneSvc(sceneSvc) {
    }

    void init();

private:
    IGuiGame &_game;
    scene::SceneServices &_sceneSvc;

    // Binding
    gui::Label *_lbl3dView {nullptr};

    void bindControls();

    bool handleClick(const gui::Control &control) override;
};

} // namespace game

} // namespace reone
