#pragma once

#include "gui.h"

namespace reone {

namespace gui {

class ModulesGui : public GUI {
public:
    ModulesGui(const render::GraphicsOptions &opts);

    void load();
    void onItemClicked(const std::string &control, const std::string &item) override;

    void setOnModuleSelected(const std::function<void(const std::string &)> &fn);

private:
    std::function<void(const std::string &name)> _onModuleSelected;

    void loadLabel();
    void loadListBox();
};

} // namespace gui

} // namespace reone
