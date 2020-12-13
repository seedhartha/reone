/*
 * Copyright (c) 2020 The reone project contributors
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
#include <string>

#include "SDL2/SDL_events.h"

#include "../gui/textinput.h"
#include "../render/font.h"
#include "../render/types.h"

namespace reone {

namespace game {

class Console {
public:
    Console(const render::GraphicsOptions &opts);

    void load();
    bool handle(const SDL_Event &event);
    void render() const;

    bool isOpen() const;

private:
    std::shared_ptr<render::Font> _font;
    render::GraphicsOptions _opts;
    bool _open { false };
    gui::TextInput _input;

    Console(const Console &) = delete;
    Console &operator=(const Console &) = delete;

    bool handleKeyDown(const SDL_KeyboardEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);

    void executeInputText();

    void drawBackground() const;
    void drawLines() const;
};

} // namespace game

} // namespace reone
