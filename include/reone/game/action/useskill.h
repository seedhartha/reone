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

#include "../object/item.h"

#include "../action.h"

namespace reone {

namespace game {

class UseSkillAction : public Action {
public:
    UseSkillAction(Game &game,
                   ServicesView &services,
                   SkillType skill,
                   std::shared_ptr<Object> target,
                   int subSkill,
                   std::shared_ptr<Item> itemUsed) :
        Action(game, services, ActionType::UseSkill),
        _skill(skill),
        _target(std::move(target)),
        _subSkill(subSkill),
        _itemUsed(std::move(itemUsed)) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

    SkillType skill() const { return _skill; }

private:
    SkillType _skill;
    std::shared_ptr<Object> _target;
    int _subSkill;
    std::shared_ptr<Item> _itemUsed;
};

} // namespace game

} // namespace reone
