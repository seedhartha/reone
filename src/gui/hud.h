#pragma once

#include "gui.h"

#include "../resources/types.h"

namespace reone {

namespace gui {

class HUD : public GUI {
public:
    HUD(const render::GraphicsOptions &opts);

    void load(resources::GameVersion version);
    void update(const HudContext &ctx);

private:
    std::string getResRef(resources::GameVersion version) const;
};

} // namespace gui

} // namespace reone
