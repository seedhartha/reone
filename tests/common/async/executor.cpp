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
 *  Tests for Executor class.
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../../../src/engine/common/async/executor.h"
#include "../../../src/engine/common/async/task.h"

using namespace std;

using namespace reone;

static volatile bool g_finished {false};

static void someFunction() {
    this_thread::sleep_for(chrono::milliseconds(100));

    g_finished = true;
}

BOOST_AUTO_TEST_CASE(WhenSubmitAndAwait_ThenBlockUntilCompleted) {
    Executor executor;
    executor.init();

    auto task = executor.submit(&someFunction);
    task->await();

    BOOST_TEST(g_finished);
}
