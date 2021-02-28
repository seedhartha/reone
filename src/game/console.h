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

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <queue>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "SDL2/SDL_events.h"

#include "../gui/textinput.h"
#include "../render/font.h"
#include "../render/types.h"

namespace reone {

namespace game {

class Game;

class Console : boost::noncopyable {
public:
    Console(Game *game);

    void load();
    bool handle(const SDL_Event &event);
    void render();

    bool isOpen() const { return _open; }

private:
    typedef std::function<void(std::vector<std::string>)> CommandHandler;

    Game *_game;
    render::GraphicsOptions _opts;
    std::shared_ptr<render::Font> _font;
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
