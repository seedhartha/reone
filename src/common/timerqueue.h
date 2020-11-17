/*
 * Copyright (c) 2020 Vsevolod Kremianskii
 * Copyright (c) 2020 uwadmin12
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

#include <cstdint>
#include <list>
#include <queue>
#include <unordered_set>
#include <vector>

namespace reone {

/**
 * Queue-based timer structure.
 */
template <typename T>
struct TimerQueue {
    /* does not override previous obj */
    void setTimeout(const T &obj, uint32_t tick) {
        _timer.push(std::make_pair(_timestamp + tick, obj));
    }

    /* call once per frame, as soon as possible */
    void update(uint32_t currentTicks) {
        _timestamp = currentTicks;

        while (!_timer.empty() && _timer.top().first < _timestamp) {
            completed.push_back(_timer.top().second);
            _timer.pop();
        }
    }

    /* users are responsible for managing this */
    std::list<T> completed;

private:
    struct TimerPairCompare {
        constexpr bool operator()(std::pair<uint32_t, T> const& a,
            std::pair<uint32_t, T> const& b) const noexcept {
            return a.first > b.first;
        } // min heap!
    };

    std::priority_queue<std::pair<uint32_t, T>,
                        std::vector<std::pair<uint32_t, T>>,
                        TimerPairCompare> _timer;

    uint32_t _timestamp { 0 };
};

} // namespace reone
