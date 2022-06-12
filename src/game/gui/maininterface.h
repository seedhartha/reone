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

#include "../../gui/gui.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace game {

class Object;

class IMainInterface {
public:
    virtual void setHoveredTarget(Object *target) = 0;
    virtual void setSelectedTarget(Object *target) = 0;
};

class MainInterface : public IMainInterface, public gui::Gui {
public:
    MainInterface(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        gui::Gui(
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void init();

    void update(float delta) override;

    void setHoveredTarget(Object *target) override;
    void setSelectedTarget(Object *target) override;

private:
    Object *_hoveredTarget {nullptr};
    Object *_selectedTarget {nullptr};

    // Binding

    gui::Label *_lblNameBg {nullptr};
    gui::Label *_lblName {nullptr};
    gui::Label *_lblHealthBg {nullptr};

    gui::Button *_btnActionDown0 {nullptr};
    gui::Button *_btnActionDown1 {nullptr};
    gui::Button *_btnActionDown2 {nullptr};
    gui::Button *_btnActionDown3 {nullptr};

    gui::Label *_lblTargetReticle {nullptr};
    gui::Label *_lblTargetReticle2 {nullptr};

    // END Binding

    void bindControls();
};

} // namespace game

} // namespace reone
