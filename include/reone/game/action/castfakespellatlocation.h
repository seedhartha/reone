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

namespace reone {

namespace game {

class CastFakeSpellAtLocationAction : public Action {
public:
    CastFakeSpellAtLocationAction(
        Game &game,
        ServicesView &services,
        SpellType spell,
        std::shared_ptr<Location> target,
        ProjectilePathType projectilePathType) :
        Action(game, services, ActionType::CastFakeSpellAtLocation),
        _spell(spell),
        _target(std::move(target)),
        _projectilePathType(projectilePathType) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

private:
    SpellType _spell;
    std::shared_ptr<Location> _target;
    ProjectilePathType _projectilePathType;
};

} // namespace game

} // namespace reone
