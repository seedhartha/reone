#pragma once

#include "gui.h"

namespace reone {

namespace gui {

class DebugGui : public GUI {
public:
    DebugGui(const render::GraphicsOptions &opts);

    void load();
    void update(const DebugContext &ctx);
};

} // namespace gui

} // namespace reone
