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

#include "../common/guardutil.h"
#include "../common/logutil.h"
#include "../graphics/context.h"
#include "../graphics/mesh/mesh.h"
#include "../graphics/mesh/meshes.h"
#include "../graphics/shader/shaders.h"
#include "../graphics/texture/texture.h"
#include "../graphics/texture/textures.h"
#include "../graphics/window.h"
#include "../resource/gffstruct.h"
#include "../resource/resources.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

GUI::GUI(
    GraphicsOptions options,
    Context &context,
    Features &features,
    Fonts &fonts,
    Materials &materials,
    Meshes &meshes,
    PBRIBL &pbrIbl,
    Shaders &shaders,
    Textures &textures,
    Window &window,
    Resources &resources,
    Strings &strings
) :
    _options(move(options)),
    _context(context),
    _features(features),
    _fonts(fonts),
    _materials(materials),
    _meshes(meshes),
    _pbrIbl(pbrIbl),
    _shaders(shaders),
    _textures(textures),
    _window(window),
    _resources(resources),
    _strings(strings) {

    _aspect = options.width / static_cast<float>(options.height);
    _screenCenter.x = options.width / 2;
    _screenCenter.y = options.height / 2;
}

void GUI::load() {
    ensureNotEmpty(_resRef, "resRef");

    debug("Load " + _resRef, LogChannels::gui);

    shared_ptr<GffStruct> gui(_resources.getGFF(_resRef, ResourceType::Gui));
    ControlType type = Control::getType(*gui);
    string tag(Control::getTag(*gui));

    _rootControl = Control::of(this, type, tag);
    _rootControl->load(*gui);
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

    for (auto &ctrlGffs : gui->getList("CONTROLS")) {
        loadControl(*ctrlGffs);
    }
}

void GUI::stretchControl(Control &control) {
    float aspectX = _options.width / static_cast<float>(_resolutionX);
    float aspectY = _options.height / static_cast<float>(_resolutionY);
    control.stretch(aspectX, aspectY);
}

void GUI::loadControl(const GffStruct &gffs) {
    ControlType type = Control::getType(gffs);
    string tag(Control::getTag(gffs));
    string parent(Control::getParent(gffs));

    shared_ptr<Control> control(Control::of(this, type, tag));
    if (!control) return;

    preloadControl(*control);
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

void GUI::preloadControl(Control &control) {
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
            if (_focus && _focus->handleMouseWheel(event.wheel.x, event.wheel.y)) return true;
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
    if (control == _focus) return;

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
        if (!ctrl->isVisible() || ctrl->isDisabled() || !test(*ctrl)) continue;

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
    if (_background) drawBackground();
    if (_rootControl) _rootControl->draw(_rootOffset, _rootControl->textLines());

    for (auto &control : _controls) {
        control->draw(_controlOffset, control->textLines());
    }
}

void GUI::draw3D() {
    for (auto &control : _controls) {
        control->draw3D(_controlOffset);
    }
}

void GUI::drawBackground() {
    _context.setActiveTextureUnit(TextureUnits::diffuseMap);
    _background->bind();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0));
    transform = glm::scale(transform, glm::vec3(_options.width, _options.height, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _window.getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
    _meshes.quad().draw();
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
        if (control->tag() == tag) return control;
    }
    warn(boost::format("Control '%s' not found in GUI '%s'") % tag % _resRef);
    return shared_ptr<Control>();
}

} // namespace gui

} // namespace reone
