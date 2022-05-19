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

#include "../control.h"

namespace reone {

namespace resource {

class Gff;

}

namespace gui {

class IControlFactory {
public:
    virtual std::shared_ptr<Control> loadControl(const resource::Gff &gui, const glm::vec4 &scale, int defaultId = -1) = 0;

    virtual std::shared_ptr<Control> newPanel(int id) = 0;
    virtual std::shared_ptr<Control> newLabel(int id) = 0;
    virtual std::shared_ptr<Control> newLabelHilight(int id) = 0;
    virtual std::shared_ptr<Control> newButton(int id) = 0;
    virtual std::shared_ptr<Control> newButtonToggle(int id) = 0;
    virtual std::shared_ptr<Control> newSlider(int id) = 0;
    virtual std::shared_ptr<Control> newScrollBar(int id) = 0;
    virtual std::shared_ptr<Control> newProgressBar(int id) = 0;
    virtual std::shared_ptr<Control> newListBox(int id) = 0;
};

} // namespace gui

} // namespace reone
