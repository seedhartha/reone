/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "camera/camera.h"
#include "object/area.h"
#include "object/creature.h"
#include "object/module.h"
#include "party.h"

using namespace std;

namespace reone {

namespace game {

Player::Player(Module *module, Area *area, Camera *camera, const Party *party) :
    _module(module), _area(area), _camera(camera), _party(party) {

    if (!module) {
        throw invalid_argument("Module must not be null");
    }
    if (!area) {
        throw invalid_argument("Area must not be null");
    }
    if (!camera) {
        throw invalid_argument("Camera must not be null");
    }
    if (!party) {
        throw invalid_argument("Party must not be null");
    }
}

bool Player::handle(const SDL_Event &event) {
    shared_ptr<Creature> partyLeader(_party->leader());
    if (!partyLeader) return false;

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
            _party->leader()->playGreetingAnimation();
            return true;

        default:
            return false;
    }
}

void Player::update(float dt) {
    shared_ptr<Creature> partyLeader(_party->leader());
    if (!partyLeader) return;

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

    ActionQueue &actions = partyLeader->actionQueue();

    if (movement) {
        actions.clear();

        glm::vec2 dest(partyLeader->position());
        dest.x -= 100.0f * glm::sin(heading);
        dest.y += 100.0f * glm::cos(heading);

        if (_area->moveCreatureTowards(*partyLeader, dest, true, dt)) {
            partyLeader->setMovementType(Creature::MovementType::Run);
        }
    } else if (actions.empty()) {
        partyLeader->setMovementType(Creature::MovementType::None);
    }
}

void Player::stopMovement() {
    _moveForward = false;
    _moveLeft = false;
    _moveBackward = false;
    _moveRight = false;
}

} // namespace game

} // namespace reone
