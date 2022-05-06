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

#include "gui.h"

#include "../common/exception/validation.h"
#include "../common/logutil.h"
#include "../graphics/options.h"
#include "../graphics/services.h"
#include "../graphics/uniforms.h"
#include "../graphics/window.h"
#include "../resource/gffs.h"
#include "../resource/services.h"

#include "control/button.h"
#include "control/buttontoggle.h"
#include "control/label.h"
#include "control/labelhilight.h"
#include "control/listbox.h"
#include "control/panel.h"
#include "control/progressbar.h"
#include "control/scrollbar.h"
#include "control/slider.h"
#include "types.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace gui {

void Gui::load(const string &resRef) {
    info("Loading GUI " + resRef, LogChannels::gui);

    auto gui = _resourceSvc.gffs.get(resRef, ResourceType::Gui);
    if (!gui) {
        throw ValidationException("GUI not found: " + resRef);
    }

    glm::vec4 scale;
    if (_scaleMode == ScaleMode::ToRootControl) {
        float rw = static_cast<float>(gui->getStruct("EXTENT")->getInt("WIDTH"));
        float rh = static_cast<float>(gui->getStruct("EXTENT")->getInt("HEIGHT"));
        float sx = _graphicsOpt.width / rw;
        float sy = _graphicsOpt.height / rh;
        scale = glm::vec4(sx, sy, sx, sy);
    } else {
        scale = glm::vec4(1.0f);
    }

    _rootControl = loadControl(*gui, scale).get();

    auto guiControls = gui->getList("CONTROLS");
    for (auto &guiControl : guiControls) {
        loadControl(*guiControl, scale);
    }
    for (auto &guiControl : guiControls) {
        auto id = guiControl->getInt("ID");
        auto &control = _controls.at(id);
        auto parentId = guiControl->getInt("Obj_ParentID");
        auto &parentControl = _controls.at(parentId);
        parentControl->append(*control);
    }

}

shared_ptr<Control> Gui::loadControl(const Gff &gui, const glm::vec4 &scale, int defaultId) {
    auto id = gui.getInt("ID", defaultId);
    auto type = static_cast<ControlType>(gui.getInt("CONTROLTYPE"));

    shared_ptr<Control> control;
    if (type == ControlType::Panel) {
        control = newPanel(id);
    } else if (type == ControlType::Label) {
        control = newLabel(id);
    } else if (type == ControlType::LabelHilight) {
        control = newLabelHilight(id);
    } else if (type == ControlType::Button) {
        control = newButton(id);
    } else if (type == ControlType::ButtonToggle) {
        control = newButtonToggle(id);
    } else if (type == ControlType::Slider) {
        control = newSlider(id);
    } else if (type == ControlType::ScrollBar) {
        control = newScrollBar(id);
    } else if (type == ControlType::ProgressBar) {
        control = newProgressBar(id);
    } else if (type == ControlType::ListBox) {
        control = newListBox(id);
    } else {
        throw ValidationException("Unsupported control type: " + to_string(static_cast<int>(type)));
    }
    control->load(gui, scale);

    return move(control);
}

bool Gui::handle(const SDL_Event &e) {
    if (!_rootControl) {
        return false;
    }
    if (_rootControl->handle(e)) {
        return true;
    }

    if (e.type == SDL_MOUSEMOTION) {
        auto control = pickControlAt(e.motion.x, e.motion.y);
        if (_controlInFocus != control) {
            if (_controlInFocus) {
                _controlInFocus->setInFocus(false);
            }
            if (control) {
                control->setInFocus(true);
            }
            _controlInFocus = control;
            return true;
        }
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (_controlInFocus) {
            debug("Control clicked on: " + to_string(_controlInFocus->id()) + "[" + _controlInFocus->tag() + "]", LogChannels::gui);
            if (handleClick(*_controlInFocus)) {
                return true;
            }
            if (ListBox::isListBoxItem(*_controlInFocus)) {
                int listBoxId = ListBox::listBoxIdFromControl(*_controlInFocus);
                int itemSlotIdx = ListBox::itemSlotIndexFromControl(*_controlInFocus);
                auto &listBox = static_cast<ListBox &>(*_controls.at(listBoxId));
                auto &item = listBox.itemBySlotIndex(itemSlotIdx);
                debug("List box item clicked on: " + to_string(listBoxId) + ":" + to_string(itemSlotIdx) + "[" + item.text + "]", LogChannels::gui);
                if (handleListBoxItemClick(listBox, item)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Gui::update(float delta) {
    if (!_rootControl) {
        return;
    }
    _rootControl->update(delta);
}

void Gui::render() {
    if (!_rootControl) {
        return;
    }
    _graphicsSvc.uniforms.setGeneral([this](auto &u) {
        u.resetGlobals();
        u.projection = _graphicsSvc.window.getOrthoProjection();
    });
    _rootControl->render();
}

Control *Gui::findControl(const string &tag) {
    return _rootControl->findControlByTag(tag);
}

Control *Gui::pickControlAt(int x, int y) {
    return _rootControl->pickControlAt(x, y);
}

void Gui::enableControl(const string &tag) {
    auto control = findControl(tag);
    if (control) {
        control->setEnabled(true);
    }
}

void Gui::disableControl(const string &tag) {
    auto control = findControl(tag);
    if (control) {
        control->setEnabled(false);
    }
}

shared_ptr<Control> Gui::newPanel(int id) {
    return newControl<Panel>(id);
}

shared_ptr<Control> Gui::newLabel(int id) {
    return newControl<Label>(id);
}

shared_ptr<Control> Gui::newLabelHilight(int id) {
    return newControl<LabelHilight>(id);
}

shared_ptr<Control> Gui::newButton(int id) {
    return newControl<Button>(id);
}

shared_ptr<Control> Gui::newButtonToggle(int id) {
    return newControl<ButtonToggle>(id);
}

shared_ptr<Control> Gui::newSlider(int id) {
    return newControl<Slider>(id);
}

shared_ptr<Control> Gui::newScrollBar(int id) {
    return newControl<ScrollBar>(id);
}

shared_ptr<Control> Gui::newProgressBar(int id) {
    return newControl<ProgressBar>(id);
}

shared_ptr<Control> Gui::newListBox(int id) {
    return newControl<ListBox>(id);
}

} // namespace gui

} // namespace reone
