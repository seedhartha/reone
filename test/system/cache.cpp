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

#include "reone/system/cache.h"
#include "reone/system/types.h"

using namespace reone;

struct CompositeKey {
    int first;
    std::string second;
};

struct CompositeKeyComparer {
    bool operator()(const CompositeKey &lhs, const CompositeKey &rhs) const {
        if (lhs.first < rhs.first) {
            return true;
        } else if (lhs.first > rhs.first) {
            return false;
        }
        return lhs.second < rhs.second;
    }
};

TEST(cache, should_create_and_cache_value_when_get_called_twice_with_the_same_key) {
    // given
    Cache<std::string, int> cache;
    int counter = 0;
    auto valueFactory = [&counter]() { return std::make_shared<int>(counter++); };

    // when
    auto value1 = cache.getOrAdd("key1", valueFactory);
    auto value2 = cache.getOrAdd("key2", valueFactory);
    auto value3 = cache.getOrAdd("key1", valueFactory);

    // then
    EXPECT_TRUE(value1 && ((*value1) == 0));
    EXPECT_TRUE(value2 && ((*value2) == 1));
    EXPECT_TRUE(value3 && ((*value3) == 0));
}

TEST(cache, should_take_optional_comparer) {
    // given
    Cache<CompositeKey, int, CompositeKeyComparer> cache;
    int counter = 0;
    auto valueFactory = [&counter]() { return std::make_shared<int>(counter++); };

    // when
    auto value1 = cache.getOrAdd(CompositeKey {0, "A"}, valueFactory);
    auto value2 = cache.getOrAdd(CompositeKey {0, "B"}, valueFactory);
    auto value3 = cache.getOrAdd(CompositeKey {1, "A"}, valueFactory);
    auto value4 = cache.getOrAdd(CompositeKey {0, "A"}, valueFactory);

    // then
    EXPECT_TRUE(value1 && ((*value1) == 0));
    EXPECT_TRUE(value2 && ((*value2) == 1));
    EXPECT_TRUE(value3 && ((*value3) == 2));
    EXPECT_TRUE(value4 && ((*value4) == 0));
}
