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

#include "reone/system/collectionutil.h"

using namespace reone;

struct Item {
    int key {0};
    int value {0};
};

TEST(collection_util, should_transform) {
    // given
    auto input = std::vector<Item> {{1, 2}, {1, 3}, {4, 5}};

    // when
    auto output = transform<Item, int>(input, [](auto &item) { return item.value; });

    // then
    EXPECT_EQ(3ll, output.size());
    EXPECT_EQ(2, output[0]);
    EXPECT_EQ(3, output[1]);
    EXPECT_EQ(5, output[2]);
}

TEST(collection_util, should_group_by_key) {
    // given
    auto input = std::vector<Item> {{1, 2}, {1, 3}, {4, 5}};

    // when
    auto output = groupBy<int, Item>(input, [](auto &item) { return item.key; });

    // then
    EXPECT_EQ(2ll, output.at(1).size());
    EXPECT_EQ(2, output.at(1)[0].value);
    EXPECT_EQ(3, output.at(1)[1].value);
    EXPECT_EQ(1ll, output.at(4).size());
    EXPECT_EQ(5, output.at(4)[0].value);
}

TEST(collection_util, should_group_by_key_and_map_value) {
    // given
    auto input = std::vector<Item> {{1, 2}, {1, 3}, {4, 5}};

    // when
    auto output = groupBy<Item, int, int>(
        input,
        [](auto &item) { return item.key; },
        [](auto &item) { return item.value; });

    // then
    EXPECT_EQ(2ll, output.at(1).size());
    EXPECT_EQ(2, output.at(1)[0]);
    EXPECT_EQ(3, output.at(1)[1]);
    EXPECT_EQ(1ll, output.at(4).size());
    EXPECT_EQ(5, output.at(4)[0]);
}
