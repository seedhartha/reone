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

#include "action.h"

namespace reone {

namespace game {

class Item;

class PickUpItemAction : public Action {
public:
    PickUpItemAction(Game &game, std::shared_ptr<Item> item) :
        Action(game, ActionType::PickUpItem),
        _item(std::move(item)) {
    }

    void execute(Object &actor, ActionContext &ctx, float dt) override;

private:
    std::shared_ptr<Item> _item;
};

} // namespace game

} // namespace reone
