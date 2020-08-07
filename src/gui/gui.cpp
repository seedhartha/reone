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

using namespace std;
using namespace std::placeholders;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

GUI::GUI(const GraphicsOptions &opts) : _opts(opts) {
    _screenCenter.x = 0.5f * _opts.width;
    _screenCenter.y = 0.5f * _opts.height;
}

void GUI::load(const string &resRef, BackgroundType background) {
    shared_ptr<GffStruct> gui(ResMan.findGFF(resRef, ResourceType::Gui));
    assert(gui);

    loadBackground(background);

    _rootControl = Control::makeControl(*gui);

    switch (_scaling) {
        case ScalingMode::Center:
            _controlOffset.x = _screenCenter.x - 0.5f * _resolutionX;
            _controlOffset.y = _screenCenter.y - 0.5f * _resolutionY;
            break;
        case ScalingMode::Stretch:
            stretchControl(*_rootControl);
            break;
        default:
            break;
    }

    const Control::Extent &rootExtent = _rootControl->extent();
    _controlOffset.x += rootExtent.left;
    _controlOffset.y += rootExtent.top;

    for (auto &ctrlGffs : gui->getList("CONTROLS")) {
        unique_ptr<Control> control(Control::makeControl(ctrlGffs));
        if (!control) continue;

        switch (_scaling) {
            case ScalingMode::PositionRelativeToCenter:
                positionRelativeToCenter(*control);
                break;
            case ScalingMode::Stretch:
                stretchControl(*control);
                break;
            default:
                break;
        }
        control->setOnClick(bind(&GUI::onClick, this, _1));
        _controls.push_back(move(control));
    }
}

void GUI::positionRelativeToCenter(Control &control) {
    Control::Extent extent(control.extent());
    if (extent.left >= 0.5f * _resolutionX) {
        extent.left = extent.left - _resolutionX + _opts.width;
    }
    if (extent.top >= 0.5f * _resolutionY) {
        extent.top = extent.top - _resolutionY + _opts.height;
    }
    control.setExtent(move(extent));
}

void GUI::stretchControl(Control &control) {
    float aspectX = _opts.width / static_cast<float>(_resolutionX);
    float aspectY = _opts.height / static_cast<float>(_resolutionY);
    control.stretch(aspectX, aspectY);
}

void GUI::loadBackground(BackgroundType type) {
    string resRef;

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

bool GUI::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_MOUSEMOTION: {
            glm::vec2 ctrlCoords(event.motion.x - _controlOffset.x, event.motion.y - _controlOffset.y);
            updateFocus(ctrlCoords.x, ctrlCoords.y);
            if (_focus) {
                _focus->handleMouseMotion(ctrlCoords.x, ctrlCoords.y);
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
            if (_focus && event.button.button == SDL_BUTTON_LEFT && event.button.clicks == 1) {
                debug("GUI: control clicked on: " + _focus->tag());
                glm::vec2 ctrlCoords(event.button.x - _controlOffset.x, event.button.y - _controlOffset.y);
                return _focus->handleClick(ctrlCoords.x, ctrlCoords.y);
            }
            break;

        case SDL_MOUSEWHEEL:
            if (_focus && _focus->handleMouseWheel(event.wheel.x, event.wheel.y)) return true;
            break;
    }

    return false;
}

void GUI::updateFocus(int x, int y) {
    if (_focus) {
        _focus->setFocus(false);
        _focus = nullptr;
    }
    for (auto it = _controls.rbegin(); it != _controls.rend(); ++it) {
        shared_ptr<Control> control(*it);
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
    if (_background) _background->initGL();

    for (auto &control : _controls) {
        control->initGL();
    }
}

void GUI::render() const {
    if (_background) renderBackground();

    for (auto &control : _controls) {
        control->render(_controlOffset);
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

void GUI::showControl(const string &tag) {
    for (auto &control : _controls) {
        if (control->tag() == tag) {
            control->setVisible(true);
            return;
        }
    }
}

void GUI::hideControl(const string &tag) {
    for (auto &control : _controls) {
        if (control->tag() == tag) {
            control->setVisible(false);
            return;
        }
    }
}

Control &GUI::getControl(const string &tag) const {
    auto it = find_if(
        _controls.begin(),
        _controls.end(),
        [&tag](const shared_ptr<Control> &ctrl) { return ctrl->tag() == tag; });

    return **it;
}

void GUI::onClick(const string &control) {
}

void GUI::onItemClicked(const string &control, const string &item) {
}

} // namespace gui

} // namespace reone
