/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../../core/log.h"
#include "../../render/mesh/guiquad.h"
#include "../../render/shadermanager.h"
#include "../../resources/manager.h"

#include "button.h"
#include "scrollbar.h"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

static const int kItemPadding = 3;

ListBox::ListBox() : Control(ControlType::ListBox) {
}

ListBox::ListBox(const string &tag) : Control(ControlType::ListBox, tag) {
}

void ListBox::loadCustom() {
    Control::Border border;
    border.fill = ResMan.findTexture("brightblue", TextureType::Diffuse);

    Control::Text text;
    text.font = ResMan.findFont("fnt_d16x16b");

    unique_ptr<Control> protoItem(new Button());
    protoItem->setExtent(Control::Extent(0, 0, _extent.width, _text.font->height() + 2 * kItemPadding));
    protoItem->setBorder(border);
    protoItem->setText(text);
    _protoItem = move(protoItem);

    _padding = 3;
    updateItems();
}

void ListBox::updateItems() {
    if (!_protoItem) return;

    _slotCount = _extent.height / (_protoItem->extent().height + _padding);

    if (_scrollBar) {
        _scrollBar->setVisible(_items.size() > _slotCount);
    }
}

void ListBox::add(const Item &item) {
    _items.push_back(item);
    updateItems();
}

void ListBox::load(const GffStruct &gffs) {
    Control::load(gffs);

    const GffField *protoItem = gffs.find("PROTOITEM");
    if (protoItem) {
        _protoItem = makeControl(protoItem->asStruct());
        updateItems();
    }

    const GffField *scrollBar = gffs.find("SCROLLBAR");
    if (scrollBar) {
        _scrollBar = makeControl(scrollBar->asStruct());
    }
}

bool ListBox::handleMouseMotion(int x, int y) {
    _hilightedIndex = getItemIndex(y);
    return false;
}

int ListBox::getItemIndex(int y) const {
    const Control::Extent &protoExtent = _protoItem->extent();
    int idx = (y - _extent.top - protoExtent.top) / (protoExtent.height + _padding) + _itemOffset;
    return idx >= 0 && idx < _items.size() ? idx : -1;
}

bool ListBox::handleMouseWheel(int x, int y) {
    if (y < 0) {
        if (_itemOffset < _items.size() - _slotCount) _itemOffset++;
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

    if (_onItemClicked) {
        _onItemClicked(_tag, _items[itemIdx].tag);
        return true;
    }

    return false;
}

void ListBox::initGL() {
    Control::initGL();

    if (_protoItem) _protoItem->initGL();
    if (_scrollBar) _scrollBar->initGL();
}

void ListBox::render(const glm::mat4 &transform, const std::string &textOverride) const {
    if (!_visible) return;

    Control::render(transform);

    if (!_protoItem) return;

    const Control::Extent &protoExtent = _protoItem->extent();
    glm::mat4 itemTransform(glm::translate(transform, glm::vec3(_extent.left, _extent.top - protoExtent.top, 0.0f)));

    for (int i = 0; i < _items.size() && i < _slotCount; ++i) {
        int itemIdx = i + _itemOffset;
        _protoItem->setFocus(_hilightedIndex == itemIdx);
        _protoItem->render(itemTransform, _items[itemIdx].text);
        itemTransform = glm::translate(itemTransform, glm::vec3(0.0f, protoExtent.height + _padding, 0.0f));
    }

    if (_scrollBar) {
        ScrollBar &scrollBar = static_cast<ScrollBar &>(*_scrollBar);
        scrollBar.setCanScrollUp(_itemOffset > 0);
        scrollBar.setCanScrollDown(_itemOffset + _slotCount < _items.size());
        scrollBar.render(transform, "");
    }
}

void ListBox::resize(float scaleX, float scaleY) {
    assert(_protoItem);
    _protoItem->resize(scaleX, 1.0f);

    if (_scrollBar) {
        _scrollBar->resize(1.0f, scaleY);
    }

    Control::resize(scaleX, scaleY);
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

Control &ListBox::protoItem() const {
    return *_protoItem;
}

Control &ListBox::scrollBar() const {
    return *_scrollBar;
}

} // namespace gui

} // namespace reone
