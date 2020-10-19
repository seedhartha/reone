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

#include "../../system/gui/gui.h"
#include "../../system/resource/types.h"

#include "../types.h"

#include "debugoverlay.h"
#include "selectoverlay.h"

namespace reone {

namespace game {

class HUD : public gui::GUI {
public:
    HUD(resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;

    void render() const override;

    void setContext(const GuiContext &ctx);
    void setOnEquipmentClick(const std::function<void()> &fn);

private:
    DebugOverlay _debug;
    SelectionOverlay _select;

    std::function<void()> _onEquipmentClick;

    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
