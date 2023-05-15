/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/gui/gui.h"

#include "reone/graphics/context.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshes.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/textures.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/resource/gff.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/system/exception/notfound.h"
#include "reone/system/logutil.h"

#include "reone/gui/control/button.h"
#include "reone/gui/control/imagebutton.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/gui/control/panel.h"
#include "reone/gui/control/progressbar.h"
#include "reone/gui/control/scrollbar.h"
#include "reone/gui/control/slider.h"
#include "reone/gui/control/togglebutton.h"
#include "reone/system/exception/validation.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace gui {

void GUI::load(const Gff &gui) {
    debug("Load " + _resRef, LogChannels::gui);

    ControlType type = Control::getType(gui);
    string tag(Control::getTag(gui));

    _rootControl = newControl(type, tag);
    _rootControl->load(gui);
    _controlByTag[tag] = _rootControl.get();

    switch (_scaling) {
    case ScalingMode::Center:
        _rootOffset.x = _screenCenter.x - _resolutionX / 2;
        _rootOffset.y = _screenCenter.y - _resolutionY / 2;
        break;
    case ScalingMode::Stretch:
        stretchControl(*_rootControl);
        break;
    default:
        break;
    }

    const Control::Extent &rootExtent = _rootControl->extent();
    _controlOffset = _rootOffset + glm::ivec2(rootExtent.left, rootExtent.top);

    for (auto &ctrlGffs : gui.getList("CONTROLS")) {
        loadControl(*ctrlGffs);
    }
}

void GUI::stretchControl(Control &control) {
    float aspectX = _options.width / static_cast<float>(_resolutionX);
    float aspectY = _options.height / static_cast<float>(_resolutionY);
    control.stretch(aspectX, aspectY);
}

void GUI::loadControl(const Gff &gffs) {
    ControlType type = Control::getType(gffs);
    string tag(Control::getTag(gffs));
    string parent(Control::getParent(gffs));

    shared_ptr<Control> control(newControl(type, tag));
    if (!control)
        return;

    control->load(gffs);
    if (_hasDefaultHilightColor) {
        control->setHilightColor(_defaultHilightColor);
    }

    ScalingMode scaling = _scaling;
    auto maybeScaling = _scalingByControlTag.find(tag);
    if (maybeScaling != _scalingByControlTag.end()) {
        scaling = maybeScaling->second;
    }
    switch (scaling) {
    case ScalingMode::PositionRelativeToCenter:
        positionRelativeToCenter(*control);
        break;
    case ScalingMode::Stretch:
        stretchControl(*control);
        break;
    default:
        break;
    }

    _controlByTag[tag] = control.get();
    _controls.push_back(move(control));
}

void GUI::positionRelativeToCenter(Control &control) {
    Control::Extent extent(control.extent());
    if (extent.left >= 0.5f * _resolutionX) {
        extent.left = extent.left - _resolutionX + _options.width;
    }
    if (extent.top >= 0.5f * _resolutionY) {
        extent.top = extent.top - _resolutionY + _options.height;
    }
    control.setExtent(move(extent));
}

bool GUI::handle(const SDL_Event &event) {
    switch (event.type) {
    case SDL_KEYDOWN:
        return handleKeyDown(event.key.keysym.scancode);

    case SDL_KEYUP:
        return handleKeyUp(event.key.keysym.scancode);

    case SDL_MOUSEMOTION: {
        glm::ivec2 ctrlCoords(event.motion.x - _controlOffset.x, event.motion.y - _controlOffset.y);
        updateFocus(ctrlCoords.x, ctrlCoords.y);
        if (_focus) {
            _focus->handleMouseMotion(ctrlCoords.x, ctrlCoords.y);
        }
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            _leftMouseDown = true;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (_leftMouseDown && event.button.button == SDL_BUTTON_LEFT) {
            _leftMouseDown = false;
            glm::ivec2 ctrlCoords(event.button.x - _controlOffset.x, event.button.y - _controlOffset.y);
            Control *control = getControlAt(ctrlCoords.x, ctrlCoords.y, [](const Control &ctrl) { return ctrl.isClickable(); });
            if (control) {
                debug("Click " + control->tag(), LogChannels::gui);
                onClick(control->tag());
                return control->handleClick(ctrlCoords.x, ctrlCoords.y);
            }
        }
        break;

    case SDL_MOUSEWHEEL:
        if (_focus && _focus->handleMouseWheel(event.wheel.x, event.wheel.y))
            return true;
        break;
    }

    return false;
}

bool GUI::handleKeyDown(SDL_Scancode key) {
    return false;
}

bool GUI::handleKeyUp(SDL_Scancode key) {
    return false;
}

void GUI::updateFocus(int x, int y) {
    Control *control = getControlAt(x, y, [](const Control &ctrl) { return ctrl.isFocusable(); });
    if (control == _focus)
        return;

    if (_focus) {
        if (_focus->isFocusable()) {
            _focus->setFocus(false);
            onFocusChanged(_focus->tag(), false);
        }
    }
    _focus = control;

    if (control) {
        control->setFocus(true);
        onFocusChanged(control->tag(), true);
    }
}

Control *GUI::getControlAt(int x, int y, const function<bool(const Control &)> &test) const {
    for (auto it = _controls.rbegin(); it != _controls.rend(); ++it) {
        Control *ctrl = (*it).get();
        if (!ctrl->isVisible() || ctrl->isDisabled() || !test(*ctrl))
            continue;

        if (ctrl->extent().contains(x, y)) {
            return ctrl;
        }
    }

    return nullptr;
}

void GUI::update(float dt) {
    for (auto &control : _controls) {
        control->update(dt);
    }
}

void GUI::draw() {
    _graphicsSvc.context.withBlending(BlendMode::Normal, [this]() {
        if (_background) {
            drawBackground();
        }
        if (_rootControl) {
            _rootControl->draw({_options.width, _options.height}, _rootOffset, _rootControl->textLines());
        }
        for (auto &control : _controls) {
            if (!control->isVisible()) {
                continue;
            }
            control->draw({_options.width, _options.height}, _controlOffset, control->textLines());
        }
    });
}

void GUI::drawBackground() {
    _graphicsSvc.textures.bind(*_background);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0));
    transform = glm::scale(transform, glm::vec3(_options.width, _options.height, 1.0f));

    _graphicsSvc.uniforms.setGeneral([this, transform](auto &general) {
        general.resetLocals();
        general.projection = _graphicsSvc.window.getOrthoProjection();
        general.model = move(transform);
    });
    _graphicsSvc.shaders.use(_graphicsSvc.shaders.gui());
    _graphicsSvc.meshes.quad().draw();
}

void GUI::resetFocus() {
    if (_focus) {
        if (_focus->isFocusable()) {
            _focus->setFocus(false);
        }
        onFocusChanged(_focus->tag(), false);
        _focus = nullptr;
    }
}

shared_ptr<Control> GUI::getControl(const string &tag) const {
    for (auto &control : _controls) {
        if (control->tag() == tag) {
            return control;
        }
    }
    throw ValidationException(str(boost::format("Control '%s' not found in GUI '%s'") % tag % _resRef));
}

unique_ptr<Control> GUI::newControl(
    ControlType type,
    string tag) {
    unique_ptr<Control> control;
    switch (type) {
    case ControlType::Panel:
        control = make_unique<Panel>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::Label:
        control = make_unique<Label>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::LabelHilight:
        control = make_unique<ImageButton>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::Button:
        control = make_unique<Button>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::ButtonToggle:
        control = make_unique<ToggleButton>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::Slider:
        control = make_unique<Slider>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::ScrollBar:
        control = make_unique<ScrollBar>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::ProgressBar:
        control = make_unique<ProgressBar>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    case ControlType::ListBox:
        control = make_unique<ListBox>(*this, _sceneGraphs, _graphicsSvc, _resourceSvc.strings);
        break;
    default:
        debug("Unsupported control type: " + to_string(static_cast<int>(type)), LogChannels::gui);
        return nullptr;
    }

    control->setTag(tag);

    return move(control);
}

void GUI::addControl(std::shared_ptr<Control> control) {
    _controls.insert(_controls.begin(), control);
    _controlByTag.insert(make_pair(control->tag(), control.get()));
}

} // namespace gui

} // namespace reone
