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

#include "../../game/gui/console.h"
#include "../../graphics/font.h"
#include "../../graphics/options.h"
#include "../../graphics/types.h"
#include "../../gui/textinput.h"

namespace reone {

namespace game {

struct Services;

class Game;

} // namespace game

namespace kotor {

class Console : public game::IConsole {
public:
    Console(
        game::Game &game,
        game::Services &services) :
        _game(game),
        _services(services),
        _input(gui::TextInputFlags::console) {

        init();
    }

    void init();
    bool handle(const SDL_Event &event) override;
    void draw() override;

    bool isOpen() const override { return _open; }

private:
    typedef std::function<void(std::string, std::vector<std::string>)> CommandHandler;

    struct Command {
        std::string name;
        std::string alias;
        std::string description;
        CommandHandler handler;
    };

    game::Game &_game;
    game::Services &_services;

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

    bool handleMouseWheel(const SDL_MouseWheelEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);

    void executeInputText();
    void print(const std::string &text);
    void trimOutput();

    void drawBackground();
    void drawLines();

    // Commands

    void initCommands();

    void addCommand(std::string name, std::string alias, std::string description, CommandHandler handler);

    void cmdClear(std::string input, std::vector<std::string> tokens);
    void cmdInfo(std::string input, std::vector<std::string> tokens);
    void cmdListAnim(std::string input, std::vector<std::string> tokens);
    void cmdPlayAnim(std::string input, std::vector<std::string> tokens);
    void cmdKill(std::string input, std::vector<std::string> tokens);
    void cmdAddItem(std::string input, std::vector<std::string> tokens);
    void cmdGiveXP(std::string input, std::vector<std::string> tokens);
    void cmdWarp(std::string input, std::vector<std::string> tokens);
    void cmdListRoutine(std::string input, std::vector<std::string> tokens);
    void cmdExec(std::string input, std::vector<std::string> tokens);
    void cmdHelp(std::string input, std::vector<std::string> tokens);

    // END Commands
};

} // namespace kotor

} // namespace reone
