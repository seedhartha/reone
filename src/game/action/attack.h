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

#include "../object/creature.h"

#include "objectaction.h"

namespace reone {

namespace game {

class AttackAction : public ObjectAction {
public:
    AttackAction(const std::shared_ptr<SpatialObject> &object, float range = kDefaultAttackRange) :
        ObjectAction(ActionType::AttackObject, object),
        _range(range) {
    }

    std::shared_ptr<SpatialObject> target() const {
        return std::static_pointer_cast<SpatialObject>(_object);
    }

    float range() const { return _range; }

private:
    float _range;
};

} // namespace game

} // namespace reone