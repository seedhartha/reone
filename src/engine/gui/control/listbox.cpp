/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../common/logutil.h"
#include "../../graphics/font.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/textutil.h"
#include "../../resource/gffstruct.h"
#include "../../resource/resources.h"

#include "../gui.h"

#include "button.h"
#include "imagebutton.h"
#include "scrollbar.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

static constexpr int kItemPadding = 3;

void ListBox::clearItems() {
    _items.clear();
    _itemOffset = 0;
    _selectedItemIndex = -1;
}

void ListBox::addItem(Item &&item) {
    if (!_protoItem) return;

    item._textLines = breakText(item.text, *_protoItem->text().font, _protoItem->extent().width);
    _items.push_back(item);

    updateItemSlots();
}

void ListBox::addTextLinesAsItems(const string &text) {
    if (!_protoItem) return;

    vector<string> lines(breakText(text, *_protoItem->text().font, _protoItem->extent().width));
    for (auto &line : lines) {
        Item item;
        item.text = line;
        item._textLines = vector<string> { line };
        _items.push_back(move(item));
    }

    updateItemSlots();
}

void ListBox::clearSelection() {
    _selectedItemIndex = -1;
}

void ListBox::load(const GffStruct &gffs) {
    Control::load(gffs);

    shared_ptr<GffStruct> protoItem(gffs.getStruct("PROTOITEM"));
    if (protoItem) {
        ControlType type = _protoItemType == ControlType::Invalid ? getType(*protoItem) : _protoItemType;
        _protoItem = _gui.newControl(type, getTag(*protoItem));
        _protoItem->load(*protoItem);
    }
    shared_ptr<GffStruct> scrollBar(gffs.getStruct("SCROLLBAR"));
    if (scrollBar) {
        _scrollBar = _gui.newControl(getType(*scrollBar), getTag(*scrollBar));
        _scrollBar->load(*scrollBar);
    }
}

bool ListBox::handleMouseMotion(int x, int y) {
    if (_selectionMode == SelectionMode::OnHover) {
        _selectedItemIndex = getItemIndex(y);
    }
    return false;
}

int ListBox::getItemIndex(int y) const {
    if (!_protoItem) return -1;

    const Control::Extent &protoExtent = _protoItem->extent();
    if (protoExtent.height == 0) return -1;

    float itemy = static_cast<float>(protoExtent.top);
    if (y < itemy) return -1;

    for (size_t i = _itemOffset; i < _items.size(); ++i) {
        const Item &item = _items[i];
        if (_protoMatchContent) {
            itemy += item._textLines.size() * _protoItem->text().font->height();
        } else {
            itemy += protoExtent.height;
        }
        itemy += _padding;
        if (y < itemy) return static_cast<int>(i);
    }

    return -1;
}

bool ListBox::handleMouseWheel(int x, int y) {
    if (y < 0) {
        if (_items.size() - _itemOffset > _slotCount) {
            _itemOffset++;
            updateItemSlots();
        }
        return true;
    } else if (y > 0) {
        if (_itemOffset > 0) {
            _itemOffset--;
            updateItemSlots();
        }
        return true;
    }

    return false;
}

void ListBox::updateItemSlots() {
    _slotCount = 0;

    // Increase the number of slots until they no longer fit vertically
    float y = 0.0f;
    for (size_t i = _itemOffset; i < _items.size(); ++i) {
        if (_protoMatchContent) {
            y += _items[i]._textLines.size() * _protoItem->text().font->height();
        } else {
            y += _protoItem->extent().height;
        }
        y += _padding;

        if (y > _extent.height) break;

        ++_slotCount;
    }

    if (_scrollBar) {
        _scrollBar->setVisible(_items.size() > _slotCount);
    }
}

bool ListBox::handleClick(int x, int y) {
    int itemIdx = getItemIndex(y);
    if (itemIdx == -1) return false;

    if (_selectionMode == SelectionMode::OnClick) {
        _selectedItemIndex = itemIdx;
    }
    if (_onItemClick) {
        _onItemClick(_items[itemIdx].tag);
    }

    return true;
}

void ListBox::draw(const glm::ivec2 &offset, const vector<string> &text) {
    if (!_visible) return;

    Control::draw(offset, text);

    if (!_protoItem) return;

    glm::vec2 itemOffset(offset);

    for (int i = 0; i < _slotCount; ++i) {
        int itemIdx = i + _itemOffset;
        if (itemIdx >= _items.size()) break;

        const Item &item = _items[itemIdx];
        if (_protoMatchContent) {
            _protoItem->setHeight(static_cast<int>(item._textLines.size() * (_protoItem->text().font->height() + _padding)));
        }
        _protoItem->setFocus(_selectedItemIndex == itemIdx);

        auto imageButton = dynamic_pointer_cast<ImageButton>(_protoItem);
        if (imageButton) {
            imageButton->draw(itemOffset, item._textLines, item.iconText, item.iconTexture, item.iconFrame);
        } else {
            _protoItem->draw(itemOffset, item._textLines);
        }

        if (_protoMatchContent) {
            itemOffset.y += item._textLines.size() * (_protoItem->text().font->height() + _padding);
        } else {
            itemOffset.y += _protoItem->extent().height + _padding;
        }
    }

    if (_scrollBar) {
        ScrollBar::ScrollState state;
        state.count = static_cast<int>(_items.size());
        state.numVisible = this->_slotCount;
        state.offset = _itemOffset;
        auto &scrollBar = static_cast<ScrollBar &>(*_scrollBar);
        scrollBar.setScrollState(move(state));
        scrollBar.draw(offset, vector<string>());
    }
}

void ListBox::stretch(float x, float y, int mask) {
    Control::stretch(x, y, mask);

    if (_protoItem) {
        // Do not change height of the proto item
        _protoItem->stretch(x, y, mask & ~kStretchHeight);
    }
    if (_scrollBar) {
        // Do not change width of the scroll bar
        _scrollBar->stretch(x, y, mask & ~kStretchWidth);
    }
}

void ListBox::setFocus(bool focus) {
    Control::setFocus(focus);
    if (!focus && _selectionMode == SelectionMode::OnHover) {
        _selectedItemIndex = -1;
    }
}

void ListBox::setExtent(Extent extent) {
    Control::setExtent(extent);
    updateItemSlots();
}

void ListBox::setExtentHeight(int height) {
    Control::setExtentHeight(height);
    updateItemSlots();
}

void ListBox::setProtoItemType(ControlType type) {
    _protoItemType = type;
}

void ListBox::setSelectionMode(SelectionMode mode) {
    _selectionMode = mode;
}

void ListBox::setProtoMatchContent(bool match) {
    _protoMatchContent = match;
}

const ListBox::Item &ListBox::getItemAt(int index) const {
    return _items[index];
}

int ListBox::getItemCount() const {
    return static_cast<int>(_items.size());
}

} // namespace gui

} // namespace reone
