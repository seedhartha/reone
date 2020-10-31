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

#pragma once

#include <vector>

#include "control.h"

namespace reone {

namespace gui {

static const int kDefaultSlotCount = 6;

class ListBox : public Control {
public:
    struct Item {
        std::string tag;
        std::string text;
        std::shared_ptr<render::Texture> icon;
    };

    ListBox(GUI *gui);

    void clear();
    void add(Item item);

    void load(const resource::GffStruct &gffs) override;
    bool handleMouseMotion(int x, int y) override;
    bool handleMouseWheel(int x, int y) override;
    bool handleClick(int x, int y) override;
    void render(const glm::ivec2 &offset, const std::string &textOverride) const override;
    void stretch(float x, float y) override;

    void setFocus(bool focus) override;
    void setExtent(const Extent &extent) override;
    void setProtoItemType(ControlType type);

    const Item &getItemAt(int index) const;

    Control &protoItem() const;
    Control &scrollBar() const;
    int itemCount() const;

private:
    ControlType _protoItemType { ControlType::Invalid };
    std::shared_ptr<Control> _protoItem;
    std::shared_ptr<Control> _scrollBar;
    std::vector<Item> _items;
    int _slotCount { 0 };
    int _itemOffset { 0 };
    int _hilightedIndex { -1 };
    int _itemMargin { 0 };

    void updateItems();
    int getItemIndex(int y) const;
};

} // namespace gui

} // namespace reone
