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
class Property : boost::noncopyable {
public:
    using ChangedHandler = std::function<void()>;

    inline T &operator*() {
        return _value;
    }

    inline T *operator->() {
        return &_value;
    }

    Property<T> &operator=(T value) {
        _value = std::move(value);
        for (auto &handler : _changedHandlers) {
            handler();
        }
        return *this;
    }

    inline void addChangedHandler(ChangedHandler handler) {
        _changedHandlers.push_back(std::move(handler));
    }

private:
    T _value;
    std::list<ChangedHandler> _changedHandlers;
};

} // namespace reone
