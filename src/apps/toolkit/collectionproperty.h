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

#include "reone/system/checkutil.h"

namespace reone {

enum class CollectionChangeType {
    Add,
    Remove,
    Reset
};

template <class T>
class CollectionProperty : boost::noncopyable {
public:
    struct ChangedArgs {
        CollectionChangeType type;
        std::optional<std::reference_wrapper<const T>> addedItem;
        std::optional<std::reference_wrapper<const T>> removedItem;
        std::optional<size_t> removedItemIdx;

        ChangedArgs(CollectionChangeType type,
                    std::optional<std::reference_wrapper<const T>> addedItem = std::nullopt,
                    std::optional<std::reference_wrapper<const T>> removedItem = std::nullopt,
                    std::optional<size_t> removedItemIdx = std::nullopt) :
            type(type),
            addedItem(std::move(addedItem)),
            removedItem(std::move(removedItem)),
            removedItemIdx(std::move(removedItemIdx)) {
            checkThat(addedItem || type != CollectionChangeType::Add, "addedItem must be present");
            checkThat(removedItem || type != CollectionChangeType::Remove, "removedItem must be present");
            checkThat(removedItemIdx || type != CollectionChangeType::Remove, "removedItemIdx must be present");
        }
    };

    using ChangedHandler = std::function<void(const ChangedArgs &)>;

    T &at(size_t index) {
        checkLess("index", index, _items.size());
        auto it = _items.begin();
        std::advance(it, index);
        return *it;
    }

    void add(T item) {
        _items.push_back(std::move(item));
        notifyChangedHandlers({CollectionChangeType::Add, _items.back()});
    }

    void removeAt(size_t index) {
        checkLess("index", index, _items.size());
        auto it = _items.begin();
        std::advance(it, index);
        auto &item = *it;
        notifyChangedHandlers({CollectionChangeType::Remove, std::nullopt, item, index});
        it = _items.erase(it);
    }

    void removeIf(std::function<bool(const T &)> pred) {
        for (auto it = _items.begin(); it != _items.end();) {
            auto &item = *it;
            if (!pred(item)) {
                ++it;
                continue;
            }
            notifyChangedHandlers({CollectionChangeType::Remove, std::nullopt, item, std::distance(_items.begin(), it)});
            it = _items.erase(it);
        }
    }

    inline const std::list<T> &operator*() const {
        return _items;
    }

    inline const std::list<T> *operator->() const {
        return &_items;
    }

    inline CollectionProperty<T> &operator=(std::list<T> items) {
        _items = std::move(items);
        ChangedArgs args {CollectionChangeType::Reset};
        notifyChangedHandlers(std::move(args));
        return *this;
    }

    inline void addChangedHandler(ChangedHandler handler) {
        _changedHandlers.push_back(std::move(handler));
    }

private:
    std::list<T> _items;
    std::list<ChangedHandler> _changedHandlers;

    void notifyChangedHandlers(ChangedArgs args) {
        for (auto &handler : _changedHandlers) {
            handler(args);
        }
    }
};

} // namespace reone
