/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../core/log.h"
#include "../resources/manager.h"
#include "../render/mesh/guiquad.h"
#include "../render/shadermanager.h"

#include "control/button.h"
#include "control/label.h"
#include "control/listbox.h"
#include "control/panel.h"

using namespace std::placeholders;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

static const char kFontResRef[] = "fnt_d16x16b";

GUI::GUI(const GraphicsOptions &opts) : _opts(opts) {
    _screenCenter = glm::vec3(0.5f * _opts.width, 0.5f * _opts.height, 0.0f);
}

void GUI::load(const std::string &resRef, BackgroundType background) {
    ResourceManager &resources = ResourceManager::instance();
    std::shared_ptr<GffStruct> gui(resources.findGFF(resRef, ResourceType::Gui));
    assert(gui);

    loadFont();
    loadBackground(background);

    _rootControl = makeControl(*gui);

    const Control::Extent &rootExtent = _rootControl->extent();
    float x = 0.0f;
    float y = 0.0f;

    switch (_scaling) {
        case ScalingMode::Center:
            x = _screenCenter.x - 0.5f * rootExtent.width;
            y = _screenCenter.y - 0.5f * rootExtent.height;
            _controlTransform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
            break;
        case ScalingMode::Scale:
            x = _opts.width / static_cast<float>(rootExtent.width);
            y = _opts.height / static_cast<float>(rootExtent.height);
            _controlTransform = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, 1.0f));
            break;
    }

    for (auto &ctrlGffs : gui->getList("CONTROLS")) {
        std::unique_ptr<Control> control(makeControl(ctrlGffs));
        if (!control) continue;

        control->setOnClick(std::bind(&GUI::onClick, this, _1));
        _controls.push_back(std::move(control));
    }
}

void GUI::loadFont() {
    ResourceManager &resources = ResourceManager::instance();
    _font = resources.findFont(kFontResRef);
    assert(_font);
}

void GUI::loadBackground(BackgroundType type) {
    std::string resRef;

    if ((_opts.width == 1600 && _opts.height == 1200) ||
        (_opts.width == 1280 && _opts.height == 960) ||
        (_opts.width == 1024 && _opts.height == 768) ||
        (_opts.width == 800 && _opts.height == 600)) {

        resRef = str(boost::format("%dx%d") % _opts.width % _opts.height);
    } else {
        resRef = "1600x1200";
    }

    switch (type) {
        case BackgroundType::Menu:
            resRef += "back";
            break;
        default:
            return;
    }

    _background = ResourceManager::instance().findTexture(resRef, TextureType::Diffuse);
    assert(_background);
}

std::unique_ptr<Control> GUI::makeControl(const GffStruct &gffs) const {
    ControlType controlType = static_cast<ControlType>(gffs.getInt("CONTROLTYPE"));
    std::unique_ptr<Control> control;

    switch (controlType) {
        case ControlType::Panel:
            control = std::make_unique<Panel>();
            break;
        case ControlType::Label:
            control = std::make_unique<Label>();
            break;
        case ControlType::Button:
            control = std::make_unique<Button>();
            break;
        case ControlType::ListBox:
            control = std::make_unique<ListBox>();
            break;
        default:
            warn("Unsupported control type: " + std::to_string(static_cast<int>(controlType)));
            return nullptr;
    }

    control->load(gffs);

    return std::move(control);
}

bool GUI::handle(const SDL_Event &event) {
    glm::vec2 ctrlCoords(0.0f);

    switch (event.type) {
        case SDL_MOUSEMOTION:
            ctrlCoords = screenToControlCoords(event.motion.x, event.motion.y);
            updateFocus(ctrlCoords.x, ctrlCoords.y);
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT && event.button.clicks == 1 && _focus) {
                debug("GUI: control clicked on: " + _focus->tag());
                ctrlCoords = screenToControlCoords(event.button.x, event.button.y);
                return _focus->handleClick(ctrlCoords.x, ctrlCoords.y);
            }
            break;

        case SDL_MOUSEWHEEL:
            if (_focus && _focus->handleMouseWheel(event.wheel.x, event.wheel.y)) return true;
            break;
    }

    return false;
}

glm::vec2 GUI::screenToControlCoords(int x, int y) const {
    return glm::inverse(_controlTransform) * glm::vec4(x, y, 0.0f, 1.0f);
}

void GUI::updateFocus(int x, int y) {
    if (_focus) {
        _focus->setFocus(false);
        _focus = nullptr;
    }
    for (auto it = _controls.rbegin(); it != _controls.rend(); ++it) {
        std::shared_ptr<Control> control(*it);
        if (!control->visible()) continue;

        const Control::Extent &extent = control->extent();
        if (extent.contains(x, y)) {
            _focus = control;
            _focus->setFocus(true);
            return;
        }
    }
}

void GUI::initGL() {
    _font->initGL();

    if (_background) _background->initGL();
    if (_rootControl) _rootControl->initGL();

    for (auto &control : _controls) {
        control->initGL();
    }
}

void GUI::render() const {
    if (_background) renderBackground();
    if (_rootControl) _rootControl->render(_controlTransform);

    for (auto &control : _controls) {
        control->render(_controlTransform);
    }
}

void GUI::renderBackground() const {
    glm::mat4 transform(glm::scale(glm::mat4(1.0f), glm::vec3(_opts.width, _opts.height, 1.0f)));

    ShaderManager &shaders = ShaderManager::instance();
    shaders.activate(ShaderProgram::BasicDiffuse);
    shaders.setUniform("model", transform);
    shaders.setUniform("color", glm::vec3(1.0f));
    shaders.setUniform("alpha", 1.0f);

    glActiveTexture(0);
    _background->bind();

    GUIQuad::instance().render(GL_TRIANGLES);

    _background->unbind();
    shaders.deactivate();
}

void GUI::showControl(const std::string &tag) {
    for (auto &control : _controls) {
        if (control->tag() == tag) {
            control->setVisible(true);
            return;
        }
    }
}

void GUI::hideControl(const std::string &tag) {
    for (auto &control : _controls) {
        if (control->tag() == tag) {
            control->setVisible(false);
            return;
        }
    }
}

Control &GUI::getControl(const std::string &tag) const {
    auto it = std::find_if(
        _controls.begin(),
        _controls.end(),
        [&tag](const std::shared_ptr<Control> &ctrl) { return ctrl->tag() == tag; });

    return **it;
}

void GUI::onClick(const std::string &control) {
}

void GUI::onItemClicked(const std::string &control, const std::string &item) {
}

} // namespace gui

} // namespace reone
