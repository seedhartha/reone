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

#include "gui.h"

#include "../../common/timer.h"
#include "../../gui/control/label.h"

namespace reone {

namespace game {

class BarkBubble : public GameGUI {
public:
    BarkBubble(KotOR &game, Services &services);

    void load() override;
    void update(float dt) override;

    void setBarkText(const std::string &text, float duration);

private:
    struct Binding {
        std::shared_ptr<gui::Label> lblBarkText;
    } _binding;

    std::string _barkText;
    Timer _timer;

    void bindControls();
};

} // namespace game

} // namespace reone
