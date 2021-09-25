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

#include "../gui/textinput.h"
#include "../graphics/font.h"
#include "../graphics/options.h"
#include "../graphics/types.h"

namespace reone {

namespace game {

class Game;

class Console : boost::noncopyable {
public:
    Console(Game &game);

    void init();
    bool handle(const SDL_Event &event);
    void draw();

    bool isOpen() const { return _open; }

private:
    typedef std::function<void(std::vector<std::string>)> CommandHandler;

    Game &_game;

    graphics::GraphicsOptions _opts;
    std::shared_ptr<graphics::Font> _font;
    bool _open { false };
    gui::TextInput _input;
    std::deque<std::string> _output;
    int _outputOffset { 0 };
    std::unordered_map<std::string, CommandHandler> _commands;
    std::stack<std::string> _history;

    bool handleMouseWheel(const SDL_MouseWheelEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);

    void executeInputText();
    void print(const std::string &text);
    void trimOutput();

    void drawBackground();
    void drawLines();

    // Commands

    void initCommands();
    void addCommand(const std::string &name, const CommandHandler &handler);

    void cmdClear(std::vector<std::string> tokens);
    void cmdDescribe(std::vector<std::string> tokens);
    void cmdListAnim(std::vector<std::string> tokens);
    void cmdPlayAnim(std::vector<std::string> tokens);
    void cmdKill(std::vector<std::string> tokens);
    void cmdAddItem(std::vector<std::string> tokens);
    void cmdGiveXP(std::vector<std::string> tokens);

    // END Commands
};

} // namespace game

} // namespace reone
