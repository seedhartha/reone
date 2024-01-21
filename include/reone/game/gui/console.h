/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/font.h"
#include "reone/graphics/options.h"
#include "reone/graphics/types.h"
#include "reone/gui/textinput.h"
#include "reone/input/event.h"

namespace reone {

namespace graphics {

struct GraphicsServices;

}

namespace resource {

struct ResourceServices;

}

namespace game {

struct OptionsView;

class Console {
public:
    using TokenList = std::vector<std::string>;
    using CommandHandler = std::function<void(const TokenList &)>;

    Console(OptionsView &options,
            graphics::GraphicsServices &graphicsSvc,
            resource::ResourceServices &resourceSvc) :
        _options(options),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc),
        _input(gui::TextInputFlags::console) {
    }

    void init();

    void registerCommand(std::string name, std::string description, CommandHandler handler);
    void printLine(const std::string &text);

    bool handle(const input::Event &event);
    void render();

    bool isOpen() const {
        return _open;
    }

private:
    struct Command {
        std::string name;
        std::string description;
        CommandHandler handler;
    };

    OptionsView &_options;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::shared_ptr<graphics::Font> _font;
    bool _open {false};
    gui::TextInput _input;
    std::deque<std::string> _output;
    int _outputOffset {0};
    std::stack<std::string> _history;

    // Commands

    std::list<Command> _commands;
    std::unordered_map<std::string, std::reference_wrapper<Command>> _nameToCommand;

    // END Commands

    bool handleMouseWheel(const input::MouseWheelEvent &event);
    bool handleKeyUp(const input::KeyEvent &event);

    void executeInputText();
    void trimOutput();

    void renderBackground();
    void renderLines();
};

} // namespace game

} // namespace reone
