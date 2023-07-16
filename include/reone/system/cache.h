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

template <class Key, class Value, class Comparer = std::less<Key>>
class Cache : boost::noncopyable {
public:
    std::shared_ptr<Value> getOrAdd(Key key, std::function<std::shared_ptr<Value>()> valueFactory) {
        auto it = _items.find(key);
        if (it != _items.end()) {
            return it->second;
        }
        auto [inserted, _] = _items.insert(std::make_pair(std::move(key), valueFactory()));
        return inserted->second;
    }

private:
    std::map<Key, std::shared_ptr<Value>, Comparer> _items;
};

} // namespace reone
