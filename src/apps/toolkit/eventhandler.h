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

template <class T>
class EventHandler : boost::noncopyable {
public:
    using Subscriber = std::function<void(const T &)>;

    void invoke(T data) {
        _data = std::move(data);
        for (auto &subscriber : _subscribers) {
            subscriber(_data);
        }
    }

    void subscribe(Subscriber subscriber) {
        _subscribers.push_back(subscriber);
    }

    const T &data() const { return _data; }

private:
    T _data;
    std::list<Subscriber> _subscribers;
};

} // namespace reone
