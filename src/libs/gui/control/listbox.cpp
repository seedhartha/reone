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

#include "reone/gui/control/listbox.h"

#include "reone/graphics/font.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textutil.h"
#include "reone/resource/gff.h"
#include "reone/resource/resources.h"
#include "reone/system/logutil.h"

#include "reone/gui/gui.h"

#include "reone/gui/control/button.h"
#include "reone/gui/control/imagebutton.h"
#include "reone/gui/control/scrollbar.h"

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
    if (!_protoItem)
        return;

    item._textLines = breakText(item.text, *_protoItem->text().font, _protoItem->extent().width);
    _items.push_back(item);

    updateItemSlots();
}

void ListBox::addTextLinesAsItems(const std::string &text) {
    if (!_protoItem)
        return;

    std::vector<std::string> lines(breakText(text, *_protoItem->text().font, _protoItem->extent().width));
    for (auto &line : lines) {
        Item item;
        item.text = line;
        item._textLines = std::vector<std::string> {line};
        _items.push_back(std::move(item));
    }

    updateItemSlots();
}

void ListBox::clearSelection() {
    _selectedItemIndex = -1;
}

void ListBox::load(const resource::generated::GUI_BASECONTROL &gui, bool protoItem) {
    Control::load(gui, protoItem);

    auto &controlStruct = *static_cast<const resource::generated::GUI_CONTROLS *>(&gui);
    if (controlStruct.PROTOITEM) {
        _protoItem = _gui.newControl(getType(*controlStruct.PROTOITEM), getTag(*controlStruct.PROTOITEM));
        _protoItem->load(*controlStruct.PROTOITEM, true);
    }
    if (controlStruct.SCROLLBAR) {
        _scrollBar = _gui.newControl(getType(*controlStruct.SCROLLBAR), getTag(*controlStruct.SCROLLBAR));
        _scrollBar->load(*controlStruct.SCROLLBAR);
    }
}

bool ListBox::handleMouseMotion(int x, int y) {
    if (_selectionMode == SelectionMode::OnHover) {
        _selectedItemIndex = getItemIndex(y);
    }
    return false;
}

int ListBox::getItemIndex(int y) const {
    if (!_protoItem)
        return -1;

    const Control::Extent &protoExtent = _protoItem->extent();
    if (protoExtent.height == 0)
        return -1;

    float itemy = static_cast<float>(protoExtent.top);
    if (y < itemy)
        return -1;

    for (size_t i = _itemOffset; i < _items.size(); ++i) {
        const Item &item = _items[i];
        if (_protoMatchContent) {
            itemy += item._textLines.size() * _protoItem->text().font->height();
        } else {
            itemy += protoExtent.height;
        }
        itemy += _padding;
        if (y < itemy)
            return static_cast<int>(i);
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

        if (y > _extent.height)
            break;

        ++_slotCount;
    }

    if (_scrollBar) {
        _scrollBar->setVisible(_items.size() > _slotCount);
    }
}

bool ListBox::handleClick(int x, int y) {
    int itemIdx = getItemIndex(y);
    if (itemIdx == -1)
        return false;

    if (_selectionMode == SelectionMode::OnClick) {
        _selectedItemIndex = itemIdx;
    }
    if (_onItemClick) {
        _onItemClick(_items[itemIdx].tag);
    }

    return true;
}

void ListBox::draw(const glm::ivec2 &screenSize, const glm::ivec2 &offset) {
    if (!_visible)
        return;

    Control::draw(screenSize, offset);

    if (!_protoItem)
        return;

    glm::vec2 itemOffset(offset);

    for (int i = 0; i < _slotCount; ++i) {
        int itemIdx = i + _itemOffset;
        if (itemIdx >= _items.size())
            break;

        const Item &item = _items[itemIdx];
        if (_protoMatchContent) {
            _protoItem->setHeight(static_cast<int>(item._textLines.size() * (_protoItem->text().font->height() + _padding)));
        }
        _protoItem->setSelected(_selectedItemIndex == itemIdx);

        auto imageButton = std::dynamic_pointer_cast<ImageButton>(_protoItem);
        if (imageButton) {
            imageButton->draw(itemOffset, item._textLines, item.iconText, item.iconTexture, item.iconFrame);
        } else {
            _protoItem->setTextLines(item._textLines);
            _protoItem->draw(screenSize, itemOffset);
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
        scrollBar.setScrollState(std::move(state));
        scrollBar.draw(screenSize, offset);
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

void ListBox::setSelected(bool selected) {
    Control::setSelected(selected);
    if (!selected && _selectionMode == SelectionMode::OnHover) {
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

void ListBox::changeProtoItemType(ControlType type) {
    // TODO: debug CTD
    return;

    if (!_protoItem) {
        return;
    }
    auto &oldProtoItem = *_protoItem;

    _protoItem = _gui.newControl(type, oldProtoItem.tag());
    _protoItem->setId(oldProtoItem.id());
    _protoItem->setPadding(oldProtoItem.padding());
    _protoItem->setExtent(oldProtoItem.extent());
    _protoItem->setBorder(oldProtoItem.border());
    _protoItem->setText(oldProtoItem.text());
    _protoItem->setHilight(oldProtoItem.hilight());

    _protoItem->updateTextLines();
    _protoItem->updateTransform();
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
