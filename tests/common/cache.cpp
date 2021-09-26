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

/** @file
 *  Tests for MemoryCache class.
 */

#include <boost/test/unit_test.hpp>

#include "../../src/engine/common/cache.h"

using namespace std;
using namespace std::placeholders;

using namespace reone;

class TestCache : public MemoryCache<int, int> {
public:
    TestCache() :
        MemoryCache(bind(&TestCache::compute, this, _1)) {
    }

private:
    int _counter {0};

    shared_ptr<int> compute(int key) {
        return make_shared<int>(_counter++);
    }
};

BOOST_AUTO_TEST_CASE(GivenValueIncremented_WhenGetTwiceUsingSameKey_ThenSameValueReturned) {
    TestCache cache;

    auto result1 = cache.get(0);
    auto result2 = cache.get(0);

    BOOST_TEST((*result1 == 0));
    BOOST_TEST((*result2 == 0));
}

BOOST_AUTO_TEST_CASE(GivenValueIncremented_WhenGetTwiceUsingDifferentKeys_ThenDifferentValuesReturned) {
    TestCache cache;

    auto result1 = cache.get(0);
    auto result2 = cache.get(1);

    BOOST_TEST((*result1 == 0));
    BOOST_TEST((*result2 == 1));
}
