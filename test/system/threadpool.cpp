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

#include "reone/system/threadpool.h"

using namespace reone;

TEST(thread_pool, should_execute_enqueued_tasks_in_parallel) {
    // given
    ThreadPool pool;
    pool.init();

    // expect
    std::atomic_int shared {0};
    pool.enqueue([&shared]() {
        while (shared != 0) {
            // busy wait
        }
        ++shared;
    });
    pool.enqueue([&shared]() {
        while (shared != 1) {
            // busy wait
        }
        ++shared;
    });
    for (int i = 0; i < 10; ++i) {
        if (static_cast<int>(shared) == 2) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(i));
    }
    EXPECT_EQ(2, static_cast<int>(shared));
}
