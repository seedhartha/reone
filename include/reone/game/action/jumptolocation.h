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

#include "locationaction.h"

namespace reone {

namespace game {

class JumpToLocationAction : public LocationAction {
public:
    JumpToLocationAction(Game &game, ServicesView &services, std::shared_ptr<Location> location) :
        LocationAction(game, services, ActionType::JumpToLocation, std::move(location)) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;
};

} // namespace game

} // namespace reone
