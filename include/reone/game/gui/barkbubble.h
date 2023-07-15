/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "../gui.h"

#include "reone/gui/control/label.h"
#include "reone/system/timer.h"

namespace reone {

namespace game {

class BarkBubble : public GameGUI {
public:
    BarkBubble(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("barkbubble");
    }

    void update(float dt) override;

    void setBarkText(const std::string &text, float duration);

private:
    struct Controls {
        std::shared_ptr<gui::Label> LBL_BARKTEXT;
    };

    Controls _controls;

    std::string _barkText;
    Timer _timer;

    void preload(gui::IGUI &gui) override;
    void onGUILoaded() override;

    void bindControls() {
        _controls.LBL_BARKTEXT = findControl<gui::Label>("LBL_BARKTEXT");
    }
};

} // namespace game

} // namespace reone
