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

#include "../../gui/gui.h"
#include "../../resources/types.h"

#include "../types.h"

namespace reone {

namespace game {

class PortraitsGui : public gui::GUI {
public:
    PortraitsGui(const render::GraphicsOptions &opts);

    void load(resources::GameVersion version);
    void loadPortraits(Gender gender);

    void setOnPortraitSelected(const std::function<void(const std::string &)> &fn);
    void setOnCancel(const std::function<void()> &fn);

private:
    struct Portrait {
        std::string resRef;
        std::shared_ptr<render::Texture> image;
    };

    resources::GameVersion _version { resources::GameVersion::KotOR };
    std::vector<Portrait> _portraits;
    int _currentPortrait { 0 };
    std::function<void(const std::string &)> _onPortraitSelected;
    std::function<void()> _onCancel;

    void setButtonColors(const std::string &tag);
    void loadCurrentPortrait();
    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
