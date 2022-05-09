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

#include "control.h"
#include "control/factory.h"
#include "control/listbox.h"
#include "guiinterface.h"

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

class Gui : public IGui, public IControlFactory, boost::noncopyable {
public:
    enum class ScaleMode {
        Manual,
        ToRootControl
    };

    Gui(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    void load(const std::string &resRef);

    virtual bool handle(const SDL_Event &e);
    virtual void update(float delta);
    virtual void render();

    // IGui

    std::shared_ptr<Control> loadControl(const resource::Gff &gui, const glm::vec4 &scale, int defaultId = -1) override;

    // END IGui

    // IControlFactory

    std::shared_ptr<Control> newPanel(int id) override;
    std::shared_ptr<Control> newLabel(int id) override;
    std::shared_ptr<Control> newLabelHilight(int id) override;
    std::shared_ptr<Control> newButton(int id) override;
    std::shared_ptr<Control> newButtonToggle(int id) override;
    std::shared_ptr<Control> newSlider(int id) override;
    std::shared_ptr<Control> newScrollBar(int id) override;
    std::shared_ptr<Control> newProgressBar(int id) override;
    std::shared_ptr<Control> newListBox(int id) override;

    // END IControlFactory

protected:
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    ScaleMode _scaleMode {ScaleMode::ToRootControl};

    // Controls

    std::map<int, std::shared_ptr<Control>> _controls;

    Control *_rootControl {nullptr};
    Control *_controlInFocus {nullptr};

    int _highestControlId {-1};

    // END Controls

    Control *findControl(const std::string &tag);
    Control *pickControlAt(int x, int y);

    void enableControl(const std::string &tag);
    void disableControl(const std::string &tag);

    virtual bool handleClick(const Control &control) {
        return false;
    }

    virtual bool handleListBoxItemClick(const ListBox &listBox, const ListBox::Item &item) {
        return false;
    }

    template <class T>
    T *findControl(const std::string &tag) {
        return dynamic_cast<T *>(findControl(tag));
    }

private:
    template <class T>
    std::shared_ptr<Control> newControl(int id) {
        auto control = std::make_shared<T>(id, *this, *this, _graphicsOpt, _graphicsSvc, _resourceSvc);
        _controls[id] = control;
        _highestControlId = std::max(id, _highestControlId);
        return move(control);
    }
};

} // namespace gui

} // namespace reone
