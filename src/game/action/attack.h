/*
 * Copyright © 2020 uwadmin12
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

#include "objectaction.h"
#include "../object/creature.h"

namespace reone {

namespace game {

class AttackAction : public ObjectAction {
public:
    AttackAction(const std::shared_ptr<Creature> &object, float distance = 3.2f, float timeout = 6.0f) :
        ObjectAction(ActionType::AttackObject, object), _timeout(timeout), _distance(distance), _inRange(false) { }

    std::shared_ptr<Creature> target() { return std::static_pointer_cast<Creature>(_object); } // static_pointer_cast(nullptr)?

    void setAttack() { _inRange = true;  } // handled by actionExecutor

    bool isInRange() { return _inRange; }

    void advance(float dt) {
        _timeout = glm::max(0.0f, _timeout - dt);
    }

    bool isTimedOut() const {
        return _timeout < 1E-6;
    }

    float distance() const { return _distance; }

private:
    float _timeout;
    float _distance;
    bool _inRange;
};

} // namespace game

} // namespace reone