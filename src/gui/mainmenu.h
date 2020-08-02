#pragma once

#include "gui.h"

#include "../resources/types.h"

namespace reone {

namespace gui {

class MainMenu : public GUI {
public:
    MainMenu(const render::GraphicsOptions &opts);

    void load(resources::GameVersion version);
    void onClick(const std::string &control) override;

    void setOnNewGame(const std::function<void()> &fn);
    void setOnExit(const std::function<void()> &fn);

private:
    std::function<void()> _onNewGame;
    std::function<void()> _onExit;

    std::string getResRef(resources::GameVersion version) const;
};

} // namespace gui

} // namespace reone
