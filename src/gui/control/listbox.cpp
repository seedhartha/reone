/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../../render/mesh/quad.h"
#include "../../render/shaders.h"
#include "../../resource/resources.h"
#include "../../system/log.h"

#include "../gui.h"

#include "button.h"
#include "imagebutton.h"
#include "scrollbar.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace gui {

static const int kItemPadding = 3;

ListBox::ListBox(GUI *gui) : Control(gui, ControlType::ListBox) {
    _clickable = true;
}

void ListBox::updateItems() {
    if (!_protoItem) return;

    _slotCount = _extent.height / (_protoItem->extent().height + _padding);

    if (_scrollBar) {
        _scrollBar->setVisible(_items.size() > _slotCount);
    }
}

void ListBox::clear() {
    _items.clear();
    _itemOffset = 0;
    updateItems();
}

void ListBox::add(Item item) {
    _items.push_back(move(item));
    updateItems();
}

void ListBox::load(const GffStruct &gffs) {
    Control::load(gffs);

    const GffField *protoItem = gffs.find("PROTOITEM");
    if (protoItem) {
        const GffStruct &protoGffs = protoItem->asStruct();
        ControlType type = _protoItemType == ControlType::Invalid ? getType(protoGffs) : _protoItemType;
        _protoItem = of(_gui, type, getTag(protoGffs));
        _protoItem->load(protoGffs);
        updateItems();
    }

    const GffField *scrollBar = gffs.find("SCROLLBAR");
    if (scrollBar) {
        const GffStruct &scrollGffs = scrollBar->asStruct();
        _scrollBar = of(_gui, getType(scrollGffs), getTag(scrollGffs));
        _scrollBar->load(scrollGffs);
    }
}

bool ListBox::handleMouseMotion(int x, int y) {
    _hilightedIndex = getItemIndex(y);
    return false;
}

int ListBox::getItemIndex(int y) const {
    if (!_protoItem) return -1;

    const Control::Extent &protoExtent = _protoItem->extent();
    int idx = (y - protoExtent.top) / (protoExtent.height + _padding) + _itemOffset;

    return idx >= 0 && idx < _items.size() ? idx : -1;
}

bool ListBox::handleMouseWheel(int x, int y) {
    if (y < 0) {
        if (_items.size() - _itemOffset > _slotCount) _itemOffset++;
        return true;
    } else if (y > 0) {
        if (_itemOffset > 0) _itemOffset--;
        return true;
    }

    return false;
}

bool ListBox::handleClick(int x, int y) {
    int itemIdx = getItemIndex(y);
    if (itemIdx == -1) return false;

    _gui->onListBoxItemClick(_tag, _items[itemIdx].tag);

    return true;
}

void ListBox::render(const glm::ivec2 &offset, const string &textOverride) const {
    if (!_visible) return;

    Control::render(offset, textOverride);

    if (!_protoItem) return;

    glm::vec2 itemOffset(offset);
    const Control::Extent &protoExtent = _protoItem->extent();

    for (int i = 0; i < _slotCount; ++i) {
        int itemIdx = i + _itemOffset;
        if (itemIdx >= _items.size()) break;

        _protoItem->setFocus(_hilightedIndex == itemIdx);

        ImageButton *imageButton = dynamic_cast<ImageButton *>(_protoItem.get());
        if (imageButton) {
            imageButton->render(itemOffset, _items[itemIdx].text, _items[itemIdx].icon);
        } else {
            _protoItem->render(itemOffset, _items[itemIdx].text);
        }

        itemOffset.y += protoExtent.height + _padding;
    }

    if (_scrollBar) {
        ScrollBar &scrollBar = static_cast<ScrollBar &>(*_scrollBar);
        scrollBar.setCanScrollUp(_itemOffset > 0);
        scrollBar.setCanScrollDown(_items.size() - _itemOffset > _slotCount);
        scrollBar.render(offset, textOverride);
    }
}

void ListBox::stretch(float x, float y) {
    Control::stretch(x, y);

    if (_protoItem) _protoItem->stretch(x, 1.0f);
    if (_scrollBar) _scrollBar->stretch(1.0f, y);
}

void ListBox::setFocus(bool focus) {
    Control::setFocus(focus);
    if (!focus) {
        _hilightedIndex = -1;
    }
}

void ListBox::setExtent(const Extent &extent) {
    Control::setExtent(extent);
    updateItems();
}

void ListBox::setProtoItemType(ControlType type) {
    _protoItemType = type;
}

const ListBox::Item &ListBox::getItemAt(int index) const {
    return _items[index];
}

Control &ListBox::protoItem() const {
    return *_protoItem;
}

Control &ListBox::scrollBar() const {
    return *_scrollBar;
}

int ListBox::itemCount() const {
    return static_cast<int>(_items.size());
}

} // namespace gui

} // namespace reone
