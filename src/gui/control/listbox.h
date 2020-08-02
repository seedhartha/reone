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
    };

    ListBox();
    ListBox(const std::string &tag);

    void loadCustom();
    void add(const Item &item);

    bool handleMouseWheel(int x, int y) override;
    bool handleClick(int x, int y) override;
    void initGL() override;
    void render(const glm::mat4 &transform) const override;

private:
    std::shared_ptr<Control> _protoItem;
    std::vector<Item> _items;
    int _slotCount { 0 };
    int _itemOffset { 0 };
};

} // namespace gui

} // namespace reone
