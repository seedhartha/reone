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

class CastSpellAtLocationAction : public Action {
public:
    CastSpellAtLocationAction(Game &game,
                              ServicesView &services,
                              SpellType spell,
                              std::shared_ptr<Location> targetLocation,
                              int metaMagic,
                              bool cheat,
                              ProjectilePathType projectilePathType,
                              bool instantSpell) :
        Action(game, services, ActionType::CastSpellAtLocation),
        _spell(spell),
        _targetLocation(std::move(targetLocation)),
        _metaMagic(metaMagic),
        _cheat(cheat),
        _projectilePathType(projectilePathType),
        _instantSpell(instantSpell) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override {
        complete();
    }

private:
    SpellType _spell;
    std::shared_ptr<Location> _targetLocation;
    int _metaMagic;
    bool _cheat;
    ProjectilePathType _projectilePathType;
    bool _instantSpell;
};

} // namespace game

} // namespace reone
