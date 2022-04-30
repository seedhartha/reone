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

#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../graphics/options.h"
#include "../../graphics/services.h"
#include "../../graphics/uniforms.h"
#include "../../resource/gffs.h"
#include "../../resource/services.h"

#include "../types.h"

#include "control/label.h"
#include "control/labelhilight.h"
#include "control/panel.h"
#include "control/progressbar.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace gui {

namespace neo {

bool Gui::handle(const SDL_Event &e) {
    return false;
}

void Gui::update(float delta) {
}

void Gui::render() {
    _graphicsSvc.uniforms.setGeneral([this](auto &u) {
        u.resetGlobals();
        u.projection = glm::ortho(
            0.0f,
            static_cast<float>(_graphicsOpt.width),
            static_cast<float>(_graphicsOpt.height),
            0.0f);
    });
    _rootControl->render();
}

unique_ptr<Gui> Gui::Loader::load(const string &resRef) {
    info("Loading GUI " + resRef, LogChannels::gui);

    auto gui = _resourceSvc.gffs.get(resRef, ResourceType::Gui);
    if (!gui) {
        throw ValidationException("GUI not found: " + resRef);
    }

    return Gui::Builder(_graphicsOpt, _graphicsSvc)
        .rootControl(loadRootControl(*gui))
        .build();
}

unique_ptr<Control> Gui::Loader::loadRootControl(const Gff &gui) {
    auto rootControl = loadControl(gui);

    auto guiControls = gui.getList("CONTROLS");
    for (auto &guiControl : guiControls) {
        auto child = loadControl(*guiControl);
        rootControl->addChild(move(child));
    }

    return move(rootControl);
}

unique_ptr<Control> Gui::Loader::loadControl(const Gff &gui) {
    auto controlType = static_cast<ControlType>(gui.getInt("CONTROLTYPE"));

    unique_ptr<Control::Loader> loader;
    if (controlType == ControlType::Panel) {
        loader = make_unique<Panel::Loader>(_graphicsOpt, _graphicsSvc);
    } else if (controlType == ControlType::Label) {
        loader = make_unique<Label::Loader>(_graphicsOpt, _graphicsSvc);
    } else if (controlType == ControlType::LabelHilight) {
        loader = make_unique<LabelHilight::Loader>(_graphicsOpt, _graphicsSvc);
    } else if (controlType == ControlType::ProgressBar) {
        loader = make_unique<ProgressBar::Loader>(_graphicsOpt, _graphicsSvc);
    } else {
        throw ValidationException("Unsupported control type: " + to_string(static_cast<int>(controlType)));
    }

    return loader->load(gui);
}

} // namespace neo

} // namespace gui

} // namespace reone
