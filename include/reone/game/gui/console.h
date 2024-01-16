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

namespace game {

struct ServicesView;

class Game;

class Console {
public:
    Console(
        Game &game,
        ServicesView &services) :
        _game(game),
        _services(services),
        _input(gui::TextInputFlags::console) {

        init();
    }

    void init();

    bool handle(const input::Event &event);
    void render();

    bool isOpen() const { return _open; }

private:
    using CommandHandler = std::function<void(std::string, std::vector<std::string>)>;

    struct Command {
        std::string name;
        std::string alias;
        std::string description;
        CommandHandler handler;
    };

    Game &_game;
    ServicesView &_services;

    std::shared_ptr<graphics::Font> _font;
    bool _open {false};
    gui::TextInput _input;
    std::deque<std::string> _output;
    int _outputOffset {0};
    std::stack<std::string> _history;

    // Commands

    std::vector<Command> _commands;
    std::unordered_map<std::string, Command> _commandByName;
    std::unordered_map<std::string, Command> _commandByAlias;

    // END Commands

    bool handleMouseWheel(const input::MouseWheelEvent &event);
    bool handleKeyUp(const input::KeyEvent &event);

    void executeInputText();
    void print(const std::string &text);
    void trimOutput();

    void renderBackground();
    void renderLines();

    // Commands

    void initCommands();

    void addCommand(std::string name, std::string alias, std::string description, CommandHandler handler);

    void cmdClear(std::string input, std::vector<std::string> tokens);
    void cmdInfo(std::string input, std::vector<std::string> tokens);
    void cmdListGlobals(std::string input, std::vector<std::string> tokens);
    void cmdListLocals(std::string input, std::vector<std::string> tokens);
    void cmdListAnim(std::string input, std::vector<std::string> tokens);
    void cmdPlayAnim(std::string input, std::vector<std::string> tokens);
    void cmdRunScript(std::string input, std::vector<std::string> tokens);
    void cmdWarp(std::string input, std::vector<std::string> tokens);
    void cmdKill(std::string input, std::vector<std::string> tokens);
    void cmdAddItem(std::string input, std::vector<std::string> tokens);
    void cmdGiveXP(std::string input, std::vector<std::string> tokens);
    void cmdShowAABB(std::string input, std::vector<std::string> tokens);
    void cmdShowWalkmesh(std::string input, std::vector<std::string> tokens);
    void cmdShowTriggers(std::string input, std::vector<std::string> tokens);
    void cmdHelp(std::string input, std::vector<std::string> tokens);

    // END Commands
};

} // namespace game

} // namespace reone
