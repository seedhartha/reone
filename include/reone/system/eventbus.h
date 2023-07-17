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

#pragma once

namespace reone {

template <class EventType, class Event>
class EventBus : boost::noncopyable {
public:
    void push(Event event) {
        static_assert(std::is_same_v<decltype(event.type), EventType>);
        std::lock_guard<std::mutex> lock {_mutex};
        _typeToEvents[event.type].push(std::move(event));
    }

    std::optional<Event> poll(EventType type) {
        std::lock_guard<std::mutex> lock {_mutex};
        auto it = _typeToEvents.find(type);
        if (it == _typeToEvents.end()) {
            return std::nullopt;
        }
        auto &events = it->second;
        if (events.empty()) {
            return std::nullopt;
        }
        auto event = events.front();
        events.pop();
        return event;
    }

private:
    std::mutex _mutex;
    std::map<EventType, std::queue<Event>> _typeToEvents;
};

} // namespace reone
