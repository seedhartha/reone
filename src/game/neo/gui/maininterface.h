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

#include "../../../gui/neo/gui.h"

namespace reone {

namespace gui {

namespace neo {

class Button;

}

} // namespace gui

namespace graphics {

struct GraphicsServices;
struct GraphicsOptions;

} // namespace graphics

namespace resource {

struct ResourceServices;

}

namespace game {

namespace neo {

class MainInterface : public gui::neo::Gui {
public:
    MainInterface(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        gui::neo::Gui(
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void init();

private:
    gui::neo::Button *_btnActionDown0 {nullptr};
    gui::neo::Button *_btnActionDown1 {nullptr};
    gui::neo::Button *_btnActionDown2 {nullptr};
    gui::neo::Button *_btnActionDown3 {nullptr};

    void bindControls();
};

} // namespace neo

} // namespace game

} // namespace reone
