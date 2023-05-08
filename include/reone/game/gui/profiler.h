/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/gui/gui.h"

namespace reone {

namespace gui {

class ListBox;
class Plotter;

}

namespace game {

class Profiler;

class ProfilerGui : public gui::Gui {
public:
    ProfilerGui(
        Profiler &profiler,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        gui::Gui(
            graphicsOpt,
            graphicsSvc,
            resourceSvc),
        _profiler(profiler) {
    }

    void init();

    void update(float delta) override;

    bool isEnabled() const {
        return _enabled;
    }

    void setEnabled(bool enabled) {
        _enabled = enabled;
    }

private:
    Profiler &_profiler;

    bool _enabled {false};

    // Binding

    gui::ListBox *_lbText;
    gui::Plotter *_pltFrameTimes;

    // END Binding
};

} // namespace game

} // namespace reone
