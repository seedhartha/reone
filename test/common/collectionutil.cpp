/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include <boost/test/unit_test.hpp>

#include "../../src/common/collectionutil.h"

using namespace std;

using namespace reone;

struct Item {
    int key {0};
    int value {0};
};

BOOST_AUTO_TEST_SUITE(collection_util)

BOOST_AUTO_TEST_CASE(should_transform) {
    // given
    auto input = vector<Item> {{1, 2}, {1, 3}, {4, 5}};

    // when
    auto output = transform<Item, int>(input, [](auto &item) { return item.value; });

    // then
    BOOST_TEST(3ll == output.size());
    BOOST_TEST(2 == output[0]);
    BOOST_TEST(3 == output[1]);
    BOOST_TEST(5 == output[2]);
}

BOOST_AUTO_TEST_CASE(should_group_by_key) {
    // given
    auto input = vector<Item> {{1, 2}, {1, 3}, {4, 5}};

    // when
    auto output = groupBy<int, Item>(input, [](auto &item) { return item.key; });

    // then
    BOOST_TEST(2ll == output.at(1).size());
    BOOST_TEST(2 == output.at(1)[0].value);
    BOOST_TEST(3 == output.at(1)[1].value);
    BOOST_TEST(1ll == output.at(4).size());
    BOOST_TEST(5 == output.at(4)[0].value);
}

BOOST_AUTO_TEST_CASE(should_group_by_key_and_map_value) {
    // given
    auto input = vector<Item> {{1, 2}, {1, 3}, {4, 5}};

    // when
    auto output = groupBy<Item, int, int>(
        input,
        [](auto &item) { return item.key; },
        [](auto &item) { return item.value; });

    // then
    BOOST_TEST(2ll == output.at(1).size());
    BOOST_TEST(2 == output.at(1)[0]);
    BOOST_TEST(3 == output.at(1)[1]);
    BOOST_TEST(1ll == output.at(4).size());
    BOOST_TEST(5 == output.at(4)[0]);
}

BOOST_AUTO_TEST_SUITE_END()
