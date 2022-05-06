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
#include "../../gui/textinput.h"

namespace reone {

namespace gui {

class Label;
class ListBox;

} // namespace gui

namespace game {

class Console : public gui::Gui {
public:
    Console(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        gui::Gui(
            graphicsOpt,
            graphicsSvc,
            resourceSvc),
        _textInput(gui::TextInputFlags::console) {
    }

    void init();

    // Gui

    bool handle(const SDL_Event &e) override;

    // END Gui

private:
    gui::TextInput _textInput;

    // Binding

    gui::ListBox *_lbLines {nullptr};
    gui::Label *_lblInput {nullptr};

    // END Binding

    void onEnterCommand(const std::string &command);
};

} // namespace game

} // namespace reone
