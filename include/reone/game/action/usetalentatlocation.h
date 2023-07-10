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
#include "../location.h"
#include "../talent.h"

namespace reone {

namespace game {

class UseTalentAtLocationAction : public Action {
public:
    UseTalentAtLocationAction(Game &game,
                              ServicesView &services,
                              std::shared_ptr<Talent> chosenTalent,
                              std::shared_ptr<Location> targetLocation) :
        Action(game, services, ActionType::UseTalentAtLocation),
        _chosenTalent(std::move(chosenTalent)),
        _targetLocation(std::move(targetLocation)) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override {
        complete();
    }

private:
    std::shared_ptr<Talent> _chosenTalent;
    std::shared_ptr<Location> _targetLocation;
};

} // namespace game

} // namespace reone
