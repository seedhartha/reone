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

#include <boost/test/unit_test.hpp>

#include "../../../src/engine/common/async/executor.h"
#include "../../../src/engine/common/async/task.h"

using namespace reone;

static void doNothing() {
}

BOOST_AUTO_TEST_CASE(ExecutorTest) {
    Executor executor;
    executor.init();

    auto task = executor.submit(&doNothing);
    task->await();
}
