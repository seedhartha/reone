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

#pragma once

#include "../action.h"

namespace reone {

namespace game {

class UseFeatAction : public Action {
public:
    UseFeatAction(Game &game,
                  ServicesView &services,
                  FeatType feat,
                  std::shared_ptr<Object> target) :
        Action(game, services, ActionType::UseFeat),
        _feat(feat),
        _target(std::move(target)) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

    FeatType feat() const { return _feat; }

private:
    FeatType _feat;
    std::shared_ptr<Object> _target;
};

} // namespace game

} // namespace reone
