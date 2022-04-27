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

#include "game.h"

#include "../../graphics/services.h"
#include "../../graphics/window.h"

namespace reone {

namespace game {

namespace neo {

void Game::init() {
    _services.graphics.window.setEventHandler(this);
}

void Game::run() {
    while (!_finished) {
        handleInput();
        update();
        render();
    }
}

void Game::handleInput() {
    _services.graphics.window.processEvents(_finished);
}

void Game::update() {
}

void Game::render() {
}

bool Game::handle(const SDL_Event &e) {
    return false;
}

} // namespace neo

} // namespace game

} // namespace reone
