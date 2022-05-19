/*
 * Copyright (c) 2020-2022 The reone project contributors
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

class Label;
class ListBox;

} // namespace gui

namespace game {

class DialogGui : public gui::Gui {
public:
    DialogGui(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        gui::Gui(
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {

        _scaleMode = ScaleMode::Manual;
    }

    void init();

private:
    // Binding

    gui::Label *_lblMessage {nullptr};
    gui::ListBox *_lbReplies {nullptr};

    // END Binding

    void bindControls();
};

} // namespace game

} // namespace reone
