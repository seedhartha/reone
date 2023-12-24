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

#include "reone/game/player.h"

#include "reone/game/object/area.h"
#include "reone/game/object/camera.h"
#include "reone/game/object/creature.h"
#include "reone/game/object/module.h"
#include "reone/game/party.h"

namespace reone {

namespace game {

bool Player::handle(const input::Event &event) {
    std::shared_ptr<Creature> partyLeader(_party.getLeader());
    if (!partyLeader)
        return false;

    switch (event.type) {
    case input::EventType::KeyDown:
        return handleKeyDown(event.key);
    case input::EventType::KeyUp:
        return handleKeyUp(event.key);
    case input::EventType::MouseButtonDown:
        return handleMouseButtonDown(event.button);
    case input::EventType::MouseButtonUp:
        return handleMouseButtonUp(event.button);
    default:
        return false;
    }
}

bool Player::handleKeyDown(const input::KeyEvent &event) {
    switch (event.code) {
    case input::KeyCode::W: {
        _moveForward = true;
        return true;
    }
    case input::KeyCode::Z: {
        _moveLeft = true;
        return true;
    }
    case input::KeyCode::S: {
        _moveBackward = true;
        return true;
    }
    case input::KeyCode::C: {
        _moveRight = true;
        return true;
    }
    case input::KeyCode::X: {
        std::shared_ptr<Creature> partyLeader(_party.getLeader());
        partyLeader->playAnimation(CombatAnimation::Draw, partyLeader->getWieldType());
        return true;
    }
    case input::KeyCode::B: {
        _walk = true;
        return true;
    }
    default:
        return false;
    }
}

bool Player::handleKeyUp(const input::KeyEvent &event) {
    switch (event.code) {
    case input::KeyCode::W:
        _moveForward = false;
        return true;

    case input::KeyCode::Z:
        _moveLeft = false;
        return true;

    case input::KeyCode::S:
        _moveBackward = false;
        return true;

    case input::KeyCode::C:
        _moveRight = false;
        return true;

    case input::KeyCode::B:
        _walk = false;
        return true;

    default:
        return false;
    }
}

bool Player::handleMouseButtonDown(const input::MouseButtonEvent &event) {
    if (_camera.isMouseLookMode() && event.button == input::MouseButton::Left) {
        _moveForward = true;
        _leftPressedInMouseLook = true;
        return true;
    }

    return false;
}

bool Player::handleMouseButtonUp(const input::MouseButtonEvent &event) {
    if (_leftPressedInMouseLook && event.button == input::MouseButton::Left) {
        _moveForward = false;
        _leftPressedInMouseLook = false;
        return true;
    }

    return false;
}

void Player::update(float dt) {
    std::shared_ptr<Creature> partyLeader(_party.getLeader());
    if (!partyLeader || partyLeader->isMovementRestricted()) {
        return;
    }
    float facing = 0.0f;
    bool movement = true;

    if (_moveForward) {
        facing = _camera.facing();
    } else if (_moveBackward) {
        facing = _camera.facing() + glm::pi<float>();
    } else if (_moveLeft) {
        facing = _camera.facing() + glm::half_pi<float>();
    } else if (_moveRight) {
        facing = _camera.facing() - glm::half_pi<float>();
    } else {
        movement = false;
    }

    if (movement) {
        partyLeader->clearAllActions();
        glm::vec2 dir(glm::normalize(glm::vec2(-glm::sin(facing), glm::cos(facing))));
        if (_area.moveCreature(partyLeader, dir, !_walk, dt)) {
            partyLeader->setMovementType(_walk ? Creature::MovementType::Walk : Creature::MovementType::Run);
        }
    } else if (partyLeader->actions().empty()) {
        partyLeader->setMovementType(Creature::MovementType::None);
    }
}

void Player::stopMovement() {
    _moveForward = false;
    _moveLeft = false;
    _moveBackward = false;
    _moveRight = false;

    std::shared_ptr<Creature> partyLeader(_party.getLeader());
    if (partyLeader) {
        partyLeader->setMovementType(Creature::MovementType::None);
    }
}

bool Player::isMovementRequested() const {
    return _moveForward || _moveLeft || _moveBackward || _moveRight;
}

} // namespace game

} // namespace reone
