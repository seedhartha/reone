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
