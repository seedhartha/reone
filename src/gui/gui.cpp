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

    auto controlById = unordered_map<int, shared_ptr<Control>>();
    auto rootControl = shared_ptr<Control>(loadControl(*gui, scale));
    controlById[-1] = rootControl;

    auto guiControls = gui->getList("CONTROLS");
    for (auto &guiControl : guiControls) {
        auto control = loadControl(*guiControl, scale);
        controlById[control->id()] = move(control);
    }
    for (auto &guiControl : guiControls) {
        auto id = guiControl->getInt("ID");
        auto control = controlById.at(id);
        auto parentId = guiControl->getInt("Obj_ParentID");
        auto parentControl = controlById.at(parentId);
        parentControl->append(control);
    }

    _rootControl = move(rootControl);
}

unique_ptr<Control> Gui::loadControl(const Gff &gui, const glm::vec4 &scale) {
    auto id = gui.getInt("ID", -1);
    auto type = static_cast<ControlType>(gui.getInt("CONTROLTYPE"));

    unique_ptr<Control> control;
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
        }
    }
    return false;
}

void Gui::update(float delta) {
    _rootControl->update(delta);
}

void Gui::render() {
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

unique_ptr<Panel> Gui::newPanel(int id) {
    return make_unique<Panel>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Label> Gui::newLabel(int id) {
    return make_unique<Label>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<LabelHilight> Gui::newLabelHilight(int id) {
    return make_unique<LabelHilight>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Button> Gui::newButton(int id) {
    return make_unique<Button>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<ButtonToggle> Gui::newButtonToggle(int id) {
    return make_unique<ButtonToggle>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<Slider> Gui::newSlider(int id) {
    return make_unique<Slider>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<ScrollBar> Gui::newScrollBar(int id) {
    return make_unique<ScrollBar>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<ProgressBar> Gui::newProgressBar(int id) {
    return make_unique<ProgressBar>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

unique_ptr<ListBox> Gui::newListBox(int id) {
    return make_unique<ListBox>(id, _graphicsOpt, _graphicsSvc, _resourceSvc);
}

} // namespace gui

} // namespace reone
