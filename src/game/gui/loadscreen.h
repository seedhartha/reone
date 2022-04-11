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

#include "../../gui/control/label.h"
#include "../../gui/control/progressbar.h"

#include "../gui.h"

namespace reone {

namespace game {

class LoadingScreen : public GameGUI {
public:
    LoadingScreen(Game &game, ServicesView &services);

    void load() override;

    void setImage(const std::string &resRef);
    void setProgress(int progress);

private:
    struct Binding {
        std::shared_ptr<gui::ProgressBar> pbProgress;
        std::shared_ptr<gui::Label> lblHint;
        std::shared_ptr<gui::Label> lblLogo;
        std::shared_ptr<gui::Label> lblLoading;
    } _binding;

    void bindControls();
};

} // namespace game

} // namespace reone
