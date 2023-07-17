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

#include "reone/system/eventbus.h"

using namespace reone;

enum class EventType {
    Foo,
    Bar
};

struct Event {
    EventType type;
};

TEST(event_bus, should_push_and_poll_event_from_diff_threads) {
    // given
    EventBus<EventType, Event> bus;

    // when
    bus.push(Event {EventType::Foo});

    std::atomic_int timesFoo {0};
    std::atomic_int timesBar {0};
    std::thread pollThread([&bus, &timesFoo, &timesBar]() {
        timesFoo += bus.poll(EventType::Foo) ? 1 : 0;
        timesFoo += bus.poll(EventType::Foo) ? 1 : 0;
        timesBar += bus.poll(EventType::Bar) ? 1 : 0;
    });
    if (pollThread.joinable()) {
        pollThread.join();
    }

    // then
    EXPECT_EQ(timesFoo, 1);
    EXPECT_EQ(timesBar, 0);
}
