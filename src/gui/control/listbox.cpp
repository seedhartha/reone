/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "listbox.h"

#include "reone/resource/gff.h"

#include "../guiinterface.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace gui {

void ListBox::load(const Gff &gui, const glm::vec4 &scale) {
    Control::load(gui, scale);

    _padding = gui.getInt("PADDING");

    auto guiProtoItem = gui.getStruct("PROTOITEM");
    auto protoItem = _gui.loadControl(*guiProtoItem, scale, itemControlId(_id, -1));
    _protoItem = protoItem.get();

    initItemSlots();
}

void ListBox::initItemSlots() {
    auto &protoExtent = _protoItem->extent();
    int protoHeightPadded = protoExtent[3] + _padding;
    _numItemSlots = _extent[3] / protoHeightPadded;

    for (int i = 0; i < _numItemSlots; ++i) {
        auto item = _protoItem->copy(itemControlId(_id, i));
        auto extent = glm::vec4(protoExtent[0], protoExtent[1] + i * protoHeightPadded, protoExtent[2], protoExtent[3]);
        item->setEnabled(false);
        item->setExtent(move(extent));
        _itemSlots.push_back(item.get());
        _children.push_back(item.get());
    }
}

void ListBox::clearItems() {
    _items.clear();
    _itemSlotOffset = 0;
    flushItemSlots();
}

void ListBox::appendItem(Item item, bool scroll) {
    _items.push_back(move(item));
    if (_items.size() > _numItemSlots) {
        ++_itemSlotOffset;
    }
    flushItemSlots();
}

bool ListBox::handle(const SDL_Event &e) {
    if (!_enabled) {
        return false;
    }
    if (e.type == SDL_MOUSEWHEEL && isInFocusRecursive()) {
        _itemSlotOffset = clamp(_itemSlotOffset - e.wheel.y, 0, static_cast<int>(_items.size()) - _numItemSlots);
        flushItemSlots();
    }
    return false;
}

void ListBox::flushItemSlots() {
    for (size_t i = 0; i < _itemSlots.size(); ++i) {
        if (i < _items.size() - _itemSlotOffset) {
            _itemSlots[i]->setEnabled(true);
            _itemSlots[i]->setText(_items[i + _itemSlotOffset].text);
        } else {
            _itemSlots[i]->setEnabled(false);
        }
    }
}

} // namespace gui

} // namespace reone
