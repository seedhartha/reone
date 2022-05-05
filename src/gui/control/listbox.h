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

#pragma once

#include "../control.h"

namespace reone {

namespace gui {

class ListBox : public Control {
public:
    struct Item {
        std::string text;
    };

    ListBox(
        int id,
        IControlFactory &controlFactory,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Control(
            id,
            ControlType::ListBox,
            controlFactory,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {

        _focusable = true;
    }

    void load(const resource::Gff &gui, const glm::vec4 &scale) override;

    bool handle(const SDL_Event &e) override;

    const Item &itemBySlotIndex(int index) const {
        return _items[index + _itemSlotOffset];
    }

    void setItems(std::vector<Item> items) {
        _items = std::move(items);
        _itemSlotOffset = 0;

        flushItemSlots();
    }

    static int itemControlId(int listBoxId, int slotIdx) {
        return ((listBoxId + 1) << 8) | (slotIdx + 1);
    }

    static bool isListBoxItem(Control &control) {
        return control.id() & 0xff00;
    }

    static int listBoxIdFromControl(Control &control) {
        return ((control.id() & 0xff00) >> 8) - 1;
    }

    static int itemSlotIndexFromControl(Control &control) {
        return (control.id() & 0xff) - 1;
    }

private:
    int _padding {0};
    int _numItemSlots {0};
    int _itemSlotOffset {0};

    std::vector<Control *> _itemSlots;

    std::vector<Item> _items;

    void flushItemSlots();
};

} // namespace gui

} // namespace reone
