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

#include "player.h"

#include <stdexcept>

#include "area.h"
#include "camera/camera.h"
#include "module.h"
#include "object/creature.h"

using namespace std;

namespace reone {

namespace game {

Player::Player(Module *module, Area *area, Camera *camera) : _module(module), _area(area), _camera(camera) {
    if (!module) {
        throw invalid_argument("Module must not be null");
    }
    if (!area) {
        throw invalid_argument("Area must not be null");
    }
    if (!camera) {
        throw invalid_argument("Camera must not be null");
    }
}

bool Player::handle(const SDL_Event &event) {
    if (!_creature) return false;

    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        case SDL_KEYUP:
            return handleKeyUp(event.key);
        default:
            return false;
    }
}

bool Player::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            _moveForward = true;
            return true;

        case SDL_SCANCODE_Z:
            _moveLeft = true;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = true;
            return true;

        case SDL_SCANCODE_C:
            _moveRight = true;
            return true;

        default:
            return false;
    }
}

bool Player::handleKeyUp(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            _moveForward = false;
            return true;

        case SDL_SCANCODE_Z:
            _moveLeft = false;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = false;
            return true;

        case SDL_SCANCODE_C:
            _moveRight = false;
            return true;

        case SDL_SCANCODE_X:
            _creature->playGreetingAnimation();
            return true;

        default:
            return false;
    }
}

void Player::update(float dt) {
    if (!_creature) return;

    float heading = 0.0f;
    bool movement = true;

    if (_moveForward) {
        heading = _camera->heading();
    } else if (_moveBackward) {
        heading = _camera->heading() + glm::pi<float>();
    } else if (_moveLeft) {
        heading = _camera->heading() + glm::half_pi<float>();
    } else if (_moveRight) {
        heading = _camera->heading() - glm::half_pi<float>();
    } else {
        movement = false;
    }
    if (movement) {
        glm::vec2 dest(_creature->position());
        dest.x -= 100.0f * glm::sin(heading);
        dest.y += 100.0f * glm::cos(heading);

        if (_area->moveCreatureTowards(*_creature, dest, true, dt)) {
            _creature->setMovementType(MovementType::Run);
            _module->area()->update3rdPersonCameraTarget();
            _area->updateRoomVisibility();
            _area->selectNearestObject();
        }
    } else {
        _creature->setMovementType(MovementType::None);
    }
}

void Player::stopMovement() {
    _moveForward = false;
    _moveLeft = false;
    _moveBackward = false;
    _moveRight = false;
}

void Player::setCreature(Creature *creature) {
    _creature = creature;
}

} // namespace game

} // namespace reone
