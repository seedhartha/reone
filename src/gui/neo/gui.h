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

#include "control/button.h"
#include "control/buttontoggle.h"
#include "control/label.h"
#include "control/labelhilight.h"
#include "control/listbox.h"
#include "control/panel.h"
#include "control/progressbar.h"
#include "control/scrollbar.h"
#include "control/slider.h"

namespace reone {

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace resource {

struct ResourceServices;

class Gff;

} // namespace resource

namespace gui {

namespace neo {

class Gui : boost::noncopyable {
public:
    Gui(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    void load(const std::string &resRef);

    bool handle(const SDL_Event &e);
    void update(float delta);
    void render();

    std::unique_ptr<Panel> newPanel(int id);
    std::unique_ptr<Label> newLabel(int id);
    std::unique_ptr<LabelHilight> newLabelHilight(int id);
    std::unique_ptr<Button> newButton(int id);
    std::unique_ptr<ButtonToggle> newButtonToggle(int id);
    std::unique_ptr<Slider> newSlider(int id);
    std::unique_ptr<ScrollBar> newScrollBar(int id);
    std::unique_ptr<ProgressBar> newProgressBar(int id);
    std::unique_ptr<ListBox> newListBox(int id);

private:
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::unique_ptr<Control> _rootControl;

    std::unique_ptr<Control> loadControl(const resource::Gff &gui);
};

} // namespace neo

} // namespace gui

} // namespace reone
