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

#include "../../graphics/eventhandler.h"

#include "../services.h"
#include "../types.h"

namespace reone {

namespace game {

namespace neo {

class Game : public graphics::IEventHandler {
public:
    Game(GameID id, ServicesView &services) :
        _id(id),
        _services(services) {
    }

    void init();

    void run();

    bool handle(const SDL_Event &e) override;

private:
    GameID _id;
    ServicesView &_services;

    bool _finished {false};

    void handleInput();
    void update();
    void render();
};

} // namespace neo

} // namespace game

} // namespace reone
