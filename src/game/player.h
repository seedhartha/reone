/*
* Copyright © 2020 Vsevolod Kremianskii
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

#include "SDL2/SDL_events.h"

namespace reone {

namespace game {

class Area;
class Camera;
class Creature;
class Module;

/**
 * Encapsulates third-person player controls.
 */
class Player {
public:
    Player(Module *module, Area *area, Camera *camera);

    bool handle(const SDL_Event &event);
    void update(float dt);

    void stopMovement();

    void setCreature(Creature *creature);

    Module *_module { nullptr };
    Area *_area { nullptr };
    Camera *_camera { nullptr };
    Creature *_creature { nullptr };
    bool _moveForward { false };
    bool _moveLeft { false };
    bool _moveBackward { false };
    bool _moveRight { false };

    Player(const Player &) = delete;
    Player &operator=(const Player &) = delete;

    bool handleKeyDown(const SDL_KeyboardEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);
};

} // namespace game

} // namespace reone
