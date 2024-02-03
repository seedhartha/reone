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

#include <gtest/gtest.h>

#include "reone/game/neo/inventory.h"
#include "reone/game/neo/object/item.h"

using namespace reone::game;
using namespace reone::game::neo;

TEST(Inventory, should_add_item) {
    // given
    Inventory inventory {0, ""};
    Item item {1, ""};

    // when
    inventory.add(item);

    // then
    auto &items = inventory.items();
    EXPECT_EQ(items.size(), 1);
    EXPECT_EQ(items.front().get(), item);
}
