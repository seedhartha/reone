/*
 * Copyright (c) 2020 The reone project contributors
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
#include <unordered_map>

namespace reone {

/**
 * Map-based timer structure (<T> must be hashable).
 */
template <typename T>
struct TimerMap {
    /* overrides previous obj timeout */
    void setTimeout(const T &obj, uint32_t tick) {
        _timer[obj] = _timestamp + tick;
    }

    /* call once per frame, as soon as possible */
    void update(uint32_t currentTicks) {
        _timestamp = currentTicks;

        for (auto it = _timer.begin(); it != _timer.end(); ) {
            if (it->second < _timestamp) {
                completed.insert(it->first);
                it = _timer.erase(it);
            }
            else ++it;
        }
    }

    bool isRegistered(const T& obj) { return _timer.count(obj) == 1; }

    void cancel(const T& obj) { _timer.erase(obj); }

    /* users are responsible for managing this */
    std::unordered_set<T> completed;

private:
    std::unordered_map<T, uint32_t> _timer;

    uint32_t _timestamp { 0 };
};

} // namespace reone
