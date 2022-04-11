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

#include "../action.h"
#include "../object.h"
#include "../types.h"

namespace reone {

namespace game {

class CastSpellAtObjectAction : public Action {
public:
    CastSpellAtObjectAction(
        Game &game,
        ServicesView &services,
        SpellType spell,
        std::shared_ptr<Object> target,
        int metaMagic,
        bool cheat,
        int domainLevel,
        ProjectilePathType projectilePathType,
        bool instantSpell) :
        Action(game, services, ActionType::CastSpellAtObject),
        _spell(spell),
        _target(std::move(target)),
        _metaMagic(metaMagic),
        _cheat(cheat),
        _domainLevel(domainLevel),
        _projectilePathType(projectilePathType),
        _instantSpell(instantSpell) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

private:
    SpellType _spell;
    std::shared_ptr<Object> _target;
    int _metaMagic;
    bool _cheat;
    int _domainLevel;
    ProjectilePathType _projectilePathType;
    bool _instantSpell;
};

} // namespace game

} // namespace reone
